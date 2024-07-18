struct Material
{
    vec4 albedo;
    vec4 metallicSpecularRoughessAnisotropy;
    vec4 emmisiveColorAo;
};


const float PI = 3.14159265359;
const float InvPI = 1 / PI;


float D_GGX ( float NdotH , float m )
{
 // Divide by PI is apply later
 float m2 = m * m ;
 float f = ( NdotH * m2 - NdotH ) * NdotH + 1;
 return m2 / (f * f) ;
}

// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------

float V_SmithGGXCorrelated (float NdotL , float NdotV , float alphaG )
{
    //Original formulation of G_SmithGGX Correlated
    //float lambda_v = ( -1 + sqrt ( alphaG2 * (1 - NdotL2 ) / NdotL2 + 1)) * 0.5 f;
    //float lambda_l = ( -1 + sqrt ( alphaG2 * (1 - NdotV2 ) / NdotV2 + 1)) * 0.5 f;
    //G_SmithGGXCorrelated = 1 / (1 + lambda_v + lambda_l );
    //V_SmithGGXCorrelated = G_SmithGGXCorrelated / (4.0 f * NdotL * NdotV );

    // This is the optimize version
    float alphaG2 = alphaG * alphaG ;
    // Caution : the " NdotL *" and " NdotV *" are explicitely inversed , this is not a mistake .
    float Lambda_GGXV = NdotL * sqrt (( - NdotV * alphaG2 + NdotV ) * NdotV + alphaG2 );
    float Lambda_GGXL = NdotV * sqrt (( - NdotL * alphaG2 + NdotL ) * NdotL + alphaG2 );

 return 0.5f / ( Lambda_GGXV + Lambda_GGXL );
}

// ----------------------------------------------------------------------------
vec3 F_Schlick (vec3 f0 ,float f90 ,float u )
{
	return f0 + ( f90 - f0 ) * pow (1.f - u , 5.f);
}
