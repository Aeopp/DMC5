matrix matWorld;
matrix ViewProjection;

uniform float exposure_corr;

uniform matrix InverseProjection;
uniform float  SoftParticleDepthScale;
uniform float  ColorIntencity;
// uniform float AlphaFactor = 1.f;

uniform vector _Color;
uniform float GradSampleU;

texture DepthMap;
sampler Depth = sampler_state
{
    texture = DepthMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = none;
    sRGBTexture = false;
};

texture GradMap;
sampler Grad = sampler_state
{
    texture = GradMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = true;
};

void VsMain(in out float4 Position : POSITION0,
            in out float2 UV0 : TEXCOORD0,
            out float4 ClipPosition : TEXCOORD1)
{
    Position = mul(Position, matWorld);
    ClipPosition = Position = mul(Position, ViewProjection);
};

void PsMain(out float4 Color  : COLOR0 ,
            in float2 UV0 : TEXCOORD0,
            in float4 ClipPosition : TEXCOORD1)
{
    float2 SampleUV = float2(UV0.x + GradSampleU, UV0.y);
    Color = tex2D(Grad, SampleUV);
    Color.rgba *= _Color;
    Color.rgb  *= ColorIntencity;
    Color.rgb  *= exposure_corr;
    
    // ����Ʈ ��ƼŬ ��� .... 
    // NDC ���� ��ǥ�� Depth UV ��ǥ�� ��ȯ ( ���� XY ���󿡼� ������ ���� ã�� ) 
    float2 vDepthUV = float2(
         (ClipPosition.x / ClipPosition.w) * 0.5f + 0.5f,
         (ClipPosition.y / ClipPosition.w) * -0.5f + 0.5f
                );
    
    // ���� ��ƼŬ�� �� �����̽� �󿡼��� ��ġ�� ���Ѵ��� �Ÿ��� ���Ѵ�. 
    float particledistance = length(mul(ClipPosition, InverseProjection).xyz);
   
    // scene depth ������ ( ���� xy ���󿡼� scene �� �׷��� ���� ���� ���� ) 
    float4 scenepos = mul(float4(ClipPosition.x, ClipPosition.y,
                     tex2D(Depth, vDepthUV).r, 1.f),
                    InverseProjection);
    // ���� �����⸦ �����ؼ� ���� ��ǥ���� �� ��ǥ�� ����ȯ �Ѵ�. 
    scenepos.xyzw /= scenepos.w;
    
    float scenedistance = length(scenepos.xyz);
    Color.a = Color.a * saturate((scenedistance - particledistance) * SoftParticleDepthScale);
    
    // Color.a *= AlphaFactor;
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
        cullmode = ccw;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};


