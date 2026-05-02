#include "DebugHelper/DebugDrawContext.hpp"


#include "Rendering/Renderer.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/ShaderSourceBinary.hpp"
#include "LowRenderer/Rhi.hpp"

#define GIZMO_PASS {1.f, 0.0f, 1.f, 0.5f}

#pragma region GeneratePrimitive
// Thanks to Chat GPT hehe
void GenerateCapsule(std::vector<Tbx::Vector3f>* _verticies, std::vector<uint32_t>* _indicies)
{
}

void GenerateWireCapsule(std::vector<Tbx::Vector3f>* _vertices, std::vector<uint32_t>* _indices)
{
}

#pragma endregion GeneratePrimitive

void PC_CORE::DebugDrawContext::PushRay(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _dir, const float _distance, const Tbx::Vector3f _color)
{

    //const auto p1 = Tbx::Vector4f(static_cast<float>(_p1.x), static_cast<float>(_p1.y), static_cast<float>(_p1.z), 0);
    //const auto dir = Tbx::Vector4f(static_cast<float>(_dir.x), static_cast<float>(_dir.y), static_cast<float>(_dir.z),
    //                               0);
    //const auto color = Tbx::Vector4f(_color.x, _color.y, _color.z, 0);

    //const Tbx::Vector4D p2 = p1 + (dir.Normalize() * _distance);

    /*
    m_Instance->m_RayPrimitiveData.rayBuffer.emplace_back(RayDataPerInstance
        {
            p1, p2, color
        });
    m_Instance->m_RayPrimitiveData.rayCount++;*/
}

void PC_CORE::DebugDrawContext::PushBoxGizmo(PrimitiveType _primitiveType, const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler,
    const Tbx::Vector3d& _size, Tbx::Vector3f _color)
{
    m_DebugDrawPrimitives[static_cast<size_t>(_primitiveType)].emplace_back(_p1, euler, _size, _color);
}

void PC_CORE::DebugDrawContext::PushSphereGizmo(PrimitiveType _primitiveType, const Tbx::Vector3d& _p1, float _radius,
    Tbx::Vector3f _color)
{
    m_DebugDrawPrimitives[static_cast<size_t>(_primitiveType)].emplace_back(_p1, Tbx::Vector3d(0.0f, 0.0f, 0.0f), Tbx::Vector3d(_radius, _radius, _radius), _color);
}


void PC_CORE::DebugDrawContext::PushCapsuleGizmo(PrimitiveType _primitiveType, const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler,
    float _radius, float _height, Tbx::Vector3f _color)
{

    //m_Instance->m_PrimitiveData[static_cast<size_t>(_primitiveType)].matrixBuffer.push_back(m);
}

void PC_CORE::DebugDrawContext::PushFrustum(const Tbx::Matrix4x4d& _FrustumToWorld, bool _IsWired, Tbx::Vector3f _Color)
{
    m_Frustums.emplace_back(Frustum{ _FrustumToWorld, _Color, _IsWired });
}

