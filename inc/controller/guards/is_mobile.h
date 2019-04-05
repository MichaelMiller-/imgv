#pragma once

#if defined(EMSCRIPTEN)
   #include <emscripten.h>
   #define EM(...) __VA_ARGS__ EM_ELSE
   #define EM_ELSE(...)
#else
   #define EM(...) EM_ELSE
   #define EM_ELSE(...) __VA_ARGS__
   #define EM_ASM_INT_V(...) 0
#endif

namespace orpg {

   const auto is_mobile = [] {
      return bool(EM_ASM_INT_V({ return / iPhone | iPad | iPod | Android / i.test(navigator.userAgent); }));
   };
}
