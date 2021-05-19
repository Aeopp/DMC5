matrix matWorld;
matrix ViewProjection;

uniform float exposure_corr;

uniform matrix InverseProjection;
uniform float SoftParticleDepthScale;

uniform float BlurIntencity = 1.f;

texture BlurMap;
sampler Blur = sampler_state
{
    texture = BlurMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = none;
    sRGBTexture = false;
};

void VsMain(in out float4 Position : POSITION0,
            in out float2 UV0 : TEXCOORD0,
            in out float2 UV1 : TEXCOORD1)
{
    Position = mul(Position, matWorld);
};


void PsMain(out float4 Color : COLOR0,
            out float4 Color1 : COLOR1,
            out float4 Color2 : COLOR2,
            in float2 UV0 : TEXCOORD0,
            in float2 UV1 : TEXCOORD1          )
{
    Color = float4(0, 0, 0, 0);
    
    float4 BlurSample = tex2D(Blur, UV0);
    
    Color2.xy = BlurSample.rg;
    Color2.z = 1.f;
    Color2.w = BlurSample.a;
    
    Color1 = Color2;
};

technique Default
{
    pass p0
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = true;
        zwriteenable = false;
        sRGBWRITEENABLE = false;
        cullmode = none;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};


