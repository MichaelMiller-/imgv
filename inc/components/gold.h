#pragma once

#include "entity_manager.h"

namespace orpg {
//! \brief can use in more ways: price of a item, amount in purse, loot from a chest
struct gold : public component {
  int value;

  constexpr gold(int value) : value{value} {}
};
}
