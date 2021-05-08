matrix matWorld;
matrix ViewProjection;

uniform float exposure_corr;

uniform float DistortionIntencity;
uniform float ColorIntencity;
uniform float EmissiveIntencity;

uniform matrix InverseProjection;
uniform float SoftParticleDepthScale;

uniform float ClipRange;

uniform float3 NoiseScale;
uniform float3 NoiseScrollSpeed;

uniform float2 NoiseDistortion0;
uniform float2 NoiseDistortion1;
uniform float2 NoiseDistortion2;

uniform float SpriteProgressTime;
uniform float AlphaFactor;
;

uniform float SpriteXStart;
uniform float SpriteXEnd;
uniform float SpriteYStart;
uniform float SpriteYEnd;

texture DepthMap;
sampler Depth = sampler_state
{
    texture = DepthMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = none;
    sRGBTexture = false;
};


texture SpriteMap;
sampler Sprite = sampler_state
{
    texture = SpriteMap;
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
            in out float2 UV0 : TEXCOORD0,
            in out float2 UV1 : TEXCOORD1,

            out float4 ClipPosition : TEXCOORD2 ,
            out float2 UV2 : TEXCOORD3,
            out float2 UV3 : TEXCOORD4,
            out float2 UV4 : TEXCOORD5            )
{
    Position = mul(Position, matWorld);
    ClipPosition = Position = mul(Position, ViewProjection);
    
    // UV0.x *= (1.f / 8.f);
    
    UV2 = (UV0 * NoiseScale.x) + NoiseScrollSpeed.x;
    UV3 = (UV0 * NoiseScale.y) + NoiseScrollSpeed.y;
    UV4 = (UV0 * NoiseScale.z) + NoiseScrollSpeed.z;
    
    
    UV0.x = lerp(SpriteXStart, SpriteXEnd, UV0.x);
    UV0.y = lerp(SpriteYStart, SpriteYEnd, UV0.y);
};


void PsMain(out float4 Color : COLOR0,
            out float4 Color1 : COLOR1,
            in float2 UV0 : TEXCOORD0,
            in float2 UV1 : TEXCOORD1,

            in float4 ClipPosition : TEXCOORD2,
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
    
    //float2 PrevUV0 = float2(
    //            lerp(SpritePrevXStart, SpritePrevXEnd, UV0.x),
    //            lerp(SpritePrevYStart, SpritePrevYEnd, UV0.y));
    
    // float4 PrevColor = tex2D(Sprite, PrevUV0 + finalNoise);
    
    
    float4 EmissiveSample = tex2D(Emissive, UV0);
    EmissiveSample.rgb *= EmissiveIntencity;
    
    // Color = tex2D(Sprite, UV0);
    // Color = tex2D(Sprite, UV0 + );
    Color = tex2D(Sprite, UV0);
    clip(Color.r - ClipRange);
    Color.a *= AlphaFactor;
    // Color = lerp(PrevColor, Color, SpriteProgressTime);
    
    Color.rgb *= ColorIntencity;
    Color.rgb += EmissiveSample.rgb;
    Color.rgb *= exposure_corr;
    
    float4 trailsample = tex2D(Trail, UV0);
    Color.a *= trailsample.a;
    
    float4 NoiseSample = tex2D(Noise, UV0 + finalNoise);
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
    Color.a = Color.a * saturate((scenedistance - particledistance) * SoftParticleDepthScale);
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


