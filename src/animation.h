#pragma once

#include <SDL3/SDL_render.h>

namespace imgv
{
   struct animation
   {
      SDL_Texture* frame{nullptr};
      int delay_ms{100};
   };
} // namespace imgv