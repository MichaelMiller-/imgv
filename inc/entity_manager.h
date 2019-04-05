#pragma once

#include <memory>
#include <bitset>
#include <vector>
#include <array>
#include <type_traits>
#include <cassert>
#include <algorithm>

// Entity�component�system
// https://en.wikipedia.org/wiki/Entity%E2%80%93component%E2%80%93system
namespace orpg
{
struct component;

using component_id = std::uint32_t;

namespace detail 
{
   inline component_id unique_component_id() noexcept
   {
      static component_id last_id{0};
      return last_id++;
   }
} // namespace detail

template <typename T> 
inline component_id get_component_typeid() noexcept
{
   static_assert(std::is_base_of<component, T>::value, "T must inherit from component");

   static component_id id{detail::unique_component_id()};
   return id;
}


template <typename T>
concept bool Component = requires(T t) 
{
    typename value_t<T>;       // T must have a value type
    // typename iterator_t<T>;    // T must have an iterator type

    // T must have begin() and end()
    // { begin(t) } -> iterator_t<T>;  
    // { end(t) } -> iterator_t<T>;  

    // requires InputIterator<iterator_t<T>>;
    // requires Same<value_t<T>, value_t<iterator_t<T>>>;
};

template <Component T> 
auto component_typeid() noexcept
{
   static component_id last_id{0};
   return last_id++;
}

struct component_tag {};

//! \brief basically the health of the entity
struct hitpoints_2 : public component_tag
{
   int value{};
}; 


// static constexpr std::size_t max_entities{500};
//! \brief feel free to edit this variable
static constexpr std::size_t max_components{ 32 };

class entity;

//! \brief every component type has an unique component_id number
struct component
{
   entity* owner{nullptr};

   virtual ~component() { }

   virtual void init() { }
   //! \todo unused variable - [[maybe_unused]]
   virtual void update(float frame_time) { }

   virtual void draw() { }
};

//! \brief every entity has a only one instance of a certain component type
class entity
{
	std::vector<std::shared_ptr<component> >  components;
   //std::vector<std::unique_ptr<component> >  components;
   //! \brief to find component per index (component_id) very fast
   std::array<component*, max_components>    component_array;
   //! \brief [ 0, 0, 0, 1, 1, 0, 0, .... N ] -> entity has component 4 and 5
   std::bitset<max_components>               component_bitset;

   bool alive{true};

public:
   entity() = default;

   //entity(entity const&) = delete;
   //entity& operator = (entity const&) = delete;

   //template <typename T>
   //void update(T frame_time) {
   void update(float frame_time) {
      for (auto& c : components) {
         c->update(frame_time);
      }
   }
   
   void draw() { 
      for (auto& c : components) 
         c->draw(); 
   }

   void destroy() noexcept { alive = false; }
   bool is_alive() const noexcept { return alive; }

   template <typename T>
   inline bool has() const noexcept {
      return component_bitset[get_component_typeid<T>()];
   }

   template <typename T, typename... Args>
   auto add(Args &&... args)
   {
      // check if entity already has this component
      assert(!has<T>());

      auto c{ new T{std::forward<Args>(args)...} };
      c->owner = this;

      decltype(components)::value_type ptr{c};
      components.push_back(std::move(ptr));
      // components.push_back(c);

      component_array[get_component_typeid<T>()] = c;
      component_bitset[get_component_typeid<T>()] = true;

      c->init();
   }

   template <typename T>
   T& get() const
   {
      assert(has<T>());

      auto ptr(component_array[get_component_typeid<T>()]);
      return *static_cast<T*>(ptr);
   }
};

class entity_manager final
{
   std::vector<std::unique_ptr<entity>>   entities;

private:
   inline void clear() noexcept {
      //! \todo test 
      entities.clear();
   }

public:
   entity_manager() = default;

   entity_manager(entity_manager const&) = delete;
   entity_manager& operator = (entity_manager const&) = delete;

   ~entity_manager() { clear(); }

   //template <class T>
   void update(float frame_time) 
   {
      entities.erase(
         std::remove_if(std::begin(entities), std::end(entities), [](auto const& e) { return !e->is_alive(); }),
         std::end(entities));


      for (auto& e : entities) {
         // log("entity in update:", *e);
         e->update(frame_time);
      }
   }

   void draw() {
      for (auto& e : entities) 
         e->draw();
   }

#if 0
   //! \todo test
   template <typename Iterator>
   void draw(Iterator begin, Iterator end) {
      for (; begin != end; ++begin) (*begin)->draw();
   }
#endif

   auto create()
   {
      entity* e = new entity;;

      decltype(entities)::value_type ptr{e};
      entities.push_back(std::move(ptr));

      return *e;
   }
};

} // namespace

