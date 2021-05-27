matrix matWorld;
matrix ViewProjection;

uniform float exposure_corr;
uniform float ColorIntencity;
uniform float LifeTimeAlphaFactor;

texture AlbmMap;
sampler Albm = sampler_state
{
    texture = AlbmMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = true;
};

texture NrmrMap;
sampler Nrmr = sampler_state
{
    texture = NrmrMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = false;
};

void VsMain(in out float4 Position : POSITION0,
            in out float2 UV0 : TEXCOORD0,
            in out float2 UV1 : TEXCOORD1)
{
    Position = mul(Position, matWorld);
    Position = mul(Position, ViewProjection);
    
};

void PsMain(out float4 Color : COLOR0,
            in float2 UV0 : TEXCOORD0,
            in float2 UV1 : TEXCOORD1)
{
    Color = tex2D(Albm, UV0);
    Color.rgb *= float4(1.f, 0.25f, 0.25f, 1.f);
    
    Color.rgb *= ColorIntencity;
    Color.rgb *= exposure_corr;
    
    Color.a *= LifeTimeAlphaFactor;
    
};

technique Default
{
    pass p0
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        //zenable = false;
        zwriteenable = true;
        sRGBWRITEENABLE = false;
        cullmode = ccw;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};


