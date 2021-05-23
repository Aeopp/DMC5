matrix matWorld;
matrix ViewProjection;

uniform float Intencity;
uniform float exposure_corr;

uniform float ColorIntencity;
uniform float AlphaFactor;

texture AlpgMap;
sampler Alpg = sampler_state
{
    texture = AlpgMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = true;
};

void VsMain(in out float4 Position : POSITION0,
            in out float2 UV : TEXCOORD0)
{
    Position = mul(Position, matWorld);
    Position = mul(Position, ViewProjection);
};

void PsMain(out float4 Color : COLOR0,
            in float2 UV : TEXCOORD0)
{
    Color = tex2D(Alpg, UV);
    Color.rgb *= ColorIntencity;
    Color.a *= AlphaFactor;
    Color.rgb *=  exposure_corr;
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

