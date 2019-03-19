cbuffer CameraBuffer : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer WorldBuffer : register(b1)
{
    float4x4 world;
}

struct VS_INPUT
{
    float4 position : POSITION;
    float3 normal   : NORMAL0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal   : NORMAL0;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    output.normal   = mul(input.normal, (float3x3) world);
    return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
    float3 lightDir = float3(-1, 0, 0);
    float intensity = clamp(dot(input.normal, -lightDir), 0.2f, 1.0f);
    return float4(1 * intensity, 1 * intensity, 1 * intensity, 1);
}