#pragma once

#include "konstantinov_s_broadcast/common/include/common.hpp"
#include "task/include/task.hpp"

namespace konstantinov_s_broadcast {

class KonstantinovSBroadcastMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit KonstantinovSBroadcastMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  static void CountSignChange(int &res, const EType *data, int start, int iterations);
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace konstantinov_s_broadcast
