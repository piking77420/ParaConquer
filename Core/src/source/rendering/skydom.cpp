#include "rendering/skydom.hpp"

#include <algorithm>
#include <assert.h>

using namespace PC_CORE;

void Skydom::Generate(int32_t _nbrOfRows, int32_t _nbrOfCols, float radius)
{
    /*
    int NumVerticesTopStrip = 3 * _nbrOfRows;
    int NumVerticesRegularStrip = 6 * _nbrOfCols;
    // Compute the coorect size
    nbrOfVerticies = (NumVerticesTopStrip + (_nbrOfRows - 1) * NumVerticesRegularStrip) * 3;

    std::vector<Vertex> vertices(nbrOfVerticies);

    float PitchAngle = 90.0f  / static_cast<float>(_nbrOfRows);
    float HeadingAngle = 360.0f / static_cast<float>(_nbrOfCols);
    float Pitch = -90.0f;

    int i = 0;
    Vector3f Apex(0.0f, radius, 0.0f);

    for (float Heading = 0.0f ; Heading < 360.0f ; Heading += HeadingAngle)
    {

        Vertex v0;
        v0.position = Apex;
        Vector3f v0N = v0.position.Normalize();
        v0.textureCoord = GetTextCoord(v0N.x , v0N.y);
        vertices[i++] = v0; 

        Vector3f Pos1;
        Pos1 = Vector3f::FromSphericalCoord(radius, (Pitch + PitchAngle) * Deg2Rad, (Heading + HeadingAngle) * Deg2Rad);
        Vertex v1;
        v1.position = Pos1;
        Vector3f v1N = v1.position.Normalize();
        v1.textureCoord = GetTextCoord(v1N.x , v1N.y);
        vertices[i++] = v1;

        Vector3f Pos2;
        Pos2 = Vector3f::FromSphericalCoord(radius, (Pitch + PitchAngle) * Deg2Rad, Heading * Deg2Rad);
        Vertex v2;
        v2.position = Pos2;
        Vector3f v2N = v2.position.Normalize();
        v2.textureCoord = GetTextCoord(v2N.x , v2N.y);
        vertices[i++] = v2;
    }

    
    for (Pitch = -90.0f + PitchAngle; Pitch < 90.f ; Pitch += PitchAngle)
    {
        for (float Heading = 0.0f; Heading < 360.0f ; Heading += HeadingAngle)
            {

            Vector3f Pos0;
            Pos0 = Vector3f::FromSphericalCoord(radius, Pitch * Deg2Rad, Heading * Deg2Rad);
            Vertex v0;
            v0.position = Pos0;
            Vector3f v0N = v0.position.Normalize();
            v0.textureCoord = GetTextCoord(v0N.x , v0N.y);

            Vector3f Pos1;
            Pos1 = Vector3f::FromSphericalCoord(radius, Pitch * Deg2Rad, (Heading + HeadingAngle) * Deg2Rad);
            Vertex v1;
            v1.position = Pos1;
            Vector3f v1N = v1.position.Normalize();
            v1.textureCoord = GetTextCoord(v1N.x , v1N.y);

            Vector3f Pos2;
            Pos2 = Vector3f::FromSphericalCoord(radius, (Pitch + PitchAngle) * Deg2Rad, Heading * Deg2Rad);
            Vertex v2;
            v2.position = Pos2;
            Vector3f v2N = v2.position.Normalize();
            v2.textureCoord = GetTextCoord(v2N.x , v2N.y);

            Vector3f Pos3;
            Pos3 = Vector3f::FromSphericalCoord(radius, (Pitch + PitchAngle) * Deg2Rad, (Heading + HeadingAngle) * Deg2Rad);
            Vertex v3;
            v3.position = Pos3;
            Vector3f v3N = v3.position.Normalize();
            v3.textureCoord = GetTextCoord(v3N.x , v3N.y);

            assert(i + 6 <= nbrOfVerticies);

            vertices[i++] = v0;
            vertices[i++] = v1;
            vertices[i++] = v2;

            vertices[i++] = v1;
            vertices[i++] = v3;
            vertices[i++] = v2;
        }
        
    }

    m_VulkanVertexBuffer.Init(vertices);*/
}

void Skydom::Destroy()
{
    m_VulkanVertexBuffer.Destroy();
}

// Spherical Mapping with Normals
Vector2f Skydom::GetTextCoord(float _nx, float _ny)
{
    const float u = (std::asin(_nx) / static_cast<float>(M_PI)) + 0.5f;
    const float v = (std::asin(_ny) / static_cast<float>(M_PI)) + 0.5f;

    return {u, v};
}
