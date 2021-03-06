matrix matWorld;
matrix ViewProjection;
uniform matrix InverseProjection;

uniform float exposure_corr;
uniform float AlphaFactor;
uniform float ColorIntencity;
uniform float Time;
uniform float DistortionIntencity;

uniform float ScrollSpeed;

uniform float Range;
uniform float EndRange;

uniform float SoftParticleDepthScale;

texture DepthMap;
sampler Depth = sampler_state
{
    texture = DepthMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = none;
    sRGBTexture = false;
};


texture DistortionMap;
sampler Distortion = sampler_state
{
    texture = DistortionMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
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

texture GradMap;
sampler Grad = sampler_state
{
    texture = GradMap;
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

            out float4 ClipPosition : TEXCOORD2 )
{
    Position = mul(Position, matWorld);
    ClipPosition = Position = mul(Position, ViewProjection);
};

void PsMain(out float4 Color : COLOR0,
out float4 Color1 : COLOR1,
            in float2 UV0 : TEXCOORD0,
            in float2 UV1 : TEXCOORD1,

            in float4 ClipPosition : TEXCOORD2
)
{
    Color = tex2D(Albm, UV0);
    Color.a *= AlphaFactor;
    
    float RangeFactor = UV0.y;
    
    if (RangeFactor < Range)
        clip(-1);
    
    if (RangeFactor > EndRange)
        clip(-1);
    
   float factor = sin(Time * ScrollSpeed);
    float cosfactor = cos(Time * ScrollSpeed);
    
    float2 DistortionUV = float2(UV0.x + factor, UV0.y + cosfactor);
    
    Color.rgb *= tex2D(Grad, DistortionUV).rgb;
    Color1 = tex2D(Distortion, float2(UV0.x + factor, UV0.y + cosfactor)) * DistortionIntencity * abs(cosfactor);
    
    Color.rgb *= abs(factor);
    
    Color.rgb *= ColorIntencity;
    Color.rgb *= exposure_corr;
    
    // ?????? ?????? ???? .... 
    // NDC ???? ?????? Depth UV ?????? ???? ( ???? XY ???????? ?????? ???? ???? ) 
    float2 vDepthUV = float2(
         (ClipPosition.x / ClipPosition.w) * 0.5f + 0.5f,
         (ClipPosition.y / ClipPosition.w) * -0.5f + 0.5f
                );
    
    // ???? ???????? ?? ???????? ???????? ?????? ???????? ?????? ??????. 
    float particledistance = length(mul(ClipPosition, InverseProjection).xyz);
   
    // scene depth ???????? ( ???? xy ???????? scene ?? ?????? ???? ???? ???? ) 
    float4 scenepos = mul(float4(ClipPosition.x, ClipPosition.y,
                     tex2D(Depth, vDepthUV).r, 1.f),
                    InverseProjection);
    // ???? ???????? ???????? ???? ???????? ?? ?????? ?????? ????. 
    scenepos.xyzw /= scenepos.w;
    
    float scenedistance = length(scenepos.xyz);
    Color.a = Color.a * saturate((scenedistance - particledistance) * SoftParticleDepthScale);
    // ?????? ?????? ??
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


