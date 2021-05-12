matrix matWorld;
matrix ViewProjection;

uniform matrix InverseProjection;
uniform float SoftParticleDepthScale;

uniform float exposure_corr;
uniform float ColorIntencity;
uniform float LifeTimeAlphaFactor;

uniform float3 _Color;

uniform float SpriteXStart;
uniform float SpriteXEnd;
uniform float SpriteYStart;
uniform float SpriteYEnd;

uniform float Time;


texture DepthMap;
sampler Depth = sampler_state
{
    texture = DepthMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = none;
    sRGBTexture = false;
};

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

texture AlpMskMap;
sampler Alp = sampler_state
{
    texture = AlpMskMap;
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
             out float2 OriginUV0 : TEXCOORD2,
         out float4 ClipPosition : TEXCOORD3)
{
    Position = mul(Position, matWorld);
    ClipPosition =  Position = mul(Position, ViewProjection);
    
    
    OriginUV0 = UV0;
    UV0.x = lerp(SpriteXStart, SpriteXEnd, UV0.x);
    UV0.y = lerp(SpriteYStart, SpriteYEnd, UV0.y);
};

void PsMain(out float4 Color : COLOR0,
            in float2 UV0 : TEXCOORD0,
            in float2 UV1 : TEXCOORD1 ,
    in float2 OriginUV0 : TEXCOORD2,
   in float4 ClipPosition : TEXCOORD3  )
{
    float4 AlpMskSample = tex2D(Alp, UV0);
    
    // Color = tex2D(Albm, OriginUV0);
    Color .rgb=_Color;
    
    Color.a = AlpMskSample.a;
    Color.rgb *= ColorIntencity;    
    Color.rgb *= exposure_corr;
    
    //float AlphaFactor = (1.0f - (abs(OriginUV0.x - 0.5f) / 0.5f) * 0.5f) + (1.0f - (abs(OriginUV0.y - 0.5f) / 0.5f) * 0.5f);
    
    //Color.a *= AlphaFactor;
    
    Color.a *= LifeTimeAlphaFactor;
    
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
    Color.a = Color.a * saturate((scenedistance - particledistance) * (SoftParticleDepthScale * 1.f));
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
        zwriteenable = false;
        sRGBWRITEENABLE = false;
        cullmode = none;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};


