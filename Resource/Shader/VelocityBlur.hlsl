uniform sampler2D RenderScene : register(s0);
uniform vector PixelSize;

uniform float exposure_corr;

uniform float  VelocityBlurIntencity;
uniform float  BlurLengthMin;
// uniform int VelocityBlurSamples;
#define VelocityBlurSamples 32

texture SceneMap;
sampler Scene= sampler_state
{
    texture = SceneMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    addressu = clamp;
    addressv = clamp;
    sRGBTexture = false;
};

texture VelocityBlurMap;
sampler VelocityBlur = sampler_state
{
    texture = VelocityBlurMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    addressu = wrap;
    addressv = wrap;
    sRGBTexture = false;
};

void VsMain(
in out float4 Position : POSITION,
in out float2 UV : TEXCOORD0)
{
   Position.xy -= PixelSize.xy;
};

void PsMain(
in float2 UV : TEXCOORD0,
out float4 Color : COLOR0)
{
    Color = float4(0, 0, 0, 0);
    
    float4 VelocitySample = tex2D(VelocityBlur, UV);
    
    VelocitySample.y *= -1.f;
    
    if (length(VelocitySample.xy) > BlurLengthMin)
    {
        VelocitySample.xy *= VelocityBlurIntencity;
        VelocitySample.xy /= (float) VelocityBlurSamples;
    
        float4 BColor;
        for (int i = 1; i <= VelocityBlurSamples; ++i)
        {
            float2 CurrentUV = UV + (VelocitySample.xy * (float) i);
            BColor = tex2D(Scene, CurrentUV);
        // BColor.a = 1.f;
        //float BColorDepth = tex2D(Velocity2_None1_Depth1Sampler, CurrentUV).w;
        //if (Velocity.a < (BColorDepth + MotionBlurDepthBias))
        //{
        //    Color += BColor;
        //}
            Color += BColor;
        }
        Color /= (float)VelocityBlurSamples;
    }

    Color.rgb *= exposure_corr;
    Color.a *= VelocitySample.a;
};

technique brightpass
{
    pass p0
    {
        srcblend = srcalpha;
        destblend = invsrcalpha;
        alphablendenable = true;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
}
