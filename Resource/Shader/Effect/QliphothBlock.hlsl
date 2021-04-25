matrix World;

matrix ViewProjection;
matrix InverseProjection;
float SoftParticleDepthScale;

float _AccumulationTexU = 0.f;
float _AccumulationTexV = 0.f;
float _SliceAmount = 0.f;


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

texture ALP0Map;
sampler ALP0 = sampler_state
{
    texture = ALP0Map;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    sRGBTexture = true;
    MaxAnisotropy = 4;
};

texture DepthMap;
sampler Depth = sampler_state
{
    texture = DepthMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = none;
    sRGBTexture = false;
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
    float4 ClipPosition : TEXCOORD1;
};

VsOut VsMain(VsIn In)
{
    VsOut Out = (VsOut) 0;
    
    matrix WVP = World;
    WVP = mul(WVP, ViewProjection);
    
    Out.ClipPosition = Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    Out.UV = In.UV;
        
    return Out;
};


struct PsIn
{
    float2 UV : TEXCOORD0;
    float4 ClipPosition : TEXCOORD1;
};

struct PsOut
{
    float4 Color : COLOR0;
};

PsOut PsMain0(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float2 newUV = In.UV;
    newUV.x += _AccumulationTexU * (1.f - _SliceAmount);
    newUV.y += _AccumulationTexV * (1.f - _SliceAmount);
    
    float4 NoiseSample = tex2D(Noise, newUV);

    Out.Color = float4(0.01f * NoiseSample.g, 0.f, 0.f, saturate(NoiseSample.b * 0.1f + 0.9f) * (1.f - _SliceAmount));
    
    // ����Ʈ ��ƼŬ ��� .... 
    // NDC ���� ��ǥ�� Depth UV ��ǥ�� ��ȯ ( ���� XY ���󿡼� ������ ���� ã�� ) 
    float2 vDepthUV = float2(
         (In.ClipPosition.x / In.ClipPosition.w) * 0.5f + 0.5f,
         (In.ClipPosition.y / In.ClipPosition.w) * -0.5f + 0.5f
                );
    // ���� ��ƼŬ�� �� �����̽� �󿡼��� ��ġ�� ���Ѵ��� �Ÿ��� ���Ѵ�. 
    float particledistance = length(mul(In.ClipPosition, InverseProjection).xyz);
   
    // scene depth ������ ( ���� xy ���󿡼� scene �� �׷��� ���� ���� ���� ) 
    float4 scenepos = mul(float4(In.ClipPosition.x, In.ClipPosition.y, 
                     tex2D(Depth, vDepthUV).r, 1.f),
                    InverseProjection);
    // ���� �����⸦ �����ؼ� ���� ��ǥ���� �� ��ǥ�� ����ȯ �Ѵ�. 
    scenepos.xyzw /= scenepos.w;
    
    float scenedistance = length(scenepos.xyz);
    Out.Color.a = Out.Color.a * saturate((scenedistance - particledistance) * SoftParticleDepthScale);
    // ����Ʈ ��ƼŬ ��
  
    return Out;
};


PsOut PsMain1(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float2 newUV = In.UV;
    newUV.x += _AccumulationTexU;
    newUV.y += _AccumulationTexV;
    
    Out.Color = float4(0.055f * tex2D(Noise, newUV).r, 0.f, 0.f, tex2D(ALP0, In.UV).a * saturate((1.f - _SliceAmount * 2.f)));
    
    // ����Ʈ ��ƼŬ ��� .... 
    // NDC ���� ��ǥ�� Depth UV ��ǥ�� ��ȯ ( ���� XY ���󿡼� ������ ���� ã�� ) 
    float2 vDepthUV = float2(
         (In.ClipPosition.x / In.ClipPosition.w) * 0.5f + 0.5f,
         (In.ClipPosition.y / In.ClipPosition.w) * -0.5f + 0.5f
                );
    // ���� ��ƼŬ�� �� �����̽� �󿡼��� ��ġ�� ���Ѵ��� �Ÿ��� ���Ѵ�. 
    float particledistance = length(mul(In.ClipPosition, InverseProjection).xyz);
   
    // scene depth ������ ( ���� xy ���󿡼� scene �� �׷��� ���� ���� ���� ) 
    float4 scenepos = mul(float4(In.ClipPosition.x, In.ClipPosition.y,
                     tex2D(Depth, vDepthUV).r, 1.f),
                    InverseProjection);
    // ���� �����⸦ �����ؼ� ���� ��ǥ���� �� ��ǥ�� ����ȯ �Ѵ�. 
    scenepos.xyzw /= scenepos.w;
    
    float scenedistance = length(scenepos.xyz);
    Out.Color.a = Out.Color.a * saturate((scenedistance - particledistance) * SoftParticleDepthScale);
    // ����Ʈ ��ƼŬ ��
  
    return Out;
};


technique Default
{
	pass p0
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
        Zenable = true;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain0();
    }
    pass p1
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        Zenable = true;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain1();
    }
};