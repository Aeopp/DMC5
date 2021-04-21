uniform sampler2D skytexture : register(s0);

uniform matrix matViewProj;
uniform matrix matSkyRotation;

void vs_main(
	in out float4 pos : POSITION,
    in out float2 tex : TEXCOORD0)
{
    matrix wvp = mul(matSkyRotation, matViewProj);
    pos = mul(pos, wvp);
}

void ps_main(
	in float2 tex    : TEXCOORD0,
	out float4 color : COLOR0)
{
    color = tex2D(skytexture, tex);
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
