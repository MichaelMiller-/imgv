#pragma once

namespace orpg {

   struct mouse_motion {
      static constexpr auto id = SDL_MOUSEMOTION;

      const int x{ 0 };
      const int y{ 0 };

      explicit mouse_motion(SDL_Event const& event)
         : x{ static_cast<decltype(x)>(event.motion.x) }
         , y{ static_cast<decltype(x)>(event.motion.y) } { }
   };

}
