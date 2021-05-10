matrix Ortho;
matrix Perspective; // xy 회전이 필요한 경우 
matrix ScreenMat; // (-width/2 ~ +width/2, +height/2 ~ -height/2)

//float exposure_corr = 1.f;
float _BrightScale = 1.f;

float2 _MinTexUV = float2(0.f, 0.f);
float2 _MaxTexUV = float2(1.f, 1.f);

float _SliceAmount = 0.f;

bool _UsingNoise = false;

texture ALB0Map;
sampler ALB0 = sampler_state
{
    texture = ALB0Map;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    sRGBTexture = false;
    MaxAnisotropy = 4;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture NoiseMap;
sampler Noise = sampler_state
{
    texture = NoiseMap;
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
};


VsOut VsMain(VsIn In)
{
    VsOut Out = (VsOut) 0;
    
    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Ortho);
    Out.UV.x = (_MaxTexUV.x - _MinTexUV.x) * In.UV.x + _MinTexUV.x;
    Out.UV.y = (_MaxTexUV.y - _MinTexUV.y) * In.UV.y + _MinTexUV.y;
    
    return Out;
};

VsOut VsMain_Perspective(VsIn In)
{
    VsOut Out = (VsOut) 0;

    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Perspective);
    Out.Position.xyz /= Out.Position.w;

    Out.UV.x = (_MaxTexUV.x - _MinTexUV.x) * In.UV.x + _MinTexUV.x;
    Out.UV.y = (_MaxTexUV.y - _MinTexUV.y) * In.UV.y + _MinTexUV.y;
    
    return Out;
};


struct PsIn
{
    float2 UV : TEXCOORD0;
};

struct PsOut
{
    float4 Color : COLOR0;
};

PsOut PsMain(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    if (_UsingNoise)
    {
        float4 NoiseSample = tex2D(Noise, In.UV).bbbb;
        NoiseSample.rgb -= saturate(_SliceAmount);
        clip(NoiseSample);
    }

    Out.Color = tex2D(ALB0, In.UV);
    Out.Color.rgb *= (_BrightScale/* exposure_corr*/);
    Out.Color.a *= saturate(1.f - _SliceAmount);
    
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

        vertexshader = compile vs_3_0 VsMain_Perspective();
        pixelshader = compile ps_3_0 PsMain();
    }
};