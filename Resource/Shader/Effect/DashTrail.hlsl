matrix matWorld;
matrix ViewProjection;

uniform float DistortionIntencity;
uniform float exposure_corr;
uniform vector _Color;
uniform float ColorIntencity;

void VsMain(in out float4 Position : POSITION0,
            in out float2 UV0 : TEXCOORD0)
{
    Position = mul(Position, matWorld);
    Position = mul(Position, ViewProjection);
};

void PsMain(out float4 Color : COLOR0, 
            out float4 Color1 : COLOR1 ,
            in float2 UV0 : TEXCOORD0)
{
    Color = _Color;
    Color.rgb *= ColorIntencity;
    Color.rgb *= exposure_corr;
    
    Color1.xy = UV0 *DistortionIntencity;
    Color1.zw= 1.f;
};

technique Default
{
	pass p0
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
        zwriteenable = false;
        sRGBWRITEENABLE = false;
        cullmode = ccw;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};


