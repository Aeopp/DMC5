matrix World;
matrix ViewProjection;

float _BrightScale = 1.f;

float _SliceAmount = 0.f;
float _TexV = 0.5f;  // 0 ~ 1

texture ALB0Map;
sampler ALB0 = sampler_state
{
    texture = ALB0Map;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    sRGBTexture = false;
    MaxAnisotropy = 4;
};

texture AlphaMap; // 번개모양을 알파로 사용하기 위함
sampler Alpha = sampler_state
{
    texture = AlphaMap;
    minfilter = point;
    magfilter = point;
    mipfilter = point;
};

texture NoiseMap;
sampler Noise = sampler_state
{
    texture = NoiseMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
};


struct VsIn
{
	float4 Position : POSITION;
    float4 Normal : NORMAL;
    float2 UV : TEXCOORD1;
};

struct VsIn_LightningParts
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
    
    matrix WVP = World;
    WVP = mul(WVP, ViewProjection);
    
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    Out.UV = In.UV;
        
    return Out;
};

VsOut VsMain_Lightning(VsIn In)
{
    VsOut Out = (VsOut) 0;
    
    matrix WVP = World;
    WVP = mul(WVP, ViewProjection);
    
    if (_SliceAmount > 0.45f)
        Out.Position = mul(float4((15.f * In.Normal.xyz) + In.Position.xyz, 1.f), WVP);
    else
        Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    
    //Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
   
    Out.UV = In.UV;
        
    return Out;
};

VsOut VsMain_LightningParts(VsIn_LightningParts In)
{
    VsOut Out = (VsOut) 0;
    
    matrix WVP = World;
    WVP = mul(WVP, ViewProjection);
    
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

// 처음에 띄울 하얀색
PsOut PsMain_White(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float2 NewUV = In.UV;
    NewUV.y = _TexV;
    
    float4 NoiseSample = tex2D(Noise, NewUV).rrrr; // r,g,b 각각 다른 노이즈. 그 중 하나만 사용
    NoiseSample.rgb -= _SliceAmount;
    clip(NoiseSample);
 
    Out.Color = tex2D(ALB0, NewUV);
    Out.Color.rgb *= _BrightScale;
    Out.Color.a *= 0.4f;    // 밝기 보정

    return Out;
};

// 그 다음에 띄울 번개색(보라)
PsOut PsMain_Lightning(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float2 NewUV = In.UV;
    NewUV.y = _TexV;
    
    float4 NoiseSample = tex2D(Noise, NewUV).rrrr;
    NoiseSample.rgb -= _SliceAmount;
    clip(NoiseSample);
 
    Out.Color = float4(tex2D(ALB0, NewUV).rgb * _BrightScale, tex2D(Alpha, NewUV).r);
    //Out.Color = float4(In.UV.x, In.UV.y, 0.f, 1.f);
    
    return Out;
};

// 메시 이펙트
PsOut PsMain_LightningParts(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float2 NewUV = In.UV;
    NewUV.y = _TexV;
   
    Out.Color = float4(tex2D(ALB0, NewUV).rgb * _BrightScale, tex2D(Alpha, NewUV).r);
    //Out.Color = float4(In.UV.x, In.UV.y, 0.f, 1.f);
    
    return Out;
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
        pixelshader = compile ps_3_0 PsMain_White();
    }
    pass p1
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        //zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain_Lightning();
        pixelshader = compile ps_3_0 PsMain_Lightning();
    }
    pass p2
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        //zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain_LightningParts();
        pixelshader = compile ps_3_0 PsMain_LightningParts();
    }
};