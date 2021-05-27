matrix matWorld;
matrix ViewProjection;

vector CurColor;
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
    float4 AlbmSample = tex2D(Albm , UV);
    float4 AlpgSample = tex2D(Alpg , UV);
    float4 MskSample = tex2D(Msk ,UV);
    
    float temp = MskSample.b;
    MskSample.b = MskSample.r;
    MskSample.r = temp;
    
    Color.rgb = AlbmSample.rgb * 
                AlpgSample.rgb * MskSample.rgb
                * CurColor.rgb;
    
    Color.a = AlpgSample.a;
    Color.rgb *= ColorIntencity;
    
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


