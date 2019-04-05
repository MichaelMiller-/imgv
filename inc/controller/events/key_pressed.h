#pragma once

namespace orpg {

   struct key_pressed {
      static constexpr auto id = SDL_KEYDOWN;

      const int key{ 0 };

      explicit key_pressed(SDL_Event const& event) : key(event.key.keysym.sym) { }
   };
}
