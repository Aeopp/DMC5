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
    // 0.001f �� �׽�Ʈ�� �־��� ���̸�, tick, timer ���� ������ ��ü�ϸ� �˴ϴ�.
    float2 Trans = UV.xy;
    // Trans.x += Time;
    float4 Noise = tex2D(Distortion, Trans);
    clip(Noise.x-0.000001f);
    
    Noise.xy = Noise.xy * 2.f - 1.f;
    // ������������ UV �� ���� �ֱ� ���� ���
    // 0.05 �� �� �κ��� Ŀ������ UV ������ ���� �������ϴ�.
    UV = UV + (Noise.xy * Intencity);
    float4 Orig = tex2D(Scene, UV);
    Color = Orig;
    Color.a = Noise.a;
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
