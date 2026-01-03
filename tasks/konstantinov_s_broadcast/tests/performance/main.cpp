#include <gtest/gtest.h>

#include <algorithm>

#include "konstantinov_s_broadcast/common/include/common.hpp"
#include "konstantinov_s_broadcast/mpi/include/ops_mpi.hpp"
#include "konstantinov_s_broadcast/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace konstantinov_s_broadcast {

class KonstantinovSBroadcastTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 150000000;
  InType input_data_;
  // OutType result_right_{};

  void SetUp() override {
    input_data_.resize(kCount_, 1);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return 0 == memcmp(input_data_.data(), output_data.data(), input_data_.size() * sizeof(EType));
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(KonstantinovSBroadcastTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, KonstantinovSBroadcastMPI<EType>, KonstantinovSBroadcastSEQ<EType>>(
        PPC_SETTINGS_konstantinov_s_broadcast);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = KonstantinovSBroadcastTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, KonstantinovSBroadcastTests, kGtestValues, kPerfTestName);

}  // namespace konstantinov_s_broadcast
