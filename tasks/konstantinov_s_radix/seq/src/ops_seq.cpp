#include "konstantinov_s_radix/seq/include/ops_seq.hpp"

// #include <iostream>
// #include <numeric>
#include <cstddef>
#include <vector>

#include "konstantinov_s_radix/common/include/common.hpp"
// #include "util/include/util.hpp"

namespace konstantinov_s_radix {

KonstantinovSRadixSEQ::KonstantinovSRadixSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KonstantinovSRadixSEQ::ValidationImpl() {
  // std::cout << "\t\tValidation seq\n";
  return !GetInput().empty();
}

bool KonstantinovSRadixSEQ::PreProcessingImpl() {
  return true;
}

bool KonstantinovSRadixSEQ::RunImpl() {
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

bool KonstantinovSRadixSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace konstantinov_s_radix
