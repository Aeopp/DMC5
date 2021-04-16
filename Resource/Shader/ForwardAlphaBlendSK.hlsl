matrix World;
matrix View;
matrix Projection;
float3 LightDirection = float3(0, -1, 0);

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

texture ALBM0Map;
sampler ALBM0 = sampler_state
{
    texture = ALBM0Map;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    sRGBTexture = true;
    MaxAnisotropy = 4;
};

texture NRMR0Map;
sampler NRMR0 = sampler_state
{
    texture = NRMR0Map;
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
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
    float3 WorldPosition : TEXCOORD4;
};

VsOut VsMain(VsIn In)
{
    VsOut Out = (VsOut) 0;
    matrix WVP = mul(World, View);
    WVP = mul(WVP, Projection);
    
    float4 AnimNormal = float4(0, 0, 0, 0);
    float4 AnimTanget = float4(0, 0, 0, 0);
    float4 AnimBiNormal = float4(0, 0, 0, 0);
    float4 AnimPos = float4(0, 0, 0, 1);
    
    In.Position.w = 1.0f;
    
    const float UVCorrection = 0.5f;
    float FVTFPitch = float(VTFPitch);
    int   IVTFPitch = int(VTFPitch);
    
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
        
    
    
    
    
    Out.UV = In.UV;
    Out.Position = mul(float4(AnimPos.xyz, 1.f), WVP);
    Out.Normal = normalize(mul(float4(AnimNormal.xyz, 0.f), World));
    Out.Tangent = normalize(mul(float4(AnimTanget.xyz, 0.f), World));
    Out.BiNormal = normalize(mul(float4(AnimBiNormal.xyz, 0.f), World));
    Out.WorldPosition = mul(float4(AnimPos.xyz, 1.f), World);

    return Out;
};


struct PsIn
{
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
    float3 WorldPosition : TEXCOORD4;
};

struct PsOut
{
    float4 Color : COLOR0;
};

PsOut PsMain(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float4 ALBMSample = tex2D(ALBM0, In.UV);
    float3 Albedo = ALBMSample.xyz;
    float Metalness = ALBMSample.w;
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    float Roughness = NRMRSample.w;
    const float2 NormalXY = NRMRSample.xy *2.f-1.f;
    const float  NormalZ =  sqrt(1 - dot(NormalXY, NormalXY));
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));

    const float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    float Diffuse = saturate(dot(WorldNormal, -normalize(LightDirection)));
     //Out.Color.rgb  =  Diffuse *Albedo;
    Out.Color.rgb = Albedo;
    return Out;
};


technique Default
{
    pass
    {
        alphablendenable = false;
        zenable = true;
        zwriteenable = true;
        sRGBWRITEENABLE = true;
        cullmode = ccw;
        fillmode = solid;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
}


