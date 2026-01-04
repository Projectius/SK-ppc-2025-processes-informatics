#pragma once

#include <string>
// #include <tuple>
#include <cstdint>
#include <vector>

#include "task/include/task.hpp"

namespace konstantinov_s_radix {

using InType = std::vector<int32_t>;
using OutType = std::vector<int32_t>;
using TestType = std::string;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace konstantinov_s_radix
