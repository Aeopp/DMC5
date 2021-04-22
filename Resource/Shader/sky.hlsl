uniform samplerCUBE environment : register(s0);

uniform matrix matViewProj;
uniform matrix matSkyRotation;
uniform matrix matWorld;

void vs_main(
	in out float4 pos : POSITION,
	out float3 view : TEXCOORD0)
{
    view = mul(float4(pos.xyz,1),matSkyRotation).xyz;
    matrix wvp = mul(matWorld, matViewProj);
    pos = mul(pos, wvp);
}

void ps_main(
	in float3 view : TEXCOORD0,
	out float4 color : COLOR0)
{
     view = normalize(view);
     color = texCUBE(environment, view);
     color.a = 1.f;
}

technique sky
{
    pass p0
    {
        vertexshader = compile vs_3_0 vs_main();
        pixelshader = compile ps_3_0 ps_main();
    }
}
