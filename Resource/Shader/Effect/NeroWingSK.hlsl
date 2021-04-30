matrix World;
matrix ViewProjection;
float3 LightDirection = float3(0.f, -1.f, 0.f);

float _BrightScale = 1.f;
float _SliceAmount = 0.f;
float _AccumulationTexV = 0.f;

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

texture NRMR0Map;
sampler NRMR0 = sampler_state
{
    texture = NRMR0Map;
    minfilter = point;
    magfilter = point;
    mipfilter = point;
    sRGBTexture = false;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture ATOS0Map;
sampler ATOS0 = sampler_state
{
    texture = ATOS0Map;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    sRGBTexture = false;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture GradationMap;
sampler Gradation = sampler_state
{
    texture = GradationMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    sRGBTexture = false;
    AddressU = mirror;
    AddressV = mirror;
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
};

VsOut VsMain0(VsIn In)
{
    VsOut Out = (VsOut) 0;
    
    float4x4 WVP = mul(World, ViewProjection);
    Out.UV = In.UV * UVScale;
    Out.Normal = mul(float4(In.Position.xyz, 0.f), World);
    Out.Tangent = mul(float4(In.Position.xyz, 0.f), World);
    Out.BiNormal = mul(float4(In.Position.xyz, 0.f), World);
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);

    return Out;
};

VsOut VsMain1(VsIn In)
{
    VsOut Out = (VsOut) 0;
 
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

    return Out;
};


struct PsIn
{
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
    float2 UV : TEXCOORD0;
};

struct PsOut
{
    float4 Color : COLOR0;
};

PsOut PsMain0(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float2 newUV = float2(In.UV.y, In.UV.x);
    newUV.x -= _AccumulationTexV;
    
    Out.Color = float4(float3(0.55f, 0.092f, 0.f) * _BrightScale, tex2D(Gradation, newUV).a * 0.2f);

    return Out;
};

PsOut PsMain1(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    float4 ATOSSample = tex2D(ATOS0, In.UV);

    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));
    
    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
             
    float Diffuse = saturate(dot(WorldNormal, -normalize(LightDirection)));
    float4 c1 = float4(Diffuse * ATOSSample.g * _BrightScale * 0.1f * float3(0.106f, 0.f, 0.f), 1.f);
    c1.a = saturate(ATOSSample.g);
    
    float4 c2 = float4(ATOSSample.r * _BrightScale * float3(0.55f, 0.001f, 0.f), 1.f);
    c2.a = saturate(ATOSSample.r);
   
    float4 c3 = float4(ATOSSample.b * _BrightScale * float3(0.121f, 0.002f, 0.f), 1.f);
    c3.a = saturate(ATOSSample.b);

    float crr = saturate(tex2D(Gradation, -float2(0.f, _AccumulationTexV)).r);
    Out.Color = c1 + (1.f - crr) * c2 + (crr) * c3;
    Out.Color.a = saturate(Out.Color.a * 0.4f);

    return Out;
};


technique Default
{
    pass p0
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        Zenable = true;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain0();
        pixelshader = compile ps_3_0 PsMain0();
    }
	pass p1
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
        Zenable = true;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain1();
        pixelshader = compile ps_3_0 PsMain1();
    }
};