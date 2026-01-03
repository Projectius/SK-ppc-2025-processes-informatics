#include <gtest/gtest.h>
#include <stb/stb_image.h>

// #include <algorithm>
#include <array>
#include <cstddef>
// #include <cstdint>
// #include <numeric>
// #include <stdexcept>
#include <string>
#include <tuple>
#include<cstring>
// #include <utility>
// #include <vector>

#include "konstantinov_s_broadcast/common/include/common.hpp"
#include "konstantinov_s_broadcast/mpi/include/ops_mpi.hpp"
#include "konstantinov_s_broadcast/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace konstantinov_s_broadcast {

class KonstantinovSBroadcastTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::get<1>(test_param) + "_" + std::to_string(std::get<0>(test_param));
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    const int insz = std::get<0>(params);
    input_data_.resize(insz);
    for (int i = 0; i < insz; ++i) {
      input_data_[i] = i;
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    // for(int i=0; i<input_data_.size();i++)
    //   std::cout<<input_data_[i]<<" "<<output_data[i]<<"\n";
    return 0 == memcmp(input_data_.data(), output_data.data(), input_data_.size() * sizeof(EType));
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
};

namespace {

TEST_P(KonstantinovSBroadcastTests, CustomBroadcast) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 3> kTestParam = {
    std::make_tuple(10, "10"), std::make_tuple(20, "20"),
    std::make_tuple(100, "100")};  // тесты с разным корнем дерева не получится создать (неизвестно колво процессов), но
                                   // корнем выбирается последний процесс в самой реализации для разнообразия

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<KonstantinovSBroadcastMPI<EType>, InType>(kTestParam, PPC_SETTINGS_konstantinov_s_broadcast),
    ppc::util::AddFuncTask<KonstantinovSBroadcastSEQ<EType>, InType>(kTestParam,
                                                                     PPC_SETTINGS_konstantinov_s_broadcast));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = KonstantinovSBroadcastTests::PrintFuncTestName<KonstantinovSBroadcastTests>;

INSTANTIATE_TEST_SUITE_P(CustomBroadcastTests, KonstantinovSBroadcastTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace konstantinov_s_broadcast
