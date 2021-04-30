matrix matWorld;
matrix ViewProjection;
uniform float DistortionIntencity;
uniform float exposure;
uniform vector _Color;

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
    Color = tex2D(Trail, UV0);
    Color.a = Color.r;
    Color *= _Color;
    Color.rgb *= exposure * 0.002f;    
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
        cullmode = none;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};


