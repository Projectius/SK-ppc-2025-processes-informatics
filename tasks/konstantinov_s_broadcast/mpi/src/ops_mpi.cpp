#include "konstantinov_s_broadcast/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <cstring>
#include <vector>
// #include<iostream>
#include <algorithm>

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
  int prank = 0;
  int pcount = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &prank);
  MPI_Comm_size(MPI_COMM_WORLD, &pcount);

  const int TAG_COUNT = 1;
  const int TAG_DATA = 2;

  MPI_Datatype mpi_type = GetMpiType<T>();

  T *odata = nullptr;

  int root_prank = pcount - 1;
  int k = std::min(2, pcount);  // k-арное дерево

  // ранг процесса в дереве
  int tree_rank = (prank - root_prank + pcount) % pcount;

  // родитель в дереве
  int parent_tree_rank = (tree_rank == 0) ? -1 : (tree_rank - 1) / k;
  int parent_prank = (parent_tree_rank < 0) ? -1 : (parent_tree_rank + root_prank) % pcount;

  int recv_count = 0;
  // приём данных от родителя (корень читает доступные ему данные)
  if (tree_rank == 0) {
    auto &ibuffer = this->GetInput();
    int elem_count = static_cast<int>(ibuffer.size());
    odata = ibuffer.data();
    recv_count = elem_count;
    this->GetOutput() = ibuffer;
  } else {
    // std::cout << "Rank " << prank << " (t " << tree_rank << ") recv from "<< parent_prank << " (t " <<
    // parent_tree_rank << ")\n";
    MPI_Recv(&recv_count, 1, MPI_INT, parent_prank, TAG_COUNT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    odata = new T[recv_count];
    MPI_Recv(odata, recv_count, mpi_type, parent_prank, TAG_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    memcpy(this->GetOutput().data(), odata, recv_count * sizeof(T));
  }

  // вычисляем потомков и рассылаем
  // std::vector<int> child_pranks;
  for (int i = 1; i <= k; ++i) {
    int child_tree_rank = (k * tree_rank) + i;
    if (child_tree_rank < pcount) {
      int child_prank = (child_tree_rank + root_prank) % pcount;
      // std::cout << "Rank " << prank << " (t " << tree_rank << ") send to " << ch << "\n";
      MPI_Send(&recv_count, 1, MPI_INT, child_prank, TAG_COUNT, MPI_COMM_WORLD);
      MPI_Send(odata, recv_count, mpi_type, child_prank, TAG_DATA, MPI_COMM_WORLD);
      // child_pranks.push_back(child_prank);
    }
  }
  if (tree_rank != 0) {
    delete[] odata;
  }

  // this->GetOutput() = ibuffer;
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
