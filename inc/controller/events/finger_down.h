#pragma once

namespace orpg {

   struct finger_down {
      static constexpr auto id = SDL_FINGERDOWN;

      const int x{ 0 };
      const int y{ 0 };

      explicit finger_down(SDL_Event const& event)
         : x{ static_cast<decltype(x)>(event.tfinger.x) }
         , y{ static_cast<decltype(x)>(event.tfinger.y) } { }
   };
}