std::pair< std::vector<Tbx::Vector3f>, std::vector<uint32_t>> PC_CORE::DebugDrawContext::GenerateBasePrimitve(PrimitiveType _primitiveType)
{
    std::vector<Tbx::Vector3f> vertices;
    std::vector<uint32_t> indices;

    switch (_primitiveType)
    {
    case PrimitiveType::Sphere:
        {
            for (size_t lat = 0; lat <= 16; ++lat)
            {
                float theta = lat * M_PI / 16; // Latitude angle (from 0 to PI)
                float sinTheta = std::sin(theta);
                float cosTheta = std::cos(theta);

                for (size_t lon = 0; lon <= 32; ++lon)
                {
                    float phi = lon * 2 * M_PI / 32; // Longitude angle (from 0 to 2PI)
                    float sinPhi = std::sin(phi);
                    float cosPhi = std::cos(phi);

                    Tbx::Vector3f vertex;
                    vertex.x = cosPhi * sinTheta;
                    vertex.y = cosTheta;
                    vertex.z = sinPhi * sinTheta;
                    vertices.push_back(vertex);
                }
            }

            // Define indices
            for (uint32_t lat = 0; lat < 16; ++lat)
            {
                for (uint32_t lon = 0; lon < 32; ++lon)
                {
                    uint32_t first = lat * (32 + 1) + lon;
                    uint32_t second = first + 32 + 1;

                    // Triangle 1: first, second, first+1
                    indices.push_back(first);
                    indices.push_back(second);
                    indices.push_back(first + 1);

                    // Triangle 2: second, second+1, first+1
                    indices.push_back(second);
                    indices.push_back(second + 1);
                    indices.push_back(first + 1);
                }
            }
        }

        break;
    case PrimitiveType::Box:
        vertices =
        {
            Tbx::Vector3f{-0.5f, -0.5f, -0.5f}, // 0
            Tbx::Vector3f{0.5f, -0.5f, -0.5f}, // 1
            Tbx::Vector3f{0.5f, 0.5f, -0.5f}, // 2
            Tbx::Vector3f{-0.5f, 0.5f, -0.5f}, // 3
            Tbx::Vector3f{-0.5f, -0.5f, 0.5f}, // 4
            Tbx::Vector3f{0.5f, -0.5f, 0.5f}, // 5
            Tbx::Vector3f{0.5f, 0.5f, 0.5f}, // 6
            Tbx::Vector3f{-0.5f, 0.5f, 0.5f} // 7
        };

        indices =
        {
            // Bottom face
            0, 1, 2,
            0, 2, 3,
            // Top face
            4, 5, 6,
            4, 6, 7,
            // Front face
            0, 1, 5,
            0, 5, 4,
            // Back face
            2, 3, 7,
            2, 7, 6,
            // Left face
            0, 4, 7,
            0, 7, 3,
            // Right face
            1, 2, 6,
            1, 6, 5
        };

        break;
    //case PrimitiveType::Capsule:
      //  GenerateCapsule(&vertices, &indices);
        //break;
    case PrimitiveType::WireSphere:
        {
            constexpr size_t segments = 24;
            vertices.resize(segments * 6);

            for (size_t i = 0; i < segments; i++)
            {
                float theta0 = i * 2.0f * M_PI / segments;
                float theta1 = (i + 1) * 2.0f * M_PI / segments;
                const size_t offset = i * 6;

                // Circle XY
                vertices[offset + 0] = Tbx::Vector3f{std::cos(theta0), std::sin(theta0), 0.0f};
                vertices[offset + 1] = Tbx::Vector3f{std::cos(theta1), std::sin(theta1), 0.0f};

                // Circle XZ
                vertices[offset + 2] = Tbx::Vector3f{std::cos(theta0), 0.0f, std::sin(theta0)};
                vertices[offset + 3] = Tbx::Vector3f{std::cos(theta1), 0.0f, std::sin(theta1)};

                // Circle YZ
                vertices[offset + 4] = Tbx::Vector3f{0.0f, std::cos(theta0), std::sin(theta0)};
                vertices[offset + 5] = Tbx::Vector3f{0.0f, std::cos(theta1), std::sin(theta1)};
            }

            // Generate indices for LineList
            for (size_t i = 0; i < segments; ++i)
            {
                size_t baseIndex = i * 6;

                // For XY Circle
                indices.push_back(baseIndex + 0);
                indices.push_back(baseIndex + 1);

                // For XZ Circle
                indices.push_back(baseIndex + 2);
                indices.push_back(baseIndex + 3);

                // For YZ Circle
                indices.push_back(baseIndex + 4);
                indices.push_back(baseIndex + 5);
            }

            // To connect the last segment to the first segment to close the circles
            for (int circleOffset = 0; circleOffset < 6; circleOffset += 2)
            {
                size_t lastIndex = (segments - 1) * 6 + circleOffset;
                size_t firstIndex = circleOffset;
                indices.push_back(lastIndex);
                indices.push_back(firstIndex);
            }
        }
        break;
    case PrimitiveType::WireBox:
        vertices =
        {
            Tbx::Vector3f{-0.5f, -0.5f, -0.5f}, // 0
            Tbx::Vector3f{0.5f, -0.5f, -0.5f}, // 1
            Tbx::Vector3f{0.5f, 0.5f, -0.5f}, // 2
            Tbx::Vector3f{-0.5f, 0.5f, -0.5f}, // 3
            Tbx::Vector3f{-0.5f, -0.5f, 0.5f}, // 4
            Tbx::Vector3f{0.5f, -0.5f, 0.5f}, // 5
            Tbx::Vector3f{0.5f, 0.5f, 0.5f}, // 6
            Tbx::Vector3f{-0.5f, 0.5f, 0.5f} // 7
        };
        indices =
        {
            0, 1, // Edge 1
            1, 2, // Edge 2
            2, 3, // Edge 3
            3, 0, // Edge 4
            4, 5, // Edge 5
            5, 6, // Edge 6
            6, 7, // Edge 7
            7, 4, // Edge 8
            0, 4, // Edge 9
            1, 5, // Edge 10
            2, 6, // Edge 11
            3, 7 // Edge 12
        };
        break;
    //case PrimitiveType::WireCapsule:
      //  GenerateWireCapsule(&vertices, &indices);
       // break;
    case PrimitiveType::Count:
    default:
        assert(false);
        break;
    }

    return std::pair<std::vector<Tbx::Vector3f>, std::vector<uint32_t>>(vertices, indices);
}

void PC_CORE::DebugDrawContext::ClearForNextFrame()
{
    for (auto& primitives : m_DebugDrawPrimitives)
        primitives.clear();

    m_Frustums.clear();
}

std::string PC_CORE::DebugDrawContext::PrimitiveTypeToString(PrimitiveType _primitiveType)
{
    switch (_primitiveType)
    {
    case PrimitiveType::Sphere:
        return "Sphere";
    case PrimitiveType::Box:
        return "Box";
    //case PrimitiveType::Capsule:
      //  return "Capsule";
    case PrimitiveType::WireSphere:
        return "WireSphere";
    case PrimitiveType::WireBox:
        return "WireBox";
    //case PrimitiveType::WireCapsule:
        //return "WireCapsule";
    case PrimitiveType::Count:
        break;
    }
    return "";
}
