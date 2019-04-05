#pragma once

#include "entity_manager.h"

namespace orpg 
{
   template <typename Shape>
   struct clickable : public component
   {
      Shape shape{};
   };
}
