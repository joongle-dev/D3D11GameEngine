#include "Common.hlsl"

struct VS_INPUT
{
    float4 position : POSITION;
    float3 normal   : NORMAL0;
    float3 tangent  : TANGENT0;
    float3 binormal : BINORMAL0;
    float2 uv       : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal   : NORMAL0;
    float3 tangent  : TANGENT0;
    float3 binormal : BINORMAL0;
    float2 uv       : TEXCOORD0;
    float3 tocamera : DIR0;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    output.position = mul(input.position, world);
    output.tocamera = normalize(campos - output.position.xyz);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    output.normal = normalize(mul(input.normal, (float3x3) world));
    output.tangent = normalize(mul(input.tangent, (float3x3)world));
    output.binormal = normalize(mul(input.binormal, (float3x3)world));
    output.uv = input.uv;

    return output;
}

Texture2D AlbedoTexture : register(t0);
Texture2D RoughnessTexture : register(t1);
Texture2D MetallicTexture : register(t2);
Texture2D NormalTexture : register(t3);
SamplerState Sampler : register(s0);

float4 PS(PS_INPUT input) : SV_TARGET
{
    float3 color = 0;
    float3 lightDir = normalize(float3(-1, 0, 0));

#ifdef NORMAL
    float3 normal = NormalTexture.Sample(Sampler, input.uv).rgb;
    input.normal = normalize(mul(normal, float3x3(input.tangent, input.binormal, input.normal)));
#endif

#ifdef ALBEDO
    float3 albedo = AlbedoTexture.Sample(Sampler, input.uv).rgb;
    float diffuse = dot(input.normal, -lightDir);
    diffuse = clamp(diffuse, 0.15f, 1.0f);
    color += albedo * diffuse;
#endif

#ifdef ROUGHNESS
    float gloss = RoughnessTexture.Sample(Sampler, input.uv).r;
    float specular = dot(reflect(lightDir, input.normal), normalize(input.tocamera));
    specular = pow(saturate(specular), 20.0f);
    color += gloss * specular;
#endif

    return float4(color, 1);
}