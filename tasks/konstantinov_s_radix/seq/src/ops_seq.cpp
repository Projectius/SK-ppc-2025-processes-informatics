#include "konstantinov_s_radix/seq/include/ops_seq.hpp"

// #include <iostream>
// #include <numeric>
#include <cstddef>
#include <cstdint>
// #include <vector>
#include <algorithm>
#include <array>

#include "konstantinov_s_radix/common/include/common.hpp"
// #include "util/include/util.hpp"

namespace konstantinov_s_radix {

KonstantinovSRadixSEQ::KonstantinovSRadixSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  // GetOutput() = 0;
}

bool KonstantinovSRadixSEQ::ValidationImpl() {
  // std::cout << "\t\tValidation seq\n";
  return true;  //! GetInput().empty();
}

bool KonstantinovSRadixSEQ::PreProcessingImpl() {
  return true;
}

bool KonstantinovSRadixSEQ::RunImpl() {
  InType arr = GetInput();
  size_t n = arr.size();
  if (n <= 1) {
    GetOutput() = arr;
    return true;
  }

  InType aux(n);

  constexpr int kBytes = 4;
  for (int byte_shift = 0; byte_shift < kBytes * 8; byte_shift += 8) {
    std::array<size_t, 256> bucket{};
    for (size_t i = 0; i < n; ++i) {
      auto u = static_cast<uint32_t>(arr[i]);
      // signed->unsigned для сортировки
      u ^= 0x80000000U;
      auto key = static_cast<uint8_t>((u >> byte_shift) & 0xFFU);
      ++bucket.at(key);
    }
    size_t acc = 0;
    for (auto &elem : bucket) {
      size_t t = elem;
      elem = acc;
      acc += t;
    }

    for (size_t i = 0; i < n; ++i) {
      auto u = static_cast<uint32_t>(arr[i]);
      u ^= 0x80000000U;
      auto key = static_cast<uint8_t>((u >> byte_shift) & 0xFFU);
      aux.at(bucket.at(key)++) = arr.at(i);
    }

    arr.swap(aux);
  }

  GetOutput() = std::move(arr);
  return true;
}

bool KonstantinovSRadixSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace konstantinov_s_radix
