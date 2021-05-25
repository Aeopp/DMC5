matrix matWorld;
matrix ViewProjection;

uniform matrix InverseProjection;
uniform float SoftParticleDepthScale;

uniform float exposure_corr;
uniform float ColorIntencity;

uniform float CurveScale;
uniform float Time;
uniform float NoiseFactor;
;


texture AlbmMap;
sampler Albm = sampler_state
{
    texture = AlbmMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = true;
};

texture NoiseMap;
sampler Noise = sampler_state
{
    texture = NoiseMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = false;
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


void VsMain(in out float4 Position : POSITION0,
            in out float2 UV       : TEXCOORD0 ,
            out float4 ClipPosition: TEXCOORD1 )
{
    Position = mul(Position, matWorld);
    ClipPosition = Position = mul(Position, ViewProjection);
    float temp = UV.x;
    UV.x = UV.y;
    UV.y = temp;
    
    //UV.x *= CurveScale;    
    //UV.x += (Time);
};

void PsMain(out float4 Color : COLOR0,
            in float2 UV : TEXCOORD0 ,
            in float4 ClipPosition : TEXCOORD1 ,
            in float Factor : TEXCOORD2)
{
    float2 NoiseUV = UV;
    
    NoiseUV.x *= (NoiseFactor);
    NoiseUV.x += sin(Time);
    float4 NoiseSample = tex2D(Noise, NoiseUV);
    
    UV.xy += NoiseSample.xy * CurveScale;
    
    Color.rgba = tex2D(Albm, UV).rgba;
    Color.rgb *= ColorIntencity;
    
    Color.rgb *= exposure_corr;
    
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
    // ����Ʈ ��ƼŬ ��
};

technique Default
{
	pass p0
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
        //zenable = false;
        cullmode = none;
        zwriteenable = false;
        sRGBWRITEENABLE = false;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};

