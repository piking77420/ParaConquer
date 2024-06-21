#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in float Height;
layout(location = 1) in vec2 TexCoords;

vec4 gLowColor = vec4(0.6,0.38,0.66 , 1.0);
vec4 gHightColor = vec4(0.0,0.15,0.66 , 1.0);

layout(set = 0,binding = 1) uniform sampler2D texSampler;

void main()
{
    /*
    outColor = vec4(TexCoords.xy,0,1);
         return;   
    if (Height < 0)
    {
        outColor = gLowColor;
        return;
    }
    
    outColor = mix(gLowColor, gHightColor , Height);  */
    
    outColor = texture(texSampler,TexCoords);
}