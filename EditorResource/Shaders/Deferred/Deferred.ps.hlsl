

//#define CAMERA_BINDING b0
//#define CAMERA_SPACE space0

#define LIGHT_BUFFER_BINDING b0
#define LIGHT_BUFFER_SPACE space0


//#include "Camera.hlsl"
#include "Light.hlsl"
#include "PBR.hlsl"

// https://github.com/Microsoft/DirectXShaderCompiler/blob/main/docs/SPIR-V.rst#subpass-inputs
[[vk::input_attachment_index(0)]]
SubpassInput<float4>  inputAlbedo : register(t0, space1);
[[vk::input_attachment_index(1)]]
SubpassInput<float4> inputNormal : register(t1, space1);
[[vk::input_attachment_index(2)]]
SubpassInput<float4> inputRoughnessMetallicAo : register(t2, space1);
[[vk::input_attachment_index(3)]]
SubpassInput <float4> inputViewSpacePosition : register(t3, space1);

struct PS_IN
{
	float2 uv : TEXCOORD0;
};

float3 Decode(float2 f)
{
	f = f * 2.0 - 1.0; // Remap [0,1] -> [-1,1]
	float3 n = float3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
	if (n.z < 0.0)
	{
		float oldX = n.x;
		n.x = (1.0 - abs(n.y)) * sign(oldX);
		n.y = (1.0 - abs(oldX)) * sign(n.y);
	}
	return normalize(n);
}

// Pixel shader
float4 Main(PS_IN input) : SV_TARGET
{
	float3 Lo = float3(0, 0, 0);

	float3 albedo = inputAlbedo.SubpassLoad().rgb;
	float3 roughnessMetallicAo = inputRoughnessMetallicAo.SubpassLoad().rgb;
    float3 viewSpacePos = inputViewSpacePosition.SubpassLoad().rgb;
	float3 normal = Decode(inputNormal.SubpassLoad().rg);
		
	float3 N = normal;
	float3 V = -normalize(viewSpacePos);
	float NoV = abs(dot(N, V)) + 1e-5;

	float metallic = roughnessMetallicAo.g;
	float percupetualRoughness = 0.3;
	float roughness = percupetualRoughness * percupetualRoughness;

	for (int i = 0; i < pointLightCount; i++)
	{
		PointLightData pointLight = pointLights[i];

		float3 fragmentViewPosToLight = pointLight.position - viewSpacePos;
		float fragLightDistance = length(fragmentViewPosToLight);

		float3 L = fragmentViewPosToLight / fragLightDistance;
		float3 H = normalize(V + L);
		float NoL = max(dot(N, L), 0.0);
		float NoH = max(dot(N, H), 0.0);
		float LoH = max(dot(L, H), 0.0);

		float attenuation = GetSquareFalloffAttenuation(fragmentViewPosToLight, 1.0 / pointLight.maxRange);
		float3 radiance = pointLight.color * attenuation * pointLight.intensity;

		float3 diffuseColor = (1.0 - metallic) * albedo;
		Lo += BRDF(diffuseColor, NoV, NoL, NoH, LoH, roughness) * radiance;
	}

	return float4(Lo, 1.0);
}
