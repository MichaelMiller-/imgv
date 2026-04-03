#include "application.h"
#include "config.h"
#include "settings.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include <algorithm>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include <utility>

namespace imgv
{
   auto compute_letterbox_rect(int window_w, int window_h, auto image_w, auto image_h) -> SDL_FRect
   {
      const float image_aspect = image_w / image_h;
      const float window_aspect = static_cast<float>(window_w) / static_cast<float>(window_h);

      SDL_FRect dst{};

      if (window_aspect > image_aspect) {
         // Window is wider than image aspect: fit to height
         dst.h = static_cast<float>(window_h);
         dst.w = dst.h * image_aspect;
         dst.x = (window_w - dst.w) * 0.5f;
         dst.y = 0.0f;
      } else {
         // Window is taller/narrower than image aspect: fit to width
         dst.w = static_cast<float>(window_w);
         dst.h = dst.w / image_aspect;
         dst.x = 0.0f;
         dst.y = (window_h - dst.h) * 0.5f;
      }

      return dst;
   }

   application::~application()
   {
      if (m_renderer) {
         SDL_DestroyRenderer(m_renderer);
      }
      if (m_window) {
         SDL_DestroyWindow(m_window);
      }
      SDL_Quit();
   }

   [[nodiscard]] auto application::init() -> bool
   {
      if (not SDL_Init(SDL_INIT_VIDEO)) {
         std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
         return false;
      }

      auto config = settings{std::filesystem::path{IMGV_BINARY_DIRECTORY} / "settings.conf"};
      auto width = config["WindowWidth"].to_int();
      auto height = config["WindowHeight"].to_int();

      m_window = SDL_CreateWindow(IMGV_BINARY_NAME, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
      if (not m_window) {
         std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
         return false;
      }

      m_renderer = SDL_CreateRenderer(m_window, nullptr);
      if (not m_renderer) {
         std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
         return false;
      }

      return load_image(*begin(m_image_list));
   }

   void application::run()
   {
      bool running = true;
      while (running) {
         SDL_Event event;
         while (SDL_PollEvent(std::addressof(event))) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
               running = false;
               break;
            case SDL_EVENT_KEY_DOWN:
               if (event.key.key == SDLK_ESCAPE) {
                  running = false;
               }
               if (event.key.key == SDLK_M) {
                  m_mode = next(m_mode);
               }
               if (event.key.key == SDLK_LEFT) {
                  previous_image();
               }
               if (event.key.key == SDLK_RIGHT) {
                  next_image();
               }
               break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
               if (event.button.button == SDL_BUTTON_LEFT) {
                  next_image();
               }
               if (event.button.button == SDL_BUTTON_RIGHT) {
                  previous_image();
               }
               if (m_mode == DisplayMode::Original and event.button.button == SDL_BUTTON_MIDDLE) {
                  m_dragging = true;
                  m_last_mouse_x = event.button.x;
                  m_last_mouse_y = event.button.y;
               }
               break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
               if (event.button.button == SDL_BUTTON_MIDDLE) {
                  m_dragging = false;
               }
               break;
            case SDL_EVENT_MOUSE_MOTION:
               if (m_mode == DisplayMode::Original && m_dragging) {
                  auto dx = event.motion.x - m_last_mouse_x;
                  auto dy = event.motion.y - m_last_mouse_y;
                  m_pan_x += dx;
                  m_pan_y += dy;
                  m_last_mouse_x = event.motion.x;
                  m_last_mouse_y = event.motion.y;
               }
               break;
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
               if (not SDL_GetCurrentRenderOutputSize(m_renderer, std::addressof(m_width), std::addressof(m_height))) {
                  std::cerr << "SDL_GetCurrentRenderOutputSize failed: " << SDL_GetError() << '\n';
                  return;
               }
               break;
            }
         }
         update();
         render();
         SDL_Delay(10);
      }
   }

   void application::update()
   {
      if (not m_animate or m_frames.empty()) {
         return;
      }

      auto const now = SDL_GetTicks();
      if (now < m_next_frame_tick) {
         return;
      }

      m_current_frame = (m_current_frame + 1) % static_cast<int>(m_frames.size());
      m_next_frame_tick = now + m_frames[m_current_frame].delay_ms;
   }
