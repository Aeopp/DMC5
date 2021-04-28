matrix matWorld;
matrix ViewProjection;

vector CurColor;
uniform float Intencity;
uniform float exposure_corr;

texture GradMap;
sampler Grad = sampler_state
{
    texture = GradMap;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = true;
    MaxAnisotropy = 8;
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
    Color = CurColor;
    Color.rgb *= Intencity * exposure_corr;
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

