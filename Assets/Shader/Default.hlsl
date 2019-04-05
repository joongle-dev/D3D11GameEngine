#include "Common.hlsl"

cbuffer Light : register(b2)
{
    float3 lightpos;
    float3 lightdir;
}

struct VS_INPUT
{
    float4 position : POSITION;
    float3 normal   : NORMAL0;
    float3 tangent  : TANGENT0;
    float2 uv       : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD0;
    float3 normal   : TEXCOORD1;
    float3 tangent  : TEXCOORD2;
    float3 binormal : TEXCOORD3;
    float3 toeye    : TEXCOORD4;
    float4 tolight  : TEXCOORD5;
    float3 lightdir : TEXCOORD6;
    float4 lightcol : TEXCOORD7;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    output.position = mul(input.position, world);

    output.toeye = normalize(campos - output.position.xyz);
    
    output.tolight.xyz = lightpos - output.position.xyz;
    output.tolight.w = length(output.tolight.xyz);
    output.tolight.xyz = normalize(output.tolight.xyz);
    output.lightdir = normalize(lightdir);
    
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);

    output.normal = normalize(mul(input.normal, (float3x3)world));
    output.tangent = normalize(mul(input.tangent, (float3x3)world));
    output.binormal = normalize(cross(output.normal, output.tangent));

    output.uv = input.uv;

    return output;
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

float4 PS(PS_INPUT input) : SV_TARGET
{
    float3x3 TBN = float3x3(input.tangent, input.binormal, input.normal);
    float3x3 invTBN = transpose(TBN);

    float3 color = 0;

    float3 albedo = 1.0f;
    float roughness = 1.0f;
    float metalness = 0.0f;

#if HEIGHT
    input.uv += mul(input.toeye, invTBN).xy * HeightTexture.Sample(Sampler, input.uv).r * 0.1f;
#endif
    
#if NORMAL
    float3 normalsample = NormalTexture.Sample(Sampler, input.uv).rgb * 2.0f - 1.0f;
    input.normal = normalize(mul(normalsample, TBN));
#endif
    
#if DIRECTIONAL_LIGHT
    float NdotL = max(dot(-input.lightdir, input.normal), 0.0f);
#else
    float NdotL = max(dot(input.tolight.xyz, input.normal), 0.0f);
#endif
    float NdotH = max(dot(normalize(input.toeye + input.tolight.xyz), input.normal), 0.0f);
    float NdotV = max(dot(input.toeye, input.normal), 0.0f);

#if ROUGHNESS
    roughness = RoughnessTexture.Sample(Sampler, input.uv).r;

    //GGX
    //float NdotHSqr = NdotH * NdotH;
    //float TanNdotHSqr = (1 - NdotHSqr) / NdotHSqr;
    //color += (1.0 / 3.1415926535) * sqrt(roughness / (NdotHSqr * (roughnessSqr + TanNdotHSqr)));
    
    //Trowbridge-Reitz
    float roughnessSqr = roughness * roughness;
    float distribution = NdotH * NdotH * (roughnessSqr - 1.0) + 1.0;
    float specular = roughnessSqr / (3.1415926535 * distribution * distribution);
    color += float3(1, 1, 1) * specular * (1.0f - roughness);

    //Blinn-Phong
    //float gloss = 1.0f - roughness;
    //float specular = pow(max(NdotH, 0.0f), gloss * 255.0f);
    //color += float3(1, 1, 1) * specular * (1.0f - roughness);
#endif
    
#if ALBEDO
    albedo = AlbedoTexture.Sample(Sampler, input.uv).rgb;
#endif
    
    color += albedo * roughness;

#if DIRECTIONAL_LIGHT
    return float4(color * NdotL, 1.0f);
#endif

#if POINT_LIGHT
    return float4(color * NdotL, saturate(2.0f / input.tolight.w));
#endif

#if SPOT_LIGHT
#endif

    return float4(color * NdotL, 1.0f);
}