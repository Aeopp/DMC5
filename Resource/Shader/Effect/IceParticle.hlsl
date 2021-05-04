matrix matWorld;
matrix ViewProjection;

uniform float exposure_corr;

uniform float  ColorIntencity;
uniform float  EmissiveIntencity;
uniform float  AlphaFactor = 1.f;

uniform float3 LightDirection;

texture AlbedoMap;
sampler Albedo = sampler_state
{
    texture = AlbedoMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = true;
};

texture EmissiveMskMap;
sampler Emissive = sampler_state
{
    texture = EmissiveMskMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = false;
};


texture NrmrMap;
sampler Nrmr = sampler_state
{
    texture = NoiseMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = false;
};

void VsMain(in out float4 Position : POSITION0,
            in out float2 UV0 : TEXCOORD0,
            out float4 ClipPosition : TEXCOORD1)
{
    Position = mul(Position, matWorld);
};

void PsMain(out float4 Color  : COLOR0 ,
            in float2 UV0 : TEXCOORD0)
{
   
 Color = tex2D(Albedo, UV0 + finalNoise);

    Color.rgb *=
ColorIntencity;
float4 EmissiveSample = tex2D(Emissive, UV0);
    Color.rgb += (EmissiveSample.rgb * EmissiveIntencity);
    Color.rgb *=
exposure_corr;
    
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
        cullmode = ccw;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};


