#pragma once
#include "core_header.hpp"
#include "reflection/reflector.hpp"

#include <cassert>
#include <bitset>

BEGIN_PCCORE

using EntityId = uint32_t;

constexpr EntityId INVALID_ENTITY_ID = -1;
constexpr EntityId MAX_ENTITIES = 20000;

constexpr uint32_t MAX_COMPONENT = 1000;
using Signature = std::bitset<MAX_COMPONENT>;
constexpr size_t s = sizeof(Signature);

END_PCCORE