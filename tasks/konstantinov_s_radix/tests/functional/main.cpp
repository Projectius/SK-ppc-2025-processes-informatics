#include <gtest/gtest.h>
#include <stb/stb_image.h>

// #include <algorithm>
// #include <array>
// #include <cstddef>
// #include <cstdint>
// #include <numeric>
// #include <stdexcept>
#include <fstream>
#include <string>
#include <tuple>
// #include <utility>
// #include <vector>

#include "konstantinov_s_radix/common/include/common.hpp"
#include "konstantinov_s_radix/mpi/include/ops_mpi.hpp"
#include "konstantinov_s_radix/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/perf_test_util.hpp"
#include "util/include/util.hpp"

namespace konstantinov_s_radix {

class KonstantinovSRadixTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return test_param;
  }

 protected:
  void SetUp() override {
    // std::cout<<"SETUP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n";

    // if (ppc::util::GetMPIRank() != 0) {
    //   return;
    // }

    TestType fileparam =
        std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam()) + ".txt";
    std::ifstream file(ppc::util::GetAbsoluteTaskPath(PPC_ID_konstantinov_s_radix, fileparam));

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
    // std::cout<<output_data[i]<<"\t"<<result_right_[i]<<"\n";
    return (output_data == result_right_);
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_{};
  OutType result_right_{};
};

namespace {

TEST_P(KonstantinovSRadixTests, Radix) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 17> kTestParam = {"fixed_0",    "fixed_1",    "fixed_2",   "fixed_3",    "fixed_4",
                                             "fixed_5",    "fixed_6",    "rand_0_n0", "rand_1_n1",  "rand_2_n2",
                                             "rand_3_n3",  "rand_4_n4",  "rand_5_n5", "rand_6_n10", "rand_7_n20",
                                             "big_0_n100", "big_1_n1000"};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<KonstantinovSRadixMPI, InType>(kTestParam, PPC_SETTINGS_konstantinov_s_radix),
    ppc::util::AddFuncTask<KonstantinovSRadixSEQ, InType>(kTestParam, PPC_SETTINGS_konstantinov_s_radix));
const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = KonstantinovSRadixTests::PrintFuncTestName<KonstantinovSRadixTests>;

INSTANTIATE_TEST_SUITE_P(RadixTests, KonstantinovSRadixTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace konstantinov_s_radix
