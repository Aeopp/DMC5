matrix World;
matrix ViewProjection;
matrix InverseProjection;

float SoftParticleDepthScale;
float _BrightScale = 1.f;

float2 _MinTexUV = float2(0.f, 0.f);
float2 _MaxTexUV = float2(1.f, 1.f);
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

texture Msk0Map;
sampler Msk0 = sampler_state
{
    texture = Msk0Map;
    minfilter = point;
    magfilter = point;
    mipfilter = point;
    sRGBTexture = false;
    AddressU = Wrap;
    AddressV = Wrap;
};

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
    Out.UV.x = (_MaxTexUV.x - _MinTexUV.x) * In.UV.x + _MinTexUV.x;
    Out.UV.y = (_MaxTexUV.y - _MinTexUV.y) * In.UV.y + _MinTexUV.y;

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

PsOut PsMain(PsIn In)
{
    PsOut Out = (PsOut) 0;

    Out.Color = tex2D(ALB0, In.UV);
    Out.Color.rgb *= _BrightScale;
    
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
        //zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};