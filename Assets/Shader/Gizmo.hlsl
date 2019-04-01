#include "Common.hlsl"

struct VS_INPUT
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    output.color = input.color;
    return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
    return input.color;
}