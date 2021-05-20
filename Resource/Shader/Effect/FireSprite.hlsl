matrix matWorld;
matrix ViewProjection;

uniform float exposure_corr;
uniform float ColorIntencity;
uniform float LifeTimeAlphaFactor;

uniform float SpriteXStart;
uniform float SpriteXEnd;
uniform float SpriteYStart;
uniform float SpriteYEnd;

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

void VsMain(in out float4 Position : POSITION0,
            in out float2 UV0 : TEXCOORD0,
            in out float2 UV1 : TEXCOORD1)
{
    Position = mul(Position, matWorld);
    Position = mul(Position, ViewProjection);
    
    UV0.x = lerp(SpriteXStart, SpriteXEnd, UV0.x);
    UV0.y = lerp(SpriteYStart, SpriteYEnd, UV0.y);
};

void PsMain(out float4 Color : COLOR0,
            in float2 UV0 : TEXCOORD0,
            in float2 UV1 : TEXCOORD1)
{
    Color = tex2D(Albm, UV0);
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
        zenable = true;
        zwriteenable = false;
        sRGBWRITEENABLE = false;
        cullmode = none;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};


