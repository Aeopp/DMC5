matrix World;
matrix ViewProjection;
float3 LightDirection = float3(0.f, -1.f, 0.f);

float exposure_corr = 1.f;
float _BrightScale = 1.f;
float _SliceAmount = 0.f;


texture ALB0Map;
sampler ALB0 = sampler_state
{
    texture = ALB0Map;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    sRGBTexture = false;
    MaxAnisotropy = 4;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture ALP0Map;
sampler ALP0 = sampler_state
{
    texture = ALP0Map;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    sRGBTexture = false;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture NRM0Map;
sampler NRM0 = sampler_state
{
    texture = NRM0Map;
    minfilter = point;
    magfilter = point;
    mipfilter = point;
    sRGBTexture = false;
    AddressU = Wrap;
    AddressV = Wrap;
};



struct VsIn
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
    float2 UV : TEXCOORD0;
};

struct VsOut
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
};

VsOut VsMain(VsIn In)
{
    VsOut Out = (VsOut) 0;
    
    matrix WVP = World;
    WVP = mul(WVP, ViewProjection);
    
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.f), World));
    Out.Tangent = normalize(mul(float4(In.Tangent.xyz, 0.f), World));
    Out.BiNormal = normalize(mul(float4(In.BiNormal.xyz, 0.f), World));
    Out.UV = In.UV;
        
    return Out;
};


struct PsIn
{
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
};

struct PsOut
{
    float4 Color : COLOR0;
};

PsOut PsMain0(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float2 NormalXY = tex2D(NRM0, In.UV).xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));
    
    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
             
    float Diffuse = saturate(dot(WorldNormal, -normalize(LightDirection))) + 0.1f;
    
    Out.Color = tex2D(ALB0, In.UV);
    Out.Color.rgb *= (Diffuse * _BrightScale * exposure_corr);
    Out.Color.a = tex2D(ALP0, In.UV).r * (1.f - _SliceAmount);

    return Out;
};


technique Default
{
	pass p0
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
        zenable = true;
        zwriteenable = true;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain0();
    }
};