#include "konstantinov_s_broadcast/mpi/include/ops_mpi.hpp"


// #include <numeric>
#include <cstring>
#include <vector>
// #include<iostream>

#include "konstantinov_s_broadcast/common/include/common.hpp"
// #include "util/include/util.hpp"

namespace konstantinov_s_broadcast {

  template <typename T>
KonstantinovSBroadcastMPI<T>::KonstantinovSBroadcastMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

template <typename T>
bool KonstantinovSBroadcastMPI<T>::ValidationImpl() {
  // std::cout << "\t\tValidation mpi\n";
  return !GetInput().empty();
}

template <typename T>
bool KonstantinovSBroadcastMPI<T>::PreProcessingImpl() {
  return true;
}

template <typename T>
bool KonstantinovSBroadcastMPI<T>::RunImpl() {
  int prank = 0, pcount = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &prank);
  MPI_Comm_size(MPI_COMM_WORLD, &pcount);

  MPI_Datatype mpi_type = MPIDatatype<T>::value();

  auto& buffer = this->GetInput();
  int elem_count = static_cast<int>(buffer.size());
  T* data = buffer.data();

  constexpr int root_prank = 0;
  constexpr int k = 4;  // k-арное дерево

  // ранг процесса в дереве (root = 0)
  int tree_rank = (prank - root_prank + pcount) % pcount;

  // родитель в дереве
  int parent_tree_rank = (tree_rank == 0) ? -1 : (tree_rank - 1) / k;
  int parent_prank = (parent_tree_rank < 0)
      ? MPI_ANY_SOURCE
      : (parent_tree_rank + root_prank) % pcount;

  // приём данных от родителя
  if (tree_rank != 0) {
    MPI_Recv(data, elem_count, mpi_type,
             parent_prank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  // вычисляем потомков
  std::vector<int> child_pranks;
  for (int i = 1; i <= k; ++i) {
    int child_tree_rank = (k * tree_rank) + i;
    if (child_tree_rank < pcount) {
      int child_prank = (child_tree_rank + root_prank) % pcount;
      child_pranks.push_back(child_prank);
    }
  }

  // неблокирующая рассылка детям
  std::vector<MPI_Request> send_requests(child_pranks.size());
  for (std::size_t i = 0; i < child_pranks.size(); ++i) {
    MPI_Isend(data, elem_count, mpi_type,
              child_pranks[i], 0, MPI_COMM_WORLD,
              &send_requests[i]);
  }

  if (!send_requests.empty()) {
    MPI_Waitall(static_cast<int>(send_requests.size()),
                send_requests.data(), MPI_STATUSES_IGNORE);
  }

  this->GetOutput() = buffer;
  return true;
}

template <typename T>
bool KonstantinovSBroadcastMPI<T>::PostProcessingImpl() {
  return true;
}

}  // namespace konstantinov_s_broadcast
