#pragma once

#include "animation.h"

#include <vector>
#include <filesystem>

namespace imgv
{
   class application
   {
      std::vector<std::filesystem::path> m_image_list{};

      SDL_Window* m_window{nullptr};
      SDL_Renderer* m_renderer{nullptr};

      enum class DisplayMode { Fit, Letterbox, Original };
      DisplayMode m_mode{DisplayMode::Fit};

      SDL_Texture* m_current_image{};
      std::vector<animation> m_frames;
      bool m_animate{false};
      int m_current_frame{0};
      Uint64 m_next_frame_tick{0};

      int m_width{};
      int m_height{};

      float m_pan_x{};
      float m_pan_y{};
      bool m_dragging{false};
      float m_last_mouse_x{0.0f};
      float m_last_mouse_y{0.0f};

   public:
      explicit application(auto images) : m_image_list{std::move(images)} {}
      ~application();

      [[nodiscard]] auto init() -> bool;

      void run();

   private:
      static constexpr auto next(DisplayMode current) -> DisplayMode;

      void update();

      void render() const;

      auto load_image(std::filesystem::path const& filename) -> bool;

      void next_image();
      void previous_image();

      auto toggle_fullscreen() -> bool;
   };
}