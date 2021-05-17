matrix Ortho;
matrix Perspective;  // 메시, xy 회전이 필요한 경우 
matrix ScreenMat;    // (-width/2 ~ +width/2, +height/2 ~ -height/2)

float3 LightDirection = float3(0.f, 0.f, 1.f);

float exposure_corr = 1.f;
float _BrightScale = 1.f;

bool _ApplyExposureCorr = true;

float _TotalAccumulateTime;
float _AccumulationTexU;
float _AccumulationTexV;
float _SliceAmount = 0.f;

float2 _MinTexUV = float2(0.f, 0.f);
float2 _MaxTexUV = float2(1.f, 1.f);

float3 _ExtraColor = float3(0.f, 0.f, 0.f);

texture NoiseMap;
sampler Noise = sampler_state
{
    texture = NoiseMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    sRGBTexture = false;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture ALB0Map;
sampler ALB0 = sampler_state
{
    texture = ALB0Map;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    sRGBTexture = true;
    MaxAnisotropy = 4;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture ALB_NOsRGBMap;
sampler ALB_NOsRGB = sampler_state
{
    texture = ALB_NOsRGBMap;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    sRGBTexture = false;
    MaxAnisotropy = 4;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture NRMR0Map;
sampler NRMR0 = sampler_state
{
    texture = NRMR0Map;
    minfilter = point;
    magfilter = point;
    mipfilter = point;
    sRGBTexture = false;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture ATOS0Map;
sampler ATOS0 = sampler_state
{
    texture = ATOS0Map;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
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

struct VsOut_Clip
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
    float4 Clip : TEXCOORD4;
};

struct VsOut_NoiseClip
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
    float2 NoiseCoord0 : TEXCOORD4;
    float2 NoiseCoord1 : TEXCOORD5;
    float2 NoiseCoord2 : TEXCOORD6;
    float Clip : TEXCOORD7;
};

struct VsOut_GUI
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
};


VsOut VsMain(VsIn In)
{
    VsOut Out = (VsOut) 0;

    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Ortho);
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.f), ScreenMat));
    Out.Tangent = normalize(mul(float4(In.BiNormal.xyz, 0.f), ScreenMat));
    Out.BiNormal = normalize(mul(float4(In.Position.xyz, 0.f), ScreenMat));
    Out.UV = In.UV;
    
    return Out;
};

VsOut VsMain_Perspective(VsIn In)
{
    VsOut Out = (VsOut) 0;

    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Perspective);
    Out.Position.xyz /= Out.Position.w;
    
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.f), ScreenMat));
    Out.Tangent = normalize(mul(float4(In.BiNormal.xyz, 0.f), ScreenMat));
    Out.BiNormal = normalize(mul(float4(In.Position.xyz, 0.f), ScreenMat));
    Out.UV = In.UV;
    
    return Out;
};

VsOut_Clip VsMain_ClipPos(VsIn In)
{
    VsOut_Clip Out = (VsOut_Clip) 0;

    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Ortho);
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.f), ScreenMat));
    Out.Tangent = normalize(mul(float4(In.BiNormal.xyz, 0.f), ScreenMat));
    Out.BiNormal = normalize(mul(float4(In.Position.xyz, 0.f), ScreenMat));
    Out.UV = In.UV;        
    Out.Clip = Out.Position;

    return Out;
};

VsOut_GUI VsMain_GUI(VsIn In)
{
    VsOut_GUI Out = (VsOut_GUI) 0;
    
    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Ortho);
    Out.UV.x = (_MaxTexUV.x - _MinTexUV.x) * In.UV.x + _MinTexUV.x;
    Out.UV.y = (_MaxTexUV.y - _MinTexUV.y) * In.UV.y + _MinTexUV.y;

    return Out;
}


struct PsIn
{
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
};

struct PsIn_Clip
{
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
    float4 Clip : TEXCOORD4;
};

struct PsIn_NoiseClip
{
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
    float2 NoiseCoord0 : TEXCOORD4;
    float2 NoiseCoord1 : TEXCOORD5;
    float2 NoiseCoord2 : TEXCOORD6;
    float2 Clip : TEXCOORD7;
};

struct PsIn_GUI
{
    float2 UV : TEXCOORD0;
};

struct PsOut
{
    float4 Color : COLOR0;
};


PsOut PsMain_Plane(PsIn In)
{
    PsOut Out = (PsOut) 0;
        
    float2 NormalXY = tex2D(NRMR0, In.UV).xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));
    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    float Diffuse = saturate(dot(WorldNormal, -normalize(LightDirection)));
    
    float4 ATOSSample = tex2D(ATOS0, In.UV);
    float Ambient = ATOSSample.b * 0.2f;

    Out.Color = (Diffuse + Ambient) * float4(tex2D(ALB_NOsRGB, In.UV).rgb, 1.f) * _BrightScale;
    if (_ApplyExposureCorr)
        Out.Color *= exposure_corr;
    Out.Color.a = ATOSSample.r;
 
    return Out;
};

PsOut PsMain_Mesh(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float2 NormalXY = tex2D(NRMR0, In.UV).xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));
    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    float Diffuse = saturate(dot(WorldNormal, -normalize(LightDirection)));

    Out.Color = Diffuse * float4(tex2D(ALB0, In.UV).rgb + _ExtraColor, 1.f) * _BrightScale * exposure_corr;
    Out.Color.a = 1.f;
    
    return Out;
};

PsOut PsMain_GUI(PsIn_GUI In)
{
    PsOut Out = (PsOut) 0;
    
    Out.Color = tex2D(ALB_NOsRGB, In.UV);
    Out.Color.rgb *= _BrightScale; // * exposure_corr;
    
    return Out;
};


technique Default
{
	pass p0
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain_Plane();
    }
};