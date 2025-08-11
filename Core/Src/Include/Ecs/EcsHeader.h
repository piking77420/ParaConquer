#pragma once
#include "CoreHeader.hpp"
#include "Reflection/Reflector.hpp"

#include <Cassert>
#include <Bitset>

BEGIN_PCCORE

using EntityId = uint32_t;

constexpr EntityId INVALID_ENTITY_ID = -1;
constexpr EntityId MAX_ENTITIES = 1048576;
constexpr uint8_t MAX_ENTITY_NAME_LENGHT = std::numeric_limits<uint8_t>::max();

constexpr uint32_t MAX_COMPONENT = 1024;
using Signature = std::bitset<MAX_COMPONENT>;
REFLECT(Signature)

static_assert(MAX_ENTITIES % 2 == 0, "MAX_ENTITIES should be a multiple of 2");
static_assert(MAX_COMPONENT % 2 == 0, "MAX_COMPONENT should be a multiple of 2");

END_PCCORE