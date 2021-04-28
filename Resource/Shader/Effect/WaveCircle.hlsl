matrix matWorld;
matrix ViewProjection;

uniform float Progress;
uniform float Intencity;
uniform vector _Color;
uniform float exposure_corr;

texture WaveMaskMap;
sampler WaveMask = sampler_state
{
    texture = WaveMaskMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = false;
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
    UV.y += (1.f - Progress);
    
    Color = _Color;
    Color.rgb *= Intencity * exposure_corr;
    Color.a = tex2D(WaveMask, UV).r;
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

