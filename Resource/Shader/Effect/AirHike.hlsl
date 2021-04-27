matrix matWorld;
matrix ViewProjection;

uniform float exposure = 1.f;

vector InnerColor;
vector OuterColor;

void VsMain(in out float4 Position : POSITION0,
            in out float2 UV :       TEXCOORD0 )
{
    Position = mul(Position, matWorld);
    Position = mul(Position, ViewProjection);
}

void PsMain(out float4 Color : COLOR0 ,
in  float2 UV : TEXCOORD0)
{
    Color = InnerColor;
    Color.b = smoothstep(InnerColor.b, OuterColor.b, UV.x);
    Color.a  = smoothstep(InnerColor.a , OuterColor.a, UV.x);
  //   Color = OuterColor;
    Color.rgb *= 0.1f;
 //  Color.rg *= (1.f / (exposure * exposure) );
}

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