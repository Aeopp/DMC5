matrix matWorld;
matrix ViewProjection;
uniform float DistortionIntencity;
uniform float exposure_corr;
uniform vector _Color;
uniform float ColorIntencity;
uniform float EmissiveIntencity;

uniform matrix InverseProjection;
uniform float SoftParticleDepthScale;

uniform float SpriteXStart;
uniform float SpriteXEnd;
uniform float SpriteYStart;
uniform float SpriteYEnd;

//uniform float3 ScrollSpeed;
//uniform float3 Scale;



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

//texture NoiseMap;
//sampler Noise = sampler_state
//{
//    texture = NoiseMap;
//    minfilter = linear;
//    magfilter = linear;
//    mipfilter = linear;
//    AddressU = wrap;
//    AddressV = wrap;
//    sRGBTexture = false;
//};


void VsMain(in out float4 Position : POSITION0,
            in out float2 UV0 : TEXCOORD0,
            in out float2 UV1 : TEXCOORD1 ,
            out float4 ClipPosition : TEXCOORD2)
            //out float2 UV2 : TEXCOORD2,
            //out float2 UV3 : TEXCOORD3,
            //out float2 UV4 : TEXCOORD4)
{
    Position = mul(Position, matWorld);
    ClipPosition = Position = mul(Position, ViewProjection);
    
    //UV2 = UV0 * Scale.x + ScrollSpeed.x;
    //UV3 = UV0 * Scale.y + ScrollSpeed.y;
    //UV4 = UV0 * Scale.z + ScrollSpeed.z;
};


void PsMain(out float4 Color : COLOR0, 
            out float4 Color1 : COLOR1 ,
            in float2 UV0 : TEXCOORD0 ,
            in float2 UV1 : TEXCOORD1 ,
            in float4 ClipPosition : TEXCOORD2
            //int float2 UV2 :TEXCOORD2 ,
)
{
    // Color = float4(1.0f, 0.0f, 0.0f, 0.5f);
    UV0.x = 1.0f - UV0.x;
    float2 OriginUV0 = UV0;
    float4 EmissiveSample = tex2D(Emissive, OriginUV0);
    EmissiveSample.rgb *= EmissiveIntencity;
    
    UV1.x = 1.0f - UV1.x;
    
    UV0.x = lerp(SpriteXStart, SpriteXEnd, UV0.x);
    UV0.y = lerp(SpriteYStart, SpriteYEnd, UV0.y);

    Color = tex2D(Sprite, UV0);
    
    Color *= _Color;
    Color.rgb *= ColorIntencity;
    Color.rgb += EmissiveSample.rgb;
    Color.rgb *= exposure_corr;
    
    float4 trailsample = tex2D(Trail, UV1);
    Color.a *= trailsample.a;
    
    Color1 = trailsample;
    Color1.rgb *= DistortionIntencity;
    
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
    Color.a = Color.a *  saturate((scenedistance - particledistance) * SoftParticleDepthScale);
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


