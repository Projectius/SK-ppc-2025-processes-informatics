#include "konstantinov_s_broadcast/seq/include/ops_seq.hpp"

// #include <iostream>
// #include <numeric>
#include <cstring>  // memcpy

#include "konstantinov_s_broadcast/common/include/common.hpp"
// #include "util/include/util.hpp"

namespace konstantinov_s_broadcast {
template <typename T>
KonstantinovSBroadcastSEQ<T>::KonstantinovSBroadcastSEQ(const InType &in) {
  this->SetTypeOfTask(GetStaticTypeOfTask());
  this->GetInput() = in;
  this->GetOutput().resize(this->GetInput().size());
}
template <typename T>
bool KonstantinovSBroadcastSEQ<T>::ValidationImpl() {
  // std::cout << "\t\tValidation seq\n";
  return !this->GetInput().empty();
}
template <typename T>
bool KonstantinovSBroadcastSEQ<T>::PreProcessingImpl() {
  return true;
}
template <typename T>
bool KonstantinovSBroadcastSEQ<T>::RunImpl() {
  memcpy(this->GetOutput().data(), this->GetInput().data(), this->GetInput().size() * sizeof(T));
  return true;
}
template <typename T>
bool KonstantinovSBroadcastSEQ<T>::PostProcessingImpl() {
  return true;
}

template class KonstantinovSBroadcastSEQ<ETypeInt>;
template class KonstantinovSBroadcastSEQ<ETypeFloat>;
template class KonstantinovSBroadcastSEQ<ETypeDouble>;

}  // namespace konstantinov_s_broadcast
