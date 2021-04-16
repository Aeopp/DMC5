matrix World;
matrix ViewProjection;
//float3 LightDirection = float3(0, -1, 0);

float _SliceAmount = 0.f;
float _TexV = 0.5f;  // 0 ~ 1

texture ALB0Map;
sampler ALB0 = sampler_state
{
    texture = ALB0Map;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    sRGBTexture = true;
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
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD1;
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
    
    float4 AlbSample = tex2D(ALB0, NewUV);
    float4 NoiseSample = tex2D(Noise, NewUV);
    
    float4 Noise = NoiseSample.rrrr;    // r,g,b 각각 다른 노이즈. 그 중 하나만 사용
    Noise.rgb -= _SliceAmount;
    clip(Noise);
 
    Out.Color = AlbSample;
    Out.Color.a *= 0.4f;    // 밝기 보정

    return Out;
};

// 그 다음에 띄울 번개색(보라)
PsOut PsMain_Lightning(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float2 NewUV = In.UV;
    NewUV.y = _TexV;
    
    float4 AlphaSample = tex2D(Alpha, NewUV);
    float4 AlbSample = tex2D(ALB0, NewUV);
    float4 NoiseSample = tex2D(Noise, NewUV);
    
    float4 Noise = NoiseSample.rrrr;
    Noise.rgb -= _SliceAmount;
    clip(Noise);
 
    Out.Color = float4(AlbSample.rgb, AlphaSample.r);
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
        sRGBWRITEENABLE = true;

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
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain_Lightning();
        pixelshader = compile ps_3_0 PsMain_Lightning();
    }
};