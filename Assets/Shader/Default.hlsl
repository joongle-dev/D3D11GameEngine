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
    float3 toeye    : DIR0;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    output.position = mul(input.position, world);
    output.toeye = normalize(campos - output.position.xyz);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    output.normal = normalize(mul(input.normal, (float3x3) world));
    output.tangent = normalize(mul(input.tangent, (float3x3)world));
    output.binormal = normalize(mul(input.binormal, (float3x3) world));
    //output.binormal = normalize(cross(output.normal, output.tangent));
    output.uv = input.uv;

    return output;
}

Texture2D AlbedoTexture : register(t0);
Texture2D RoughnessTexture : register(t1);
Texture2D MetallicTexture : register(t2);
Texture2D NormalTexture : register(t3);
Texture2D HeightTexture : register(t4);
SamplerState Sampler : register(s0);

float4 PS(PS_INPUT input) : SV_TARGET
{
    input.tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);
    input.binormal = normalize(cross(input.normal, input.tangent));

    float3x3 TBN = float3x3(input.tangent, input.binormal, input.normal);
    float3x3 invTBN = transpose(TBN);

    float3 color = 0;

    float3 albedo = 1.0f;
    float roughness = 1.0f;
    float metalness = 0.0f;

    float3 lightDir = normalize(float3(0, 0, -1));

    float2 dx = ddx(input.uv);
    float2 dy = ddx(input.uv);
    float fDisplaceFactor = 0.04f;
    float3 vToEyeTangent = normalize(mul(input.toeye, invTBN));
    float fNumStep = lerp(80, 0, abs(dot(float3(0.0f, 0.0f, 1.0f), vToEyeTangent)));
    float2 vDisplace = fDisplaceFactor * vToEyeTangent.xy / vToEyeTangent.z / fNumStep;
    float fStepSize = 1.0f / fNumStep;
    float fCurrentStep = 0.0f;
    float2 vCurrTexcoord = input.uv;
    float2 vPrevTexcoord = input.uv;
    float fCurrHeight = HeightTexture.Sample(Sampler, vCurrTexcoord);
    float fPrevHeight = fCurrHeight;
    [unroll(20)]
    while (fCurrentStep < fCurrHeight)
    {
        vPrevTexcoord = vCurrTexcoord;
        vCurrTexcoord += vDisplace;
        fPrevHeight = fCurrHeight;
        fCurrHeight = HeightTexture.Sample(Sampler, vCurrTexcoord);
        fCurrentStep += fStepSize;
    }
    //float fWeight = fCurrHeight / (fCurrHeight - fPrevHeight);
    //input.uv = vPrevTexcoord * fWeight + vCurrTexcoord * (1.0f - fWeight);
    input.uv = vCurrTexcoord;

    //if (input.uv.x < 0.0f || input.uv.x > 1.0f || input.uv.y < 0.0f || input.uv.y > 1.0f)
    //    discard;

#if HEIGHT

#endif
    
#if NORMAL
    float3 vNormalSample = NormalTexture.Sample(Sampler, input.uv).rgb * 2.0f - 1.0f;
    input.normal = normalize(mul(vNormalSample, TBN));
#endif
    
    float NdotL = max(dot(-lightDir, input.normal), 0.1f);
    float NdotH = max(dot(normalize(input.toeye - lightDir), input.normal), 0.0f);
    float NdotV = max(dot(input.toeye, input.normal), 0.0f);

#if ALBEDO
    albedo = AlbedoTexture.Sample(Sampler, input.uv).rgb;
#endif

#if ROUGHNESS
    roughness = RoughnessTexture.Sample(Sampler, input.uv).r;
#endif
    
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

    color += albedo * roughness;

    return float4(color * NdotL, 1);
}