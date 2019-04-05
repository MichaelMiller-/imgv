#pragma once

#include <exception>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>

//#include <SDL2/SDL_opengles2_gl2.h>
//#include <SDL2/SDL_opengles2_gl2ext.h>

namespace orpg 
{
class sdl_canvas
{
   std::shared_ptr<SDL_Window> window_;
   std::shared_ptr<SDL_Renderer> renderer;

   std::shared_ptr<void> ctx;
	// std::vector<std::pair<std::shared_ptr<void>, SDL_Rect>> elements_;

public:
   explicit sdl_canvas(config c) 
   {
      if (SDL_Init(SDL_INIT_VIDEO) != 0)
         throw std::runtime_error(std::string{"Unable to initialize SDL: "} + SDL_GetError());

      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
      SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
      SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
      // use OpenGL 2.1
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
      SDL_DisplayMode current;
      SDL_GetCurrentDisplayMode(0, &current);

      window_ = std::shared_ptr<SDL_Window>(
         SDL_CreateWindow(
            c.window_title.c_str(), 
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, 
            c.window_width, 
            c.window_height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE),
         SDL_DestroyWindow);
      
      ctx = std::shared_ptr<void>(SDL_GL_CreateContext(window_.get()), SDL_GL_DeleteContext);

      glewInit();
   }

   ~sdl_canvas() noexcept 
   {
      SDL_Quit();
   }

   void clear() const noexcept
   {
      //SDL_GL_SwapWindow(window_.get());
   }

   void draw() const noexcept
   {

      SDL_GL_SwapWindow(window_.get());
   }
};

}
