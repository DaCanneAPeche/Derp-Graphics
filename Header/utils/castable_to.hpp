#pragma once

#include <type_traits>
#include <concepts>
#include <cstdint>

namespace dg
{

  template <typename FinalType, typename T>
    concept CastableTo = requires(T x) {
      { static_cast<FinalType>(x) } -> std::same_as<FinalType>;
    };

}
