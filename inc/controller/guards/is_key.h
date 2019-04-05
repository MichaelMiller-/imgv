#pragma once

namespace orpg {

   const auto is_key = [](int keycode) {
#ifdef _MSC_VER
      return [=](key_pressed event) { return event.key == keycode; };
#else
      return [=](auto event) { return event.key == keycode; };
#endif
   };
}
