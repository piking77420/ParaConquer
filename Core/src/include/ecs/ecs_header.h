#pragma once
#include "core_header.hpp"
#include "reflection/reflector.hpp"

#include <cassert>

BEGIN_PCCORE

using EntityId = uint32_t;
constexpr EntityId INVALID_ENTITY_ID = -1;
constexpr EntityId RESERVE_ENTITIES = 2000;

constexpr EntityId MAX_ENTITY = RESERVE_ENTITIES * RESERVE_ENTITIES;

END_PCCORE