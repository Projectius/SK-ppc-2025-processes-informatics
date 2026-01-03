#pragma once
#include <mpi.h>

#include <vector>

#include "konstantinov_s_broadcast/common/include/common.hpp"
#include "task/include/task.hpp"

namespace konstantinov_s_broadcast {

template <typename T>
class KonstantinovSBroadcastMPI : public BaseTask<T> {
 public:
  using InType = std::vector<T>;
  using OutType = InType;
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit KonstantinovSBroadcastMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

template <typename T>
constexpr MPI_Datatype get_mpi_type();

template <>
constexpr MPI_Datatype get_mpi_type<int>() {
  return MPI_INT;
}

template <>
constexpr MPI_Datatype get_mpi_type<float>() {
  return MPI_FLOAT;
}

template <>
constexpr MPI_Datatype get_mpi_type<double>() {
  return MPI_DOUBLE;
}

}  // namespace konstantinov_s_broadcast
