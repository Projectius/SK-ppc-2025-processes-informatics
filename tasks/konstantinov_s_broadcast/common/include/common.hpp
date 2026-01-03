#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include "task/include/task.hpp"

namespace konstantinov_s_broadcast {

using TestType = std::tuple<int, std::string>;

using ETypeInt = int;
using ETypeFloat = float;
using ETypeDouble = double;

using EType = ETypeInt;
using InType = std::vector<EType>;
using OutType = InType;

template <typename T>
using BaseTask = ppc::task::Task<std::vector<T>, std::vector<T>>;

}  // namespace konstantinov_s_broadcast
