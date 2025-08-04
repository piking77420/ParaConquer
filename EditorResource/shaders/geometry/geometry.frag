#version 450

// Outputs vers le framebuffer (G-Buffer)
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outNormal;
layout(location = 2) out vec4 outRoughnessMetallicAo;
layout(location = 3) out vec4 outWorldPosition;

// Inputs du vertex shader
layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec4 inWorldPosition;
layout(location = 3) in vec2 inTexCoord;

// Texture albedo
layout(set = MATERIAL_DESCRIPTOR_SET, binding = ALBEDO_BINDING) uniform sampler2D u_AlbedoTexture;


// TO DO
//https://advances.realtimerendering.com/s2010/Kaplanyan-CryEngine3%28SIGGRAPH%202010%20Advanced%20RealTime%20Rendering%20Course%29.pdf
//https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/
//https://jcgt.org/published/0003/02/01/
vec2 OctWrap(vec2 v)
{
    return (1.0 - abs(v.yx)) * sign(v);
}

vec2 Encode(vec3 n)
{
    n /= (abs(n.x) + abs(n.y) + abs(n.z));

    if (n.z < 0.0)
        n.xy = OctWrap(n.xy);

    return n.xy * 0.5 + 0.5;
}


void main()
{
    float gamma = 2.2; 
    
    vec3 diffuse = pow(texture(u_AlbedoTexture, inTexCoord).rgb, vec3(gamma)); // linear space to SRGB space
    outColor = vec4(diffuse, 1);

    outNormal = Encode(inNormal);
   
    outRoughnessMetallicAo = vec4(0, 0, 0, 0);
    
    outWorldPosition = inWorldPosition;
}
