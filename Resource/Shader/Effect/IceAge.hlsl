matrix matWorld;
matrix ViewProjection;

uniform float exposure_corr;

uniform float  DistortionIntencity;
uniform float ColorIntencity;
uniform float  EmissiveIntencity;
uniform float VelocityBlurIntencity = 1.f;
uniform float VelocityBlurAlpha = 1.f;

uniform matrix InverseProjection;
uniform float  SoftParticleDepthScale;
uniform float AlphaFactor = 1.f;

uniform float3 NoiseScale;
uniform float3 NoiseScrollSpeed;

uniform float2 NoiseDistortion0;
uniform float2 NoiseDistortion1;
uniform float2 NoiseDistortion2;

uniform bool bNoise;

texture DepthMap;
sampler Depth = sampler_state
{
    texture = DepthMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = none;
    sRGBTexture = false;
};


texture AlbedoMap;
sampler Albedo = sampler_state
{
    texture = AlbedoMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = true;
};

texture TrailMap;
sampler Trail = sampler_state
{
    texture = TrailMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = false;
};

texture EmissiveMskMap;
sampler Emissive = sampler_state
{
    texture = EmissiveMskMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = false;
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

void VsMain(in out float4 Position : POSITION0,
            in float4 Normal : NORMAL0,
            in out float2 UV0 : TEXCOORD0,
            out float4 ClipPosition : TEXCOORD1,
            out float2 UV2 : TEXCOORD2,
            out float2 UV3 : TEXCOORD3,
            out float2 UV4 : TEXCOORD4 ,
            out float4 LNormal : TEXCOORD5 )
{
    Position = mul(Position, matWorld);
    ClipPosition = Position = mul(Position, ViewProjection);
    LNormal = Normal;
    
    UV2 = (UV0 * NoiseScale.x) + NoiseScrollSpeed.x;
    UV3 = (UV0 * NoiseScale.y) + NoiseScrollSpeed.y;
    UV4 = (UV0 * NoiseScale.z) + NoiseScrollSpeed.z;
};

void PsMain(out float4 Color  : COLOR0,
            out float4 Color1 : COLOR1,
            out float4 Color2 : COLOR2,
            in float2 UV0 : TEXCOORD0,
            in float4 ClipPosition : TEXCOORD1,
            in float2 UV2 : TEXCOORD2,
            in float2 UV3 : TEXCOORD3,
            in float2 UV4 : TEXCOORD4 ,
            in float4 LNormal : TEXCOORD5
)
{
    // 노이즈 시작 
    
    float4 Noise1 = tex2D(Noise, UV2);
    float4 Noise2 = tex2D(Noise, UV3);
    float4 Noise3 = tex2D(Noise, UV4);

    Noise1 = (Noise1 - 0.5f) * 2.0f;
    Noise2 = (Noise2 - 0.5f) * 2.0f;
    Noise3 = (Noise3 - 0.5f) * 2.0f;
    
    Noise1.xy = Noise1.xy * NoiseDistortion0.xy;
    Noise2.xy = Noise2.xy * NoiseDistortion1.xy;
    Noise3.xy = Noise3.xy * NoiseDistortion2.xy;
    
    float2  finalNoise = Noise1 + Noise2 + Noise3;
    // .....
    
    
    Color = tex2D(Albedo, UV0 + finalNoise);    
    float4 NoiseSample = tex2D(Noise, UV0 + finalNoise);
    float4 trailsample = tex2D(Trail, UV0);
    Color *= trailsample.b;
    Color.rgb *= ColorIntencity;
    float4 EmissiveSample = tex2D(Emissive, UV0);
    Color.rgb += (EmissiveSample.rgb * EmissiveIntencity);
    Color.rgb *= exposure_corr;
    
    // 디스토션 시작 
    Color1 = NoiseSample;
    Color1.rgb *= DistortionIntencity;
    // ....
    
    
    
    
    // 소프트 파티클 계산 .... 
    // NDC 투영 좌표를 Depth UV 좌표로 변환 ( 같은 XY 선상에서 투영된 깊이 찾자 ) 
    float2 vDepthUV = float2(
         (ClipPosition.x / ClipPosition.w) * 0.5f + 0.5f,
         (ClipPosition.y / ClipPosition.w) * -0.5f + 0.5f
                );
    
    // 현재 파티클의 뷰 스페이스 상에서의 위치를 구한다음 거리를 구한다. 
    float particledistance = length(mul(ClipPosition, InverseProjection).xyz);
   
    // scene depth 얻어오기 ( 같은 xy 선상에서 scene 에 그려진 제일 낮은 깊이 ) 
    float4 scenepos = mul(float4(ClipPosition.x, ClipPosition.y,
                     tex2D(Depth, vDepthUV).r, 1.f),
                    InverseProjection);
    // 투영 나누기를 수행해서 투영 좌표에서 뷰 좌표로 역변환 한다. 
    scenepos.xyzw /= scenepos.w;
    
    float scenedistance = length(scenepos.xyz);
    Color.a = Color.a * saturate((scenedistance - particledistance) * SoftParticleDepthScale);
    
    Color.a *= AlphaFactor;
    // ....... 
    
    // 속도 블러 테스트
    Color2.xy = LNormal.xy * VelocityBlurIntencity;
    Color2.z = 1.f;
    Color2.w = VelocityBlurAlpha;
    // Color2 = float4(1(속도), 1(속도), 1(의미없음), 1 <-1로하길 바람);
    //
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


