#pragma once

#include "entity_manager.h"

#include <boost/qvm/vec.hpp>

namespace orpg 
{
   using vec2f_t = boost::qvm::vec<float, 2>;

   struct position : public component 
   {
      vec2f_t value{0, 0};

      constexpr position(vec2f_t const &value) : value{value} {}

      explicit operator vec2f_t() const { return value; }
   };
}
