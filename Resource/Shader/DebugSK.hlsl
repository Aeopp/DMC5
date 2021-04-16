float4x4 World;
float4x4 ViewProjection;
float2 UVScale = { 1, 1 };
float4 DebugColor = float4(0.7f, 0.0f, 0.3f, 0.5f);
int nMaxBonesRefPerVtx = 4;
int VTFPitch;
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
    float4 BoneIds0 : BLENDINDICES0;
    float4 BoneIds1 : BLENDINDICES1;
    float4 Weights0 : BLENDWEIGHT0;
    float4 Weights1 : BLENDWEIGHT1;
    float2 UV : TEXCOORD0;
};

struct VsOut
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
};

VsOut VsDebugSK(VsIn In)
{
    VsOut Out = (VsOut) 0;
    
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
            AnimPos += (mul(In.Position, AnimMatrix) * In.Weights1[i]);
        }
    }
        
    float4x4 WVP = mul(World, ViewProjection);
    Out.UV = In.UV * UVScale;
    Out.Position = mul(float4(AnimPos.xyz, 1.f), WVP);
    return Out;
}

struct PsIn
{
    float2 UV : TEXCOORD0;
};

struct PsOut
{
    vector Color : COLOR0;
};

PsOut PsDebugSK(PsIn In)
{
    PsOut Out = (PsOut)0;
    Out.Color = DebugColor;
    
    return Out;
};

technique DebugSK
{
    pass
    {
        alphablendenable = true;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;
        cullmode = none;
        fillmode = wireframe;
        vertexshader = compile vs_3_0 VsDebugSK();
        pixelshader = compile ps_3_0 PsDebugSK();
    }
}

