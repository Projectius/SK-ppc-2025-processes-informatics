#include <gtest/gtest.h>

// #include <array>

#include "konstantinov_s_radix/common/include/common.hpp"
#include "konstantinov_s_radix/mpi/include/ops_mpi.hpp"
#include "konstantinov_s_radix/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace konstantinov_s_radix {

class KonstantinovSRadixTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  InType input_data_{};
  OutType result_right_{};

  void SetUp() override {
    // if (ppc::util::GetMPIRank() != 0) {
    //   return;
    // }

    std::ifstream file(ppc::util::GetAbsoluteTaskPath(PPC_ID_konstantinov_s_radix, "big_2_n800000.txt"));

    if (file.is_open()) {
      // std::cout<<"OPENED "<<fileparam<<"\n";
      int size = 0;
      file >> size;

      InType input_data(size);
      for (int i = 0; i < size; i++) {
        file >> input_data[i];
      }

      OutType right_data(size);
      for (int i = 0; i < size; i++) {
        file >> right_data[i];
        // std::cout<<input_data[i]<<"\t"<<right_data[i]<<"\n";
      }
      input_data_ = input_data;
      result_right_ = right_data;
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if (ppc::util::IsUnderMpirun() && ppc::util::GetMPIRank() != 0) {
      return true;
    }
    // std::cout<<"CHECK: \n";
    // for(int i=0;i<output_data.size();i++)
    return (output_data == result_right_);
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(KonstantinovSRadixTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, KonstantinovSRadixMPI, KonstantinovSRadixSEQ>(
    PPC_SETTINGS_konstantinov_s_radix);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = KonstantinovSRadixTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, KonstantinovSRadixTests, kGtestValues, kPerfTestName);

}  // namespace konstantinov_s_radix
