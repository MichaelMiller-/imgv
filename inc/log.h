#pragma once

#include <iostream>

namespace orpg {

   template <typename... Ts>
   inline void log(Ts ... ts)
   {
      using swallow = int[];
      (void)swallow {
         (std::cout << ts << " ", 0)...
      };
      std::cout << std::endl;
   }

}
