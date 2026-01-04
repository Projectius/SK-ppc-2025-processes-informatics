#include "konstantinov_s_radix/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>
// #include <numeric>
// #include <cstring>
#include <vector>
// #include<iostream>

#include "konstantinov_s_radix/common/include/common.hpp"
// #include "util/include/util.hpp"

namespace konstantinov_s_radix {

KonstantinovSRadixMPI::KonstantinovSRadixMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  // GetOutput() = 0;
  SetTypeOfTask(GetStaticTypeOfTask());
  // копируем данные интуитивно (внешний код может переиспользовать in)
  // GetInput().clear();
  // GetInput().insert(GetInput().end(), in.begin(), in.end());
  // GetOutput().clear();
}

bool KonstantinovSRadixMPI::ValidationImpl() {
  // std::cout << "\t\tValidation mpi\n";
  return true;
}

bool KonstantinovSRadixMPI::PreProcessingImpl() {
  return true;
}

void KonstantinovSRadixMPI::LocalRadixPass(InType &block) {
  if (block.size() <= 1) {
    return;
  }

  size_t n = block.size();
  InType tmp(n);

  constexpr int kBytes = 4;
  for (int shift = 0; shift < kBytes * 8; shift += 8) {
    std::array<size_t, 256> cnt{};
    for (size_t i = 0; i < n; ++i) {
      auto u = static_cast<uint32_t>(block[i]) ^ 0x80000000U;
      auto key = static_cast<uint8_t>((u >> shift) & 0xFFU);
      ++cnt.at(key);
    }
    size_t prefix = 0;
    for (auto &cur_val : cnt) {
      size_t cur = cur_val;
      cur_val = prefix;
      prefix += cur;
    }
    for (size_t i = 0; i < n; ++i) {
      auto u = static_cast<uint32_t>(block[i]) ^ 0x80000000U;
      auto key = static_cast<uint8_t>((u >> shift) & 0xFFU);
      tmp.at(cnt.at(key)++) = block.at(i);
    }
    block.swap(tmp);
  }
}

// обмен и простое слияние попарно
void KonstantinovSRadixMPI::PairwiseMergeExchange(InType &local_block, int prank, int comm_sz) {
  for (int step = 1; step < comm_sz; step <<= 1) {
    if ((prank % (2 * step)) == 0) {
      int partner = prank + step;
      if (partner < comm_sz) {
        // получает длину
        int remote_len = 0;
        MPI_Recv(&remote_len, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        InType remote;
        if (remote_len > 0) {
          remote.resize(static_cast<size_t>(remote_len));
          MPI_Recv(remote.data(), remote_len, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        InType merged;
        merged.reserve(local_block.size() + remote.size());
        std::ranges::merge(local_block, remote, std::back_inserter(merged));
        local_block.swap(merged);
      }
    } else {
      int target = prank - step;
      int my_len = static_cast<int>(local_block.size());

      MPI_Send(&my_len, 1, MPI_INT, target, 0, MPI_COMM_WORLD);
      if (my_len > 0) {
        MPI_Send(local_block.data(), my_len, MPI_INT, target, 0, MPI_COMM_WORLD);
      }

      local_block.clear();
      break;
    }
  }
}

bool KonstantinovSRadixMPI::RunImpl() {
  int prank = 0;
  int comm_sz = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &prank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  int total_n = 0;
  if (prank == 0) {
    total_n = static_cast<int>(GetInput().size());
  }
  MPI_Bcast(&total_n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // sendcounts displs
  std::vector<int> sendcounts(comm_sz);
  std::vector<int> displs(comm_sz);
  int base = total_n / comm_sz;
  int rem = total_n % comm_sz;
  int acc = 0;
  for (int i = 0; i < comm_sz; ++i) {
    sendcounts[i] = base + (i < rem ? 1 : 0);
    displs[i] = acc;
    acc += sendcounts[i];
  }

  int my_count = sendcounts[prank];
  InType local_block;
  if (my_count > 0) {
    local_block.resize(static_cast<size_t>(my_count));
  }

  // my_count == 0 ==> nullptr in recvbuf
  MPI_Scatterv(GetInput().data(), sendcounts.data(), displs.data(), MPI_INT,
               (my_count > 0 ? local_block.data() : nullptr), my_count, MPI_INT, 0, MPI_COMM_WORLD);

  LocalRadixPass(local_block);
  PairwiseMergeExchange(local_block, prank, comm_sz);

  if (prank == 0) {
    // std::cout<<"CALCULATED\n";
    // for(int i=0;i<local_block.size();i++)
    //   std::cout<<local_block[i]<<"\n";
    GetOutput() = std::move(local_block);
  }
  return true;
}

bool KonstantinovSRadixMPI::PostProcessingImpl() {
  return true;
}

}  // namespace konstantinov_s_radix
