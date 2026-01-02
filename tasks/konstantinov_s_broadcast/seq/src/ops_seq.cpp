#include "konstantinov_s_broadcast/seq/include/ops_seq.hpp"

// #include <iostream>
// #include <numeric>
#include <cstddef>
#include <vector>

#include "konstantinov_s_broadcast/common/include/common.hpp"
// #include "util/include/util.hpp"

namespace konstantinov_s_broadcast {

KonstantinovSBroadcastSEQ::KonstantinovSBroadcastSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KonstantinovSBroadcastSEQ::ValidationImpl() {
  // std::cout << "\t\tValidation seq\n";
  return !GetInput().empty();
}

bool KonstantinovSBroadcastSEQ::PreProcessingImpl() {
  return true;
}

bool KonstantinovSBroadcastSEQ::RunImpl() {
  const auto invec = GetInput();
  int res = 0;
  size_t iterations = invec.size() - 1;
  const EType *v = invec.data();
  for (size_t i = 0; i < iterations; i++) {
    res += static_cast<int>((v[i] > 0) != (v[i + 1] > 0));  // + 1 если занки разные
  }
  GetOutput() = res;
  return true;
}

bool KonstantinovSBroadcastSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace konstantinov_s_broadcast
