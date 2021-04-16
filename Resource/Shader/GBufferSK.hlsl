uniform sampler2D ALBM: register(s0);
uniform sampler2D NRMR: register(s1);

uniform float4x4 World;
uniform float4x4 View;
uniform float4x4 Projection;
uniform float2 UVScale = { 1, 1 };
uniform int nMaxBonesRefPerVtx = 4;
uniform int VTFPitch;

texture VTF;
sampler VTFSampler = sampler_state
{
    texture = VTF;

    minfilter = point;
    magfilter = point;
    mipfilter = point;
    sRGBTexture = false;
};


struct VsIn
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
    float2 UV : TEXCOORD0;
    float4 BoneIds0 : BLENDINDICES0;
    float4 BoneIds1 : BLENDINDICES1;
    float4 Weights0 : BLENDWEIGHT0;
    float4 Weights1 : BLENDWEIGHT1;
};

struct VsOut
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
    float2 UV : TEXCOORD0;
    float2 ZW : TEXCOORD1;
};

VsOut VsGBuffer(VsIn In)
{
    VsOut Out = (VsOut) 0;
    float4x4 ViewProjection = mul(View, Projection);
    
    float4 AnimNormal = float4(0, 0, 0, 0);
    float4 AnimTanget = float4(0, 0, 0, 0);
    float4 AnimBiNormal = float4(0, 0, 0, 0);
    float4 AnimPos = float4(0, 0, 0, 1);
    
    In.Position.w = 1.0f;
    
    const float UVCorrection = 0.5f;
    float FVTFPitch = float(VTFPitch);
    int IVTFPitch = int(VTFPitch);
    
    for (int i = 0; i < 4; ++i)
    {
        int Idx = In.BoneIds0[i] * 4;
        
        float2 VTFUVRow0 = float2((float(Idx % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float(Idx / IVTFPitch) + UVCorrection) / FVTFPitch);
        
        float2 VTFUVRow1 = float2((float((Idx + 1) % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float((Idx + 1) / IVTFPitch) + UVCorrection) / FVTFPitch);
        
        float2 VTFUVRow2 = float2((float((Idx + 2) % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float((Idx + 2) / IVTFPitch) + UVCorrection) / FVTFPitch);
        
        float2 VTFUVRow3 = float2((float((Idx + 3) % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float((Idx + 3) / IVTFPitch) + UVCorrection) / FVTFPitch);
        
        float4x4 AnimMatrix =
        {
            tex2Dlod(VTFSampler, float4(VTFUVRow0, 0.f, 0.f)),
            tex2Dlod(VTFSampler, float4(VTFUVRow1, 0.f, 0.f)),
            tex2Dlod(VTFSampler, float4(VTFUVRow2, 0.f, 0.f)),
            tex2Dlod(VTFSampler, float4(VTFUVRow3, 0.f, 0.f))
        };
        
        AnimTanget += (mul(float4(In.Tangent, 0.f), AnimMatrix) * In.Weights0[i]);
        AnimNormal += (mul(float4(In.Normal, 0.f), AnimMatrix) * In.Weights0[i]);
        AnimBiNormal += (mul(float4(In.BiNormal, 0.f), AnimMatrix) * In.Weights0[i]);
        AnimPos += (mul(In.Position, AnimMatrix) * In.Weights0[i]);
    }
    
    if (nMaxBonesRefPerVtx > 4)
    {
        for (int i = 0; i < 4; ++i)
        {
            int Idx = In.BoneIds1[i] * 4;
        
            float2 VTFUVRow0 = float2((float(Idx % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float(Idx / IVTFPitch) + UVCorrection) / FVTFPitch);
        
            float2 VTFUVRow1 = float2((float((Idx + 1) % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float((Idx + 1) / IVTFPitch) + UVCorrection) / FVTFPitch);
        
            float2 VTFUVRow2 = float2((float((Idx + 2) % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float((Idx + 2) / IVTFPitch) + UVCorrection) / FVTFPitch);
        
            float2 VTFUVRow3 = float2((float((Idx + 3) % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float((Idx + 3) / IVTFPitch) + UVCorrection) / FVTFPitch);
        
            float4x4 AnimMatrix =
            {
                tex2Dlod(VTFSampler, float4(VTFUVRow0, 0.f, 0.f)),
             tex2Dlod(VTFSampler, float4(VTFUVRow1, 0.f, 0.f)),
             tex2Dlod(VTFSampler, float4(VTFUVRow2, 0.f, 0.f)),
                tex2Dlod(VTFSampler, float4(VTFUVRow3, 0.f, 0.f))
            };
        
            AnimTanget += (mul(float4(In.Tangent, 0.f), AnimMatrix) * In.Weights1[i]);
            AnimNormal += (mul(float4(In.Normal, 0.f), AnimMatrix) * In.Weights1[i]);
            AnimBiNormal += (mul(float4(In.BiNormal, 0.f), AnimMatrix) * In.Weights1[i]);
            AnimPos += (mul(In.Position, AnimMatrix) * In.Weights1[i]);
        }
    }
    
    float4x4 WVP = mul(World, ViewProjection);
    Out.UV = In.UV * UVScale;
    Out.Normal = mul(float4(AnimNormal.xyz, 0.f), World);
    Out.Tangent = mul(float4(AnimTanget.xyz, 0.f), World);
    Out.BiNormal = mul(float4(AnimBiNormal.xyz, 0.f), World);
    Out.Position = mul(float4(AnimPos.xyz, 1.f), WVP);
    Out.ZW = Out.Position.zw;
    
    return Out;
}

struct PsIn
{
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
    float2 UV : TEXCOORD0;
    float2 ZW : TEXCOORD1;
};
struct PsOut
{
    vector Albm :  COLOR0;  // Albedo , Metalness
    vector Nrmr :  COLOR1;  // Normal xy , Unknown , Roughness
    vector Depth : COLOR2;  // Depth , 채널 X
};

PsOut PsGBuffer(PsIn In)
{
    PsOut Out = (PsOut)0;
    
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));
    float4 sampleNrmr = tex2D(NRMR, In.UV); 
    float2 NormalXY   = sampleNrmr.xy * 2.0 - 1.0f;
    const float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
    
    float3 PackNormal=normalize(mul(
    normalize(float3(NormalXY, NormalZ)), TBN));
    
    Out.Albm = tex2D(ALBM, In.UV);
    Out.Nrmr = float4(PackNormal.xyz *0.5f+0.5f,sampleNrmr.w);
    /*원근나누기*/
    Out.Depth = float4(In.ZW.x / In.ZW.y,0 ,0 ,0 );
    
    return Out;
};

technique GBuffer
{
    pass
    {
        vertexshader = compile vs_3_0 VsGBuffer();
        pixelshader = compile ps_3_0 PsGBuffer();
    }
}

