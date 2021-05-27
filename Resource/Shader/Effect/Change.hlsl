matrix matWorld;
matrix ViewProjection;

vector CurColor;
uniform float Intencity;
uniform float exposure_corr;
uniform float ColorIntencity;

texture AlbmMap;
sampler Albm = sampler_state
{
    texture = AlbmMap;
    minfilter = ANISOTROPIC;
    magfilter = ANISOTROPIC;
    mipfilter = ANISOTROPIC;
    MAXANISOTROPY = 8;
    addressu = wrap;
    addressv = wrap;
    SRGBTEXTURE = false;
};

texture AlpgMap;
sampler Alpg = sampler_state
{
    texture = AlpgMap;
    minfilter = ANISOTROPIC;
    magfilter = ANISOTROPIC;
    mipfilter = ANISOTROPIC;
    MAXANISOTROPY = 8;
    addressu = wrap;
    addressv = wrap;
    SRGBTEXTURE = false;
};

texture MskMap;
sampler Msk = sampler_state
{
    texture = MskMap;
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
    Color = lerp(tex2D(Alpg, UV), tex2D(Msk, UV) * tex2D(Albm, UV), 1.0f - CurColor.a);
    Color.rgb *= ColorIntencity;
    
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

