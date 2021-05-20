matrix matWorld;
matrix ViewProjection;

uniform float BlurIntencity = 1.f;
uniform float DistortionIntencity= 1.f;
uniform float BlurMaxLength=200.f;
uniform float BlurAlpha;
uniform float DistortionAlpha;

uniform float exposure_corr;

uniform matrix InverseProjection;
uniform float SoftParticleDepthScale;

uniform vector _Color;
uniform float AlphaFactor;

void VsMain(in out float4 Position : POSITION0,
            in float4 Normal :NORMAL0,
            in out float2 UV0 : TEXCOORD0,
            in out float2 UV1 : TEXCOORD1,
            out float4 LNormal : TEXCOORD2,        
            out float4 ClipPosition : TEXCOORD3)
{
    LNormal = Position;
    Position = mul(Position, matWorld);
    ClipPosition = Position = mul(Position, ViewProjection);
    
};


void PsMain(out float4 Color : COLOR0,
            out float4 Color1 : COLOR1,
            out float4 Color2 : COLOR2,
            in float2 UV0 : TEXCOORD0,
            in float2 UV1 : TEXCOORD1,
            in float4 LNormal :      TEXCOORD2,
            in float4 ClipPosition : TEXCOORD3)
{
    Color = _Color;
    
    float BlurFactor = saturate(saturate(BlurMaxLength - length(LNormal)) / BlurMaxLength);
    float CurBlurIntencity = lerp(0, BlurIntencity, BlurFactor);
    
    LNormal = normalize(LNormal);
    
    Color1.xy = LNormal.xy * DistortionIntencity;
    Color1.z = 1.f;
    Color1.w = DistortionAlpha;
    
    Color2.xy = LNormal.xy * CurBlurIntencity;
    Color2.z = 1.f;
    Color2.w = BlurAlpha;
};

technique Default
{
    pass p0
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = false;
        cullmode = ccw;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};


