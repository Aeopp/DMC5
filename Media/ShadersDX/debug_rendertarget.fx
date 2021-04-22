uniform sampler2D sampler0 : register(s0);
uniform matrix ortho;

void vs_main(
	in out float4 pos : POSITION,
	in out float2 tex : TEXCOORD0)
{
    //pos = mul(pos, ortho);
}

void ps_screenquad(
	in float2 tex : TEXCOORD0,
	out float4 color : COLOR0)
{
    color = tex2D(sampler0, tex);
}

technique screenquad
{
    pass p0
    {
        zenable = false;
        vertexshader = compile vs_3_0 vs_main();
        pixelshader = compile ps_3_0 ps_screenquad();
    }
}