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
  //GetOutput() = 0;
}

bool KonstantinovSRadixSEQ::ValidationImpl() {
  // std::cout << "\t\tValidation seq\n";
  return !GetInput().empty();
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

  // количество байтов = sizeof(int32_t) == 4 -> shifts 0,8,16,24
  constexpr int BYTES = 4;
  for (int byte_shift = 0; byte_shift < BYTES * 8; byte_shift += 8) {
    // counts
    std::array<size_t, 256> bucket{};
    // подсчёт
    for (size_t i = 0; i < n; ++i) {
      uint32_t u = static_cast<uint32_t>(arr[i]);
      // трансформируем signed->lexicographically sortable unsigned
      u ^= 0x80000000u;
      uint8_t key = static_cast<uint8_t>((u >> byte_shift) & 0xFFu);
      ++bucket[key];
    }
    // prefix-sum -> позиции
    size_t acc = 0;
    for (size_t k = 0; k < bucket.size(); ++k) {
      size_t t = bucket[k];
      bucket[k] = acc;
      acc += t;
    }
    // стабильная перестановка в aux
    for (size_t i = 0; i < n; ++i) {
      uint32_t u = static_cast<uint32_t>(arr[i]);
      u ^= 0x80000000u;
      uint8_t key = static_cast<uint8_t>((u >> byte_shift) & 0xFFu);
      aux[bucket[key]++] = arr[i];
    }
    // swap buffers (изменим ссылку)
    arr.swap(aux);
  }

  GetOutput() = std::move(arr);
  return true;
}

bool KonstantinovSRadixSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace konstantinov_s_radix
