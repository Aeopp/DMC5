uniform float Time;
uniform bool Distortion;
uniform float noisewrap;
uniform float timecorr;
uniform vector DistortionColor;

texture SkyMap;
sampler Sky = sampler_state
{
    texture = SkyMap;
    minfilter = ANISOTROPIC;
    magfilter = ANISOTROPIC;
    mipfilter = ANISOTROPIC;
    MAXANISOTROPY = 4;
    addressu = clamp;
    addressv = clamp;
    SRGBTEXTURE = false;
};

texture NoiseMap;
sampler Noise = sampler_state
{
    texture = NoiseMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    addressu= wrap;
    addressv = wrap;
    srgbtexture = false;
};


uniform matrix matViewProj;
uniform matrix matSkyRotation;

uniform float intencity = 1.f;


void vs_main(
	in out float4 pos : POSITION,
    in out float2 tex : TEXCOORD0)
{
    matrix wvp = mul(matSkyRotation, matViewProj);
    pos = mul(pos, wvp);
}

void ps_main(
	in float2 tex    : TEXCOORD0,
	out float4 color : COLOR0 ,
    out float4 color1 : COLOR1)
{
    // tex += NoiseSample.xy;
    color = tex2D(Sky, tex);
    color.rgb *= intencity;
    if (Distortion)
    {
        float2 noiseuv = tex * noisewrap + (Time * timecorr);
        float4 NoiseSample = tex2D(Noise, noiseuv);
        color1 = NoiseSample;
        color.rgba *= DistortionColor;
    }
}

technique sky
{
    pass p0

    {
        zenable = false;
        alphablendenable = false;
        zwriteenable = false;
        fillmode = solid;
        StencilEnable = true;
        StencilRef = 1;
        StencilFunc = notequal;
        vertexshader = compile vs_3_0 vs_main();
        pixelshader = compile ps_3_0 ps_main();
    }
}
