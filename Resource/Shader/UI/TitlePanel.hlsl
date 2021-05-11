matrix Ortho;
matrix ScreenMat; // (-width/2 ~ +width/2, +height/2 ~ -height/2)

float exposure_corr = 1.f;
float _BrightScale = 1.f;

float2 _MinTexUV = float2(0.f, 0.f);
float2 _MaxTexUV = float2(1.f, 1.f);
float _SliceAmount = 0.f;

float _LoadingbarCurXPosOrtho;
float _AccumulationTexU;


texture ALB0Map;
sampler ALB0 = sampler_state
{
    texture = ALB0Map;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
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

struct VsIn
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
};

struct VsOut
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float4 Clip : TEXCOORD1;
};


VsOut VsMain(VsIn In)
{
    VsOut Out = (VsOut) 0;
    
    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Ortho);
    Out.UV.x = (_MaxTexUV.x - _MinTexUV.x) * In.UV.x + _MinTexUV.x;
    Out.UV.y = (_MaxTexUV.y - _MinTexUV.y) * In.UV.y + _MinTexUV.y;
    Out.Clip = Out.Position;
    
    return Out;
};


struct PsIn
{
    float2 UV : TEXCOORD0;
    float4 Clip : TEXCOORD1;
};

struct PsOut
{
    float4 Color : COLOR0;
};

PsOut PsMain(PsIn In)
{
    PsOut Out = (PsOut) 0;
   
    Out.Color = tex2D(ALB0, In.UV);
    Out.Color.rgb *= (_BrightScale/* exposure_corr*/);
    Out.Color.a *= saturate(1.f - _SliceAmount);
    
    return Out;
};

PsOut PsMain_Loadingbar0(PsIn In)
{
    PsOut Out = (PsOut) 0;
   
    Out.Color.rgb = float3(0.0117f, 0.862f, 0.753f) * _BrightScale * exposure_corr;
    Out.Color.a = tex2D(ATOS0, In.UV).r;
    
    return Out;
};

PsOut PsMain_Loadingbar1(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    clip(In.UV.y < 0.5f ? 1.f : -1.f);
    clip(_LoadingbarCurXPosOrtho - In.Clip.x);

    // 0.6875f == LoadingbarOrthoOffsetToCenter
    float corr = abs(((In.Clip.x + 0.6875f) / (_LoadingbarCurXPosOrtho + 0.6875f)) - 0.5f) * 2.f + 0.05f;
  
    Out.Color.rgb = float3(0.0117f, 0.962f, 0.853f) * _BrightScale * exposure_corr * corr;
    Out.Color.a = tex2D(ATOS0, In.UV).b * tex2D(ATOS0, In.UV + float2(_AccumulationTexU, 0.f)).g * 0.25f * corr;
    
    return Out;
};

technique Default
{
    pass p0
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
    pass p1
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain_Loadingbar0();
    }
    pass p2
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain_Loadingbar1();
    }
};