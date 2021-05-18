uniform float2 PixelSize;
float AlphaFactor = 1.f;
// uniform float exposure_corr;

void VsMain(
	in out float4 pos : POSITION)
{
    pos.xy -= PixelSize.xy;
};

void PsMain(
	out float4 color : COLOR0)
{
    color = float4(0, 0, 0, AlphaFactor);
};


technique Default
{
    pass FadeEffect
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
