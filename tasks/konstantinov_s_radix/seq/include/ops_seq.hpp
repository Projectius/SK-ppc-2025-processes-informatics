#pragma once

#include "konstantinov_s_radix/common/include/common.hpp"
#include "task/include/task.hpp"

namespace konstantinov_s_radix {

class KonstantinovSRadixSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit KonstantinovSRadixSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace konstantinov_s_radix
