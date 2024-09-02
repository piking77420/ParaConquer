#pragma once
#include "core_header.hpp"
#include <cassert>

using EntityId = uint32_t;
constexpr EntityId INVALID_ENTITY_ID = -1;
constexpr EntityId MAX_ENTITIES = 100;

#define IsValidEntityId(entityId) \
    assert((entityId) != INVALID_ENTITY_ID && (entityId) < MAX_ENTITIES);\

