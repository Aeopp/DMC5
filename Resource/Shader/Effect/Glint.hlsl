matrix World;
matrix ViewProjection;

float exposure_corr = 1.f;
float _BrightScale = 1.f;
float _SliceAmount = 0.f;


texture BaseMap;
sampler Base = sampler_state
{
    texture = BaseMap;
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
    
    matrix WVP = World;
    WVP = mul(WVP, ViewProjection);
    
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    Out.UV = In.UV;
    
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
    
    float4 BaseSample = tex2D(Base, In.UV);

    Out.Color = BaseSample;
    Out.Color.rgb *= (_BrightScale * exposure_corr);
    Out.Color.a *= (1.f - _SliceAmount) * 0.01f;
    
    return Out;
};


technique Default
{
    pass p0
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        //zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};