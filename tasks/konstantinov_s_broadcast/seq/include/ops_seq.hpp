#pragma once
#include <vector>

#include "konstantinov_s_broadcast/common/include/common.hpp"
#include "task/include/task.hpp"

namespace konstantinov_s_broadcast {
template <typename T>
class KonstantinovSBroadcastSEQ : public BaseTask<T> {
 public:
  using InType = std::vector<T>;
  using OutType = InType;
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit KonstantinovSBroadcastSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace konstantinov_s_broadcast
