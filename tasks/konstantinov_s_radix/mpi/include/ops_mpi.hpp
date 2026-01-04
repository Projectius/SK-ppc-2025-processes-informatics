#pragma once

#include "konstantinov_s_radix/common/include/common.hpp"
#include "task/include/task.hpp"

namespace konstantinov_s_radix {

class KonstantinovSRadixMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit KonstantinovSRadixMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  static void LocalRadixPass(InType &block);
  static void PairwiseMergeExchange(InType &local_block, int prank, int comm_sz);
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace konstantinov_s_radix
