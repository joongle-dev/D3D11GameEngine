#include "Common.hlsl"

struct VS_INPUT
{
    float4 position : POSITION;
    float3 normal   : NORMAL0;
    //float3 tangent  : TANGENT0;
    float2 uv       : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal   : NORMAL0;
    //float3 tangent  : TANGENT0;
    float2 uv       : TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    output.normal = normalize(mul(input.normal, (float3x3) world));
    //output.tangent = normalize(mul(input.tangent, (float3x3)world));
    output.uv = input.uv;

    return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
    float3 lightDir = float3(-1, 0, 0);
    float intensity = clamp(dot(input.normal, -lightDir), 0.15f, 1.0f);
    return float4(float3(1, 1, 1) * intensity, 1);
}