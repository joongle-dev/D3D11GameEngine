cbuffer CameraBuffer : register(b0)
{
    float4x4 view;
    float4x4 projection;
}

cbuffer WorldBuffer : register(b1)
{
    float4x4 world;
}