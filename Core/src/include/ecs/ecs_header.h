#pragma once
#include "core_header.hpp"
#include "reflection/reflector.hpp"

#include <cassert>
#include <bitset>

BEGIN_PCCORE

using EntityId = uint32_t;

constexpr EntityId INVALID_ENTITY_ID = -1;
constexpr EntityId MAX_ENTITIES = 1048576;
constexpr uint8_t MAX_ENTITY_NAME_LENGHT = std::numeric_limits<uint8_t>::max();

constexpr uint32_t MAX_COMPONENT = 1024;
using Signature = std::bitset<MAX_COMPONENT>;
constexpr size_t s = sizeof(Signature);
REFLECT(Signature)

END_PCCORE