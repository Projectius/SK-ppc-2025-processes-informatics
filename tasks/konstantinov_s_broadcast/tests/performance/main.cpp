#include <gtest/gtest.h>

#include <array>

#include "konstantinov_s_broadcast/common/include/common.hpp"
#include "konstantinov_s_broadcast/mpi/include/ops_mpi.hpp"
#include "konstantinov_s_broadcast/seq/include/ops_seq.hpp"
#include "konstantinov_s_broadcast/tests/testgen.h"
#include "util/include/perf_test_util.hpp"

namespace konstantinov_s_broadcast {

class KonstantinovSBroadcastTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 150000000;
  InType input_data_;
  OutType result_right_{};

  void SetUp() override {
    input_data_.resize(kCount_);

    const std::array<EType, 15> arr = {1, -1, 23838, -121, -1223, -567, 12334, 42, -12, 2, -43, 33, 44, -7, 1};
    const int arrsz = 15;
    const int chngcnt = 10;

    result_right_ = GenerateTestData(arr.data(), arrsz, chngcnt, input_data_);
    // std::cout<<input_data_.capacity()<<" "<<kCount_<<"\n";
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return result_right_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(KonstantinovSBroadcastTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, KonstantinovSBroadcastMPI, KonstantinovSBroadcastSEQ>(
        PPC_SETTINGS_konstantinov_s_broadcast);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = KonstantinovSBroadcastTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, KonstantinovSBroadcastTests, kGtestValues, kPerfTestName);

}  // namespace konstantinov_s_broadcast
