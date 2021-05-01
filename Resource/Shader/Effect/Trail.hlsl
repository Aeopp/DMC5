matrix matWorld;
matrix ViewProjection;
uniform float DistortionIntencity;
uniform float exposure_corr;
uniform vector _Color;
uniform float ColorIntencity;

uniform float SpriteXStart;
uniform float SpriteXEnd;
uniform float SpriteYStart;
uniform float SpriteYEnd;

texture SpriteMap;
sampler Sprite = sampler_state
{
    texture = SpriteMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = true;
};

texture TrailMap;
sampler Trail = sampler_state
{
    texture = TrailMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = false;
};


void VsMain(in out float4 Position : POSITION0,
            in out float2 UV0 : TEXCOORD0,
            in out float2 UV1 : TEXCOORD1)
{
    Position = mul(Position, matWorld);
    Position = mul(Position, ViewProjection);
};

void PsMain(out float4 Color : COLOR0, 
            out float4 Color1 : COLOR1 ,
            in float2 UV0 : TEXCOORD0 ,
            in float2 UV1 : TEXCOORD1)
{
    // Color = float4(1.0f, 0.0f, 0.0f, 0.5f);
    UV0.x = 1.0f - UV0.x;
    UV1.x = 1.0f - UV1.x;
    
    UV0.x = lerp(SpriteXStart, SpriteXEnd, UV0.x);
    UV0.y = lerp(SpriteYStart, SpriteYEnd, UV0.y);

    Color = tex2D(Sprite, UV0);
    Color *= _Color;
    Color.rgb *= ColorIntencity;
    Color.rgb *= exposure_corr;
    
    Color1 = tex2D(Trail, UV1);    
    Color1.rgb *= DistortionIntencity;
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