#if 0
   void clampPan(int viewport_w, int viewport_h, int image_w, int image_h)
   {
      // If image is smaller than viewport, keep it inside the viewport bounds.
      if (image_w <= viewport_w) {
         if (m_pan_x < 0.0f) m_pan_x = 0.0f;
         if (m_pan_x > viewport_w - image_w) m_pan_x = static_cast<float>(viewport_w - image_w);
      } else {
         // If image is larger, allow scrolling but don't lose it entirely.
         if (m_pan_x > 0.0f) m_pan_x = 0.0f;
         if (m_pan_x < viewport_w - image_w) m_pan_x = static_cast<float>(viewport_w - image_w);
      }

      if (image_h <= viewport_h) {
         if (m_pan_y < 0.0f) m_pan_y = 0.0f;
         if (m_pan_y > viewport_h - image_h) m_pan_y = static_cast<float>(viewport_h - image_h);
      } else {
         if (m_pan_y > 0.0f) m_pan_y = 0.0f;
         if (m_pan_y < viewport_h - image_h) m_pan_y = static_cast<float>(viewport_h - image_h);
      }
   }
#endif

   //! \todo this is not the best implementation
   constexpr auto application::next(DisplayMode current) -> DisplayMode
   {
      if (current == DisplayMode::Fit) {
         return DisplayMode::Letterbox;
      }
      if (current == DisplayMode::Letterbox) {
         return DisplayMode::Original;
      }
      if (current == DisplayMode::Original) {
         return DisplayMode::Fit;
      }
      // fallback
      return DisplayMode::Fit;
   }

   void application::render() const
   {
      SDL_SetRenderDrawColor(m_renderer, 20, 20, 20, 255);
      SDL_RenderClear(m_renderer);

      static SDL_Texture* ptr{nullptr};

      if (m_animate and not m_frames.empty()) {
         decltype(auto) obj = m_frames[m_current_frame];
         ptr = obj.frame;
      } else {
         ptr = m_current_image;
      }

      if (m_mode == DisplayMode::Fit) {
         SDL_RenderTexture(m_renderer, ptr, nullptr, nullptr);
      }
      if (m_mode == DisplayMode::Letterbox) {
         auto dst = compute_letterbox_rect(m_width, m_height, ptr->w, ptr->h);
         SDL_RenderTexture(m_renderer, ptr, nullptr, std::addressof(dst));
      }
      if (m_mode == DisplayMode::Original) {
         auto dst = SDL_FRect{m_pan_x, m_pan_y, static_cast<float>(ptr->w), static_cast<float>(ptr->h)};
         SDL_RenderTexture(m_renderer, ptr, nullptr, std::addressof(dst));
      }

      SDL_RenderPresent(m_renderer);
   }

   //! \todo simplify and split function
   auto application::load_image(std::filesystem::path const& filename) -> bool
   {
      if (filename.extension() == ".gif") {
         auto anim = IMG_LoadAnimation(filename.string().c_str());
         if (not anim) {
            std::cerr << "IMG_LoadAnimation failed: " << SDL_GetError() << '\n';
            return false;
         }

         m_frames.reserve(anim->count);
         for (decltype(anim->count) i = 0; i < anim->count; ++i) {
            auto surface = anim->frames[i];
            if (not surface) {
               continue;
            }

            auto tex = SDL_CreateTextureFromSurface(m_renderer, surface);
            if (not tex) {
               std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << '\n';
               IMG_FreeAnimation(anim);
               return false;
            }

            animation step{
               .frame = tex,
               .delay_ms = std::max(anim->delays[i], 10), // avoid zero / too-fast frames
            };
            m_frames.push_back(step);
         }

         IMG_FreeAnimation(anim);

         if (m_frames.empty()) {
            std::cerr << "No GIF frames were loaded.\n";
            return false;
         }

         m_animate = true;
         m_current_frame = 0;
         m_next_frame_tick = SDL_GetTicks() + m_frames[0].delay_ms;
         return true;
      } else {
         auto next_image = IMG_LoadTexture(m_renderer, filename.string().c_str());
         if (not next_image) {
            std::cerr << "IMG_LoadTexture failed for '" << filename << "': " << SDL_GetError() << '\n';
            return false;
         }

         m_animate = false;
         // unload and replace old texture
         SDL_DestroyTexture(m_current_image);
         m_current_image = next_image;
      }

      SDL_SetWindowTitle(m_window, std::format("{} - {}", IMGV_BINARY_NAME, filename.filename().string()).c_str());
      return true;
   }

   void application::next_image()
   {
      std::rotate(std::rbegin(m_image_list), std::rbegin(m_image_list) + 1, std::rend(m_image_list));
      load_image(*std::begin(m_image_list));
   }

   void application::previous_image()
   {
      std::rotate(std::begin(m_image_list), std::begin(m_image_list) + 1, std::end(m_image_list));
      load_image(*std::begin(m_image_list));
   }
} // namespace imgv