#pragma once

namespace orpg 
{
   template <typename Canvas>
   class view
   {
      Canvas& canvas;

   public:
      explicit view(Canvas& c) : canvas{ c } {}

      void clear() const noexcept {
         canvas.clear();
      }

      void update() const noexcept {
         canvas.draw();
      }
   };
}
