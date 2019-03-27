cbuffer CameraBuffer : register(b0)
{
    float4x4 view;
    float4x4 projection;
    float3 campos;
}

cbuffer WorldBuffer : register(b1)
{
    float4x4 world;
}