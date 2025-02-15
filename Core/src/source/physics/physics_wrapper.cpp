#include "physics/physics_wrapper.hpp"

#include "app.hpp"
#include "physics/box_collider.hpp"
#include "physics/rigid_body.hpp"
#include "physics/sphere_collider.hpp"
#include "time/core_time.hpp"
#include "time/timer.hpp"
#include "world/transform.hpp"

using namespace PC_CORE;


using namespace Tbx;
#pragma region ReflectMathType

REFLECT(Vector2f);
REFLECT(Vector2i);
REFLECT(Vector2ui);
REFLECT(Vector2d);

REFLECT(Vector3f);
REFLECT(Vector3i);
REFLECT(Vector3d);

REFLECT(Vector4f);
REFLECT(Vector4i);
REFLECT(Vector4d);

REFLECT(Matrix2x2f);
REFLECT(Matrix2x2i);
REFLECT(Matrix2x2d);

REFLECT(Matrix3x3f);
REFLECT(Matrix3x3i);
REFLECT(Matrix3x3d);

REFLECT(Matrix4x4f);
REFLECT(Matrix4x4i);
REFLECT(Matrix4x4d);

REFLECT(Quaternionf);
REFLECT(Quaternioni);
REFLECT(Quaterniond);

#pragma endregion

