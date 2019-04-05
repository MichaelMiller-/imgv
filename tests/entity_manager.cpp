#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <iostream>

#include "inc/entity_manager.h"

TEST_CASE("test get_component_typeid", "[entity_manager][component]")
{
   using namespace orpg;

   struct A : component {};
   struct B : component {};
   struct C : component {};
   struct X : component {};

   REQUIRE(get_component_typeid<A>() == 0);
   REQUIRE(get_component_typeid<B>() == 1);
   REQUIRE(get_component_typeid<C>() == 2);
   REQUIRE(get_component_typeid<X>() == 3);

   REQUIRE(get_component_typeid<A>() == 0);
   REQUIRE(get_component_typeid<B>() == 1);
   REQUIRE(get_component_typeid<C>() == 2);
   REQUIRE(get_component_typeid<X>() == 3);
}

