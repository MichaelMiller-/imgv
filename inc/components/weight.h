#pragma once

#include "entity_manager.h"

namespace orpg 
{
   //! \brief weight from items, ...
   struct weight : public component 
   {
      int value;

      constexpr weight(int value) : value{value} {}
   };
}
