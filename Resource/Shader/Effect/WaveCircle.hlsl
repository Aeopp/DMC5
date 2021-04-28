matrix matWorld;
matrix ViewProjection;

uniform float Progress;
uniform float Intencity;
uniform vector _Color;
uniform float exposure_corr;

uniform float UV_VOffset;

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
            out float4 Color1 : COLOR1 ,
            in float2 UV : TEXCOORD0)
{
    UV.y = (UV.y + (1.f - Progress) + UV_VOffset);
    
    clip(UV.y > 1.5f  ?  -1 : 1);
    
    Color = _Color;
    Color.rgb *= Intencity * exposure_corr;
    float4 Wave = tex2D(WaveMask, UV).r;
    Color.a = Wave .x* _Color.a;
    Color1.rgba = Wave;
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

