#include "Common.hlsl"

cbuffer CameraBuffer : register(b0)
{
    float4x4 view;
    float4x4 projection;
    float3 campos;
}

#if SKINNED
cbuffer BoneBuffer : register(b1)
{
    float4x4 bones[96];
}
#else
cbuffer WorldBuffer : register(b1)
{
    float4x4 world;
}
#endif

struct VS_INPUT
{
    float4 position : POSITION;
    float3 normal   : NORMAL0;
    float3 tangent  : TANGENT0;
    float2 uv       : TEXCOORD0;
#if SKINNED
    float4 blendIndices : BLENDINDICES0;
    float4 blendWeights : BLENDWEIGHTS0;
#endif
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 worldpos : POSITION0;
    float2 uv       : TEXCOORD0;
    float3 normal   : NORMAL0;
    float3 tangent  : TANGENT0;
    float3 binormal : BINORMAL0;
    float3 toeye    : DIRECTION0;
};

PS_INPUT VS(VS_INPUT input)
{
#if SKINNED
    float4x4 world = 0;
    for (int i = 0; i < 4; i++)
        world += bones[input.blendIndices[i]] * input.blendWeights[i];
#endif

    PS_INPUT output;

    output.worldpos = mul(input.position, world);
    output.position = mul(output.worldpos, view);
    output.position = mul(output.position, projection);

    output.normal = normalize(mul(input.normal, (float3x3) world));
    output.tangent = normalize(mul(input.tangent, (float3x3) world));
    output.binormal = normalize(cross(output.normal, output.tangent));

    output.toeye = normalize(campos - output.worldpos.xyz);

    output.uv = input.uv;

    return output;
}

cbuffer Light : register(b0)
{
    uint numDirectionalLight;
    uint numPointLight;
    uint numSpotLight;

    struct DirectionalLight
    {
        float3 color;
        float3 direction;
    } directionalLights[4];

    struct PointLight
    {
        float3 color;
        float  range;
        float3 position;
        float  intensity;
    } pointLights[32];

    struct SpotLight
    {
        float3 color;
        float  range;
        float3 position;
        float  intensity;
        float3 direction;
        float  spotangle;
    } spotLights[32];
}

Texture2D AlbedoTexture : register(t0);
Texture2D RoughnessTexture : register(t1);
Texture2D MetallicTexture : register(t2);
Texture2D NormalTexture : register(t3);
Texture2D HeightTexture : register(t4);
SamplerState Sampler : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float GGX(float roughness, float NdotH)
{
    //Trowbridge-Reitz
    float roughnessSqr = roughness * roughness;
    float distribution = NdotH * NdotH * (roughnessSqr - 1.0f) + 1.0f;
    return roughnessSqr * NdotH / (3.1415926535 * distribution * distribution);
}

float4 PS(PS_INPUT input) : SV_TARGET
{
    float3x3 TBN = float3x3(input.tangent, input.binormal, input.normal);
    float3x3 invTBN = transpose(TBN);

    float3 albedo = 1.0f;
    float roughness = 1.0f;
    float metalness = 0.0f;

#if HEIGHT_TEXTURE
    //input.uv += mul(input.toeye, invTBN).xy * HeightTexture.Sample(Sampler, input.uv).r * 0.1f;
#endif
    
#if NORMAL_TEXTURE
    float3 normalsample = NormalTexture.Sample(Sampler, input.uv).rgb * 2.0f - 1.0f;
    input.normal = normalize(mul(normalsample, TBN));
#endif

#if ROUGHNESS_TEXTURE
    roughness = RoughnessTexture.Sample(Sampler, input.uv).r;
#endif
    
#if ALBEDO_TEXTURE
    albedo = AlbedoTexture.Sample(Sampler, input.uv).rgb;
#endif

    float3 color = 0.0f;
    //float NdotV = max(dot(input.toeye, input.normal), 0.0f);

    [loop]
    for (uint dl = 0; dl < numDirectionalLight; dl++)
    {
        float3 incidence = normalize(-directionalLights[dl].direction);
        float  NdotL = max(dot(incidence, input.normal), 0.0f);
        float  NdotH = max(dot(normalize(input.toeye + incidence), input.normal), 0.0f);
        float3 lightcol = directionalLights[dl].color;
        float3 specular = GGX(roughness, NdotH) * (1.0f - roughness) * lightcol;
        color += (specular + albedo * lightcol) * NdotL;
    }
    
    [loop]
    for (uint pl = 0; pl < numPointLight; pl++)
    {
        float3 tolight = pointLights[pl].position - input.worldpos.xyz;
        float  distance = length(tolight);
        float3 incidence = normalize(tolight);
        float  NdotL = max(dot(incidence, input.normal), 0.0f);
        float  NdotH = max(dot(normalize(input.toeye + incidence), input.normal), 0.0f);
        float3 lightcol = pointLights[pl].color;
        float3 specular = GGX(roughness, NdotH) * (1.0f - roughness) * lightcol;
        float attenuation = 1.0f - smoothstep(pointLights[pl].range * pointLights[pl].intensity, pointLights[pl].range, distance);
        color += (specular + albedo * lightcol) * NdotL * attenuation;
    }

    [loop]
    for (uint sl = 0; sl < numSpotLight; sl++)
    {
        float3 tolight = spotLights[sl].position - input.worldpos.xyz;
        float  distance = length(tolight);
        float3 incidence = normalize(tolight);
        float NdotL = max(dot(incidence, input.normal), 0.0f);
        float NdotH = max(dot(normalize(input.toeye + incidence), input.normal), 0.0f);
        float3 lightcol = spotLights[sl].color;
        float3 specular = GGX(roughness, NdotH) * (1.0f - roughness) * lightcol;
        float attenuation = 1.0f - smoothstep(spotLights[sl].range * spotLights[sl].intensity, spotLights[sl].range, distance);
        float mincos = cos(spotLights[sl].spotangle);
        float maxcos = cos(spotLights[sl].spotangle * spotLights[sl].intensity);
        float spotintensity = smoothstep(mincos, maxcos, dot(-incidence, spotLights[sl].direction));
        color += (specular + albedo * lightcol) * NdotL * attenuation * spotintensity;
    }
    
    return float4(color, 1.0f);
}