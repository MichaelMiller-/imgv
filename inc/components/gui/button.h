#pragma once

#include "entity_manager.h"

namespace orpg {
namespace gui 
{
   struct button : public ::orpg::component
   {
      ::clickable<::orpg::rect>  bounds;

      button(decltype(bounds)& b) : bounds{ b } {

      }
   };
} 
}
