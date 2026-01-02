#include "konstantinov_s_broadcast/mpi/include/ops_mpi.hpp"

#include <mpi.h>
// #include <numeric>
#include <cstring>
#include <vector>
// #include<iostream>

#include "konstantinov_s_broadcast/common/include/common.hpp"
// #include "util/include/util.hpp"

namespace konstantinov_s_broadcast {

KonstantinovSBroadcastMPI::KonstantinovSBroadcastMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KonstantinovSBroadcastMPI::ValidationImpl() {
  // std::cout << "\t\tValidation mpi\n";
  return !GetInput().empty();
}

bool KonstantinovSBroadcastMPI::PreProcessingImpl() {
  return true;
}

void KonstantinovSBroadcastMPI::CountSignChange(int &res, const EType *data, int start, int iterations) {
  for (int i = start; i < iterations; i++) {
    res += static_cast<int>((data[i] > 0) != (data[i + 1] > 0));
  }

  // for (int i = elemcount - rem; i < elemcount - 1; i++) {
  //       // std::cout<<sendbuf[i];
  //       local_res += static_cast<int>((sendbuf[i] > 0) != (sendbuf[i + 1] > 0));
  //     }

  // for (int i = 0; i < step; i++) {
  //   local_res += static_cast<int>((recbuf[i] > 0) != (recbuf[i + 1] > 0));
  // }
}

template <typename T>
bool KonstantinovSBroadcastMPI::RunImpl() {
  
  return true;
}

bool KonstantinovSBroadcastMPI::PostProcessingImpl() {
  return true;
}

}  // namespace konstantinov_s_broadcast
