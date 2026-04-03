#pragma once

#include <tuple>

namespace imgv
{
   template <typename... Fs>
   class any_of
   {
      std::tuple<Fs...> fs;

      template <std::size_t... Is, typename... Args>
      constexpr bool invoke(std::index_sequence<Is...>, Args&&... args) const
      {
         return (... || std::get<Is>(fs)(args...));
      }

   public:
      constexpr explicit any_of(Fs... fs) : fs{fs...} {};

      template <typename... Args>
      constexpr bool operator()(Args&&... args) const
      {
         return invoke(std::index_sequence_for<Fs...>(), std::forward<Args>(args)...);
      }
   };
}