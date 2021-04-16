matrix World;
matrix View;
matrix Projection;
//float3 LightDirection = float3(0, -1, 0);

texture ColorMap;
float fAlpha;
sampler Color = sampler_state
{
    texture = ColorMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
};

struct VsIn
{
	float4 Position : POSITION;
    float2 UV : TEXCOORD0;
};

struct VsOut
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
};

VsOut VsMain(VsIn In)
{
    VsOut Out = (VsOut) 0;
    matrix WVP = mul(World, View);
    WVP = mul(WVP, Projection);
    
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    Out.UV = In.UV;
    
    return Out;
};


struct PsIn
{
    float2 UV : TEXCOORD0;
};

struct PsOut
{
    float4 Color : COLOR0;
};

PsOut PsMain(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float4 ColorSample = tex2D(Color, In.UV);
 
    Out.Color = ColorSample;

	Out.Color.a += fAlpha;
    
    return Out;
};

technique Default
{
	pass
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
        //zenable = false;
        zwriteenable = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};