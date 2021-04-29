uniform sampler2D RenderScene : register(s0);
uniform vector PixelSize;

uniform float Time;
uniform float Intencity;

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

texture DistortionMap;
sampler Distortion = sampler_state
{
    texture = DistortionMap;
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
    //uv trans
    // 0.001f 은 테스트로 넣어준 값이며, tick, timer 등의 값으로 교체하면 됩니다.
    float2 Trans = UV.xy;
    // Trans.x += Time;
    float4 Noise = tex2D(Distortion, Trans);
    clip(Noise.x-0.000001f);
    
    Noise.xy = Noise.xy * 2.f - 1.f;
    // 오리지날씬의 UV 를 흔들어 주기 위한 계산
    // 0.05 이 값 부분이 커질수록 UV 편차가 더욱 심해집니다.
    UV = UV + (Noise.xy * Intencity);
    float4 Orig = tex2D(Scene, UV);
    Color = Orig;
    Color.a = Noise.a;
};

technique brightpass
{
    pass p0
    {
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
}
