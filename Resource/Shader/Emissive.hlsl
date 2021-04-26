matrix ViewProjection;
matrix matWorld;
uniform sampler2D baseColor : register(s0);
float EmissivePower = 3.f;

// texture DepthMap;
// sampler Depth = sampler_state
//{
//    texture = DepthMap;
//    minfilter = linear;
//    magfilter = linear;
//    mipfilter = none;
//    sRGBTexture = false;
//};

struct VsIn
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
};

struct VsOut
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float2 ZW : TEXCOORD1;
};

VsOut VsMain(VsIn In)
{
    VsOut Out = (VsOut) 0;
    Out.Position = mul(float4(In.Position.xyz, 1.f), matWorld);
    Out.UV = In.UV;
    Out.ZW = Out.Position.zw;
    
    return Out;
};

struct PsIn
{
    float2 UV : TEXCOORD0;
    float2 ZW : TEXCOORD1;
};

struct PsOut
{
    float4 Color : COLOR0;
    float4 Depth : COLOR1;
};

PsOut PsMain(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    Out.Color.rgb = tex2D(baseColor, In.UV) * EmissivePower;
    Out.Color.a = 1.f;
    Out.Depth.xyzw = (In.ZW.x / In.ZW.y);
    
    return Out;
};

technique Default
{
	pass p0
	{
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};
