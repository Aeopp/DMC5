matrix matWorld;
matrix ViewProjection;

float3 CurColor;
uniform float Intencity;
uniform float exposure_corr;

float Lerp;

uniform bool bDecal = false;

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

texture NrmrMap;
sampler Nrmr = sampler_state
{
    texture = NrmrMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
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
    float4 AlbSample = tex2D(Alb, UV);
    float4 MskSample = tex2D(Msk, UV);
    float msk_temp = MskSample.b;
    MskSample.b = MskSample.r;
    MskSample.r = msk_temp;
    
    float4 AlpSample = tex2D(Magic, UV);
    float4 NrmrSample = tex2D(Nrmr, UV);
   
    if (bDecal)
    {
        float3 Normal = normalize(NrmrSample.xyz * 2.f - 1.f);
        float temp = Normal.z;
        Normal.z = Normal.y;
        Normal.y = temp;
        
        Color.rgb = AlbSample.rgb;
        Color.rgb *= saturate(dot(Normal, float3(0, 1, 0)));
    }
    else
    {
        Color.rgb = lerp(AlbSample.rgb, MskSample.rgb, 
                        saturate(Lerp));
    }
    
    Color.a = AlpSample.a;
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
        zwriteenable = true;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};

