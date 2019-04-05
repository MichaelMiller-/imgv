#pragma once

namespace orpg {

   struct mouse_button_down {
      static constexpr auto id = SDL_MOUSEBUTTONDOWN;

      const int x{ 0 };
      const int y{ 0 };

      explicit mouse_button_down(SDL_Event const& event)
         : x{ static_cast<decltype(x)>(event.button.x) }
         , y{ static_cast<decltype(x)>(event.button.y) } { }
   };
}
