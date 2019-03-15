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
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);

    return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
    return float4(1, input.position.z / input.position.w, 0, 1);
}