uniform float Time;
uniform float Noisewrap;
uniform float Timecorr;
uniform float3 DistortionColor;
uniform float  DistortionIntencity;
uniform float  Intencity = 1.f;
uniform float2 PixelSize;
uniform float NoiseIntencity;
uniform float exposure_corr;

texture AlbmMap;
sampler Albm = sampler_state
{
    texture = AlbmMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    addressu = wrap;
    addressv = wrap;
    srgbtexture = true;
};

texture NoiseMap;
sampler Noise = sampler_state
{
    texture = NoiseMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    addressu = wrap;
    addressv = wrap;
    srgbtexture = false;
};

void VsMain(
	in out float4 pos : POSITION,
    in out float2 tex : TEXCOORD0)
{
    pos.xy -= PixelSize.xy;
}

void PsMain(
	in float2 tex : TEXCOORD0,
	out float4 color : COLOR0,
    out float4 color1 : COLOR1)
{
    color = tex2D(Albm, tex);
   //  color.rgb *= Intencity;
    
    
    float2 noiseuv = (tex * Noisewrap) + (Time * Timecorr);
    float4 NoiseSample = tex2D(Noise, noiseuv);
    
    color1 = NoiseSample;
    // color1.xy *= NoiseIntencity;
    color.rgb *= DistortionColor;
    color.rgb *= exposure_corr;
};

technique Default
{
    pass p0
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zwriteenable = false;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};
