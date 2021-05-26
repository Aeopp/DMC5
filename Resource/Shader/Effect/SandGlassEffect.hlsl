matrix matWorld;
matrix ViewProjection;

vector CurColor;
uniform float Intencity;
uniform float exposure_corr;

texture MagicMap;
sampler Magic = sampler_state
{
    texture = MagicMap;
    minfilter = ANISOTROPIC;
    magfilter = ANISOTROPIC;
    mipfilter = ANISOTROPIC;
    MAXANISOTROPY = 8;
    addressu = wrap;
    addressv = wrap;
    SRGBTEXTURE = false;
};

texture MagicAlb;
sampler Alb = sampler_state
{
    texture = MagicAlb;
    minfilter = ANISOTROPIC;
    magfilter = ANISOTROPIC;
    mipfilter = ANISOTROPIC;
    MAXANISOTROPY = 8;
    addressu = wrap;
    addressv = wrap;
    SRGBTEXTURE = false;
};

texture MagicMsk;
sampler Msk = sampler_state
{
    texture = MagicMsk;
    minfilter = ANISOTROPIC;
    magfilter = ANISOTROPIC;
    mipfilter = ANISOTROPIC;
    MAXANISOTROPY = 8;
    addressu = wrap;
    addressv = wrap;
    SRGBTEXTURE = false;
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
    // Color = tex2D(Magic, UV) * CurColor;
    Color = lerp(tex2D(Magic, UV), tex2D(Msk, UV) * tex2D(Alb, UV), 1.0f - CurColor.a);
    Color.a *= CurColor.a;
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

