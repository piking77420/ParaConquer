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

REFLECT(Vector2i);
REFLECT_MEMBER(Vector2i, x);
REFLECT_MEMBER(Vector2i, y);

REFLECT(Vector2ui);
REFLECT_MEMBER(Vector2ui, x);
REFLECT_MEMBER(Vector2ui, y);
REFLECT(Vector2f);
REFLECT_MEMBER(Vector2f, x);
REFLECT_MEMBER(Vector2f, y);
REFLECT(Vector2d);
REFLECT_MEMBER(Vector2d, x);
REFLECT_MEMBER(Vector2d, y);

REFLECT(Vector3i);
REFLECT_MEMBER(Vector3i, x);
REFLECT_MEMBER(Vector3i, y);
REFLECT_MEMBER(Vector3i, z);

REFLECT(Vector3f);
REFLECT_MEMBER(Vector3f, x);
REFLECT_MEMBER(Vector3f, y);
REFLECT_MEMBER(Vector3f, z);

REFLECT(Vector3d);
REFLECT_MEMBER(Vector3d, x);
REFLECT_MEMBER(Vector3d, y);
REFLECT_MEMBER(Vector3d, z);

REFLECT(Vector4i);
REFLECT_MEMBER(Vector4i, x);
REFLECT_MEMBER(Vector4i, y);
REFLECT_MEMBER(Vector4i, z);
REFLECT_MEMBER(Vector4i, w);

REFLECT(Vector4f);
REFLECT_MEMBER(Vector4f, x);
REFLECT_MEMBER(Vector4f, y);
REFLECT_MEMBER(Vector4f, z);
REFLECT_MEMBER(Vector4f, w);

REFLECT(Vector4d);
REFLECT_MEMBER(Vector4d, x);
REFLECT_MEMBER(Vector4d, y);
REFLECT_MEMBER(Vector4d, z);
REFLECT_MEMBER(Vector4d, w);

// TO DO HANDLE PRIVATE FIELD

REFLECT(Matrix2x2i);
REFLECT(Matrix2x2f);
REFLECT(Matrix2x2d);

REFLECT(Matrix3x3i);
REFLECT(Matrix3x3f);
REFLECT(Matrix3x3d);

REFLECT(Matrix4x4i);
REFLECT(Matrix4x4f);
REFLECT(Matrix4x4d);

REFLECT(Quaternioni);
REFLECT_MEMBER(Quaternioni, imaginary);
REFLECT_MEMBER(Quaternioni, real);

REFLECT(Quaternionf);
REFLECT_MEMBER(Quaternionf, imaginary);
REFLECT_MEMBER(Quaternionf, real);

REFLECT(Quaterniond);
REFLECT_MEMBER(Quaterniond, imaginary);
REFLECT_MEMBER(Quaterniond, real);

#pragma endregion

