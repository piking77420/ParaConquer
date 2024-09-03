#pragma once

#include <type_traits>

#include "ecs_header.h"
#include "ecs\ecs_context.h"
#include "ecs\component.h"

#define MAKE_ECS_COMPONENT(T) \
struct T : public Component { \
public: \
static constexpr bool IsComponent = true; \
