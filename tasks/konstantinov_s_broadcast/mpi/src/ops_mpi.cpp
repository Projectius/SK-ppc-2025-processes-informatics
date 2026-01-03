#include "konstantinov_s_broadcast/mpi/include/ops_mpi.hpp"

#include <mpi.h>
// #include <numeric>
#include <cstring>
#include <type_traits>
#include <vector>
// #include<iostream>

#include "konstantinov_s_broadcast/common/include/common.hpp"
// #include "util/include/util.hpp"

namespace konstantinov_s_broadcast {

template <typename T>
KonstantinovSBroadcastMPI<T>::KonstantinovSBroadcastMPI(const InType &in) {
  this->SetTypeOfTask(GetStaticTypeOfTask());
  this->GetInput() = in;
  this->GetOutput().resize(this->GetInput().size());
}

template <typename T>
bool KonstantinovSBroadcastMPI<T>::ValidationImpl() {
  // std::cout << "\t\tValidation mpi\n";
  return !this->GetInput().empty();
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

  MPI_Datatype mpi_type = get_mpi_type<T>();

  auto &buffer = this->GetInput();
  int elem_count = static_cast<int>(buffer.size());
  T *data = buffer.data();

  int root_prank = pcount - 1;
  int k = std::min(4, pcount);  // k-арное дерево

  // ранг процесса в дереве
  int tree_rank = (prank - root_prank + pcount) % pcount;

  // родитель в дереве
  int parent_tree_rank = (tree_rank == 0) ? -1 : (tree_rank - 1) / k;
  int parent_prank = (parent_tree_rank < 0) ? -1 : (parent_tree_rank + root_prank) % pcount;

  // приём данных от родителя (если не корень)
  if (tree_rank != 0) {
    // std::cout << "Rank " << prank << " (t " << tree_rank << ") recv from "<< parent_prank << " (t " <<
    // parent_tree_rank << ")\n";
    MPI_Recv(data, elem_count, mpi_type, parent_prank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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

  // рассылка
  for (int ch : child_pranks) {
    // std::cout << "Rank " << prank << " (t " << tree_rank << ") send to " << ch << "\n";
    MPI_Send(data, elem_count, mpi_type, ch, 0, MPI_COMM_WORLD);
  }

  this->GetOutput() = buffer;
  return true;
}

template <typename T>
bool KonstantinovSBroadcastMPI<T>::PostProcessingImpl() {
  return true;
}
template class KonstantinovSBroadcastMPI<ETypeInt>;
template class KonstantinovSBroadcastMPI<ETypeFloat>;
template class KonstantinovSBroadcastMPI<ETypeDouble>;

}  // namespace konstantinov_s_broadcast
