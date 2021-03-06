matrix matWorld;
matrix ViewProjection;

uniform float DistortionIntencity;
uniform float exposure_corr;
uniform vector _Color;
uniform float ColorIntencity;

uniform matrix InverseProjection;
uniform float SoftParticleDepthScale;

uniform float3 ScrollSpeed;
uniform float3 Scale;

uniform float2 NoiseDistortion0 = float2(1, 1);
uniform float2 NoiseDistortion1 = float2(2, 2);
uniform float2 NoiseDistortion2 = float2(3, 3);

texture DepthMap;
sampler Depth = sampler_state
{
    texture = DepthMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = none;
    sRGBTexture = false;
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
            in out float2 UV0 : TEXCOORD0,
            in out float2 UV1 : TEXCOORD1 ,
            out float4 ClipPosition : TEXCOORD2,
            out float2 UV2 : TEXCOORD3,
            out float2 UV3 : TEXCOORD4,
            out float2 UV4 : TEXCOORD5   )
{
    Position = mul(Position, matWorld);
    ClipPosition = Position = mul(Position, ViewProjection);
    
    
    UV0.x = 1.0F - UV0.x;
    
    UV2 = UV0 * Scale.x + ScrollSpeed.x;
    UV3 = UV0 * Scale.y + ScrollSpeed.y;
    UV4 = UV0 * Scale.z + ScrollSpeed.z;
};


void PsMain(out float4 Color : COLOR0, 
            out float4 Color1 : COLOR1 ,
            in float2 UV0 : TEXCOORD0 ,
            in float2 UV1 : TEXCOORD1 ,
            in float4 ClipPosition : TEXCOORD2 ,

            in float2 UV2 : TEXCOORD3,
            in float2 UV3 : TEXCOORD4,
            in float2 UV4 : TEXCOORD5
)
{
    float2 finalNoise = float2(0, 0);
    
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
    
    finalNoise = Noise1 + Noise2 + Noise3;
    // 노이즈 끝.
    
    // Color = float4(1.0f, 0.0f, 0.0f, 0.5f);
    // UV0.x = 1.0f - UV0.x;
    float2 OriginUV0 = UV0;
    // UV1.x = 1.0f - UV1.x;
    
    Color = _Color;
    float4 trailsample = tex2D(Trail,OriginUV0);
    Color *= trailsample;
    Color.rgb *= ColorIntencity;
    Color.rgb *= exposure_corr;
    
    float4 NoiseSample=tex2D(Noise, OriginUV0 + finalNoise);
    Color1 = NoiseSample;
    Color1.rgb *= DistortionIntencity;
    
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
    Color.a = Color.a *  saturate((scenedistance - particledistance) * SoftParticleDepthScale);
    // 소프트 파티클 끝
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
        cullmode = none;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};


