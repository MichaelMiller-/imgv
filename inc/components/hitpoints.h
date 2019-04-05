#pragma once

#include "entity_manager.h"

namespace orpg 
{
   //! \brief basically the health of the entity
   struct hitpoints : public component 
   {
     int value;

     constexpr hitpoints(int value) : value{value} {}

     void update(float frame_time) override {
       if (value <= 0)
         owner->destroy();
     }
   };
}
