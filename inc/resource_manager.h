#pragma once

#include <iostream>

using std::cout;
using std::endl;

namespace orpg {

   namespace legacy {

      using GLsizei = std::size_t;
      using GLuint = int;
   }

   namespace behavior 
   {
      template <typename T>
      struct free_store
      {
         using hande_t = T*;

         constexpr const auto null_handle() const noexcept { return nullptr; }

         hande_t init(T* ptr) const {
//            return legacy::free_store_new<T>(ptr);
            return handle_t;
         }

         void release(hande_t const& handle) {
//            legacy::free_store_delete(handle);
         }
      };


   }

   namespace resource
   {
      template <typename TBehavior>
      class unique : TBehavior
      {
      public:
         using behavior_type = TBehavior;
         using handle_type = typename behavior_type::handle_t;

      private:
         handle_type handle;

         auto& as_behavior() noexcept {
            return static_cast<behavior_type&>(*this);
         }

         const auto& as_behavior() const noexcept {
            return static_cast<behavior_type const&>(*this);
         }

      public:
         unique() noexcept : handle{ as_behavior().null_type() } { }
         ~unique() noexcept { reset(); }

         explicit unique(handle_type const& handle) noexcept;

         unique(unique const&) = delete;
         unique& operator = (unique const&) = delete;

         unique(unique &&) noexcept : handle{ rhs.handle } {
            rhs.release();
         }
         auto& operator = (unique &&) noexcept;

         auto release() noexcept;

         void reset() noexcept;
         void reset(handle_type const& handle) noexcept;

         void swap(unique& rhs) noexcept;

         auto get() const noexcept;

         explicit operator bool() const noexcept;

         friend bool operator == (unique const& lhs, unique const& rhs) noexcept;
         friend bool operator == (unique const& lhs, unique const& rhs) noexcept;

         friend void swap(unique& lhs, unique& rhs) noexcept;
      };
   };

   struct resource_manager
   {

   };
}