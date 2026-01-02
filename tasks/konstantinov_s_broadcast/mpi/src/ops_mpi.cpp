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
  int parent_tree_rank = (tree_rank - 1) / k;
  int parent_prank = (parent_tree_rank < 0)
      ? MPI_ANY_SOURCE
      : (parent_tree_rank + root_prank) % pcount;


  return true;
}

template <typename T>
bool KonstantinovSBroadcastMPI<T>::PostProcessingImpl() {
  return true;
}

}  // namespace konstantinov_s_broadcast
