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


