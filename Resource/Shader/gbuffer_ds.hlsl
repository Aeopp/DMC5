
uniform sampler2D baseColor : register(s0);
uniform sampler2D normalMap : register(s1);

uniform matrix matWorld;
uniform matrix matWorldInv;
uniform matrix matViewProj;

uniform float2 uv = { 1, 1 };
uniform float handedness = -1.0f;

void vs_gbuffer(
	in out float4 pos : POSITION,
	in float3 norm : NORMAL, 
	in out float2 tex : TEXCOORD0,
    in out float2 tex2 : TEXCOORD3,
	out float2 zw : TEXCOORD1,
	out float3 wnorm : TEXCOORD2)
{
    pos = mul(float4(pos.xyz, 1), matWorld);
    wnorm = mul(float4(norm, 0), matWorld).xyz;

    pos = mul(pos, matViewProj);
    zw = pos.zw;

    tex *= uv;
}

void ps_gbuffer(
	in float2 tex : TEXCOORD0,
    in float2 tex2 : TEXCOORD3,
	in float2 zw : TEXCOORD1,
	in float3 wnorm : TEXCOORD2,
	out float4 color0 : COLOR0, // albedo
	out float4 color1 : COLOR1, // normals
	out float4 color2 : COLOR2)	// depth
{
    float3 n = normalize(wnorm);
     
    //float temp = n.z;
    //n.z = n.x;
    //n.x = temp;
    
    color0 = tex2D(baseColor, tex);
    color1 = float4(n * 0.5f + 0.5f, 1);
    color2 = float4(zw.x / zw.y, 0, 0, 0);
}

void vs_gbuffer_tbn(
	in out float4 pos : POSITION,
	in float3 norm : NORMAL,
	in float3 tang : TANGENT,
	in float3 bin : BINORMAL,
	in out float2 tex : TEXCOORD0,
    in out float2 tex2 : TEXCOORD5,
	out float2 zw : TEXCOORD1,
	out float3 wnorm : TEXCOORD2,
	out float3 wtan : TEXCOORD3,
	out float3 wbin : TEXCOORD4)
{
    pos = mul(float4(pos.xyz, 1), matWorld);

    wnorm = mul(float4(norm, 0), matWorld).xyz;
    wtan = mul(float4(tang, 0), matWorld).xyz;
    wbin = mul(float4(bin, 0), matWorld).xyz;

    pos = mul(pos, matViewProj);
    zw = pos.zw;

    tex *= uv;
}

void ps_gbuffer_tbn(
	in float2 tex : TEXCOORD0,
    in float2 tex2 : TEXCOORD5,
	in float2 zw : TEXCOORD1,
	in float3 wnorm : TEXCOORD2,
	in float3 wtan : TEXCOORD3,
	in float3 wbin : TEXCOORD4,
	out float4 color0 : COLOR0, // albedo
	out float4 color1 : COLOR1, // normals
	out float4 color2 : COLOR2)	// depth
{
    float3x3 tbn =
                float3x3(normalize(float3(wtan)),
                         normalize(float3(wbin)),
                         normalize(float3(wnorm)) );
    
    tbn = transpose(tbn);
    float4 normap_sample = tex2D(normalMap, tex);
    
    float3 tnorm = normap_sample.xyz* 2.0f - 1.0f;
    //�븻�� gä�� ���������� . 
     //tnorm.y *= -1.f;
    ////tnorm.x *= -1.f;
    //float temp = tnorm.z;
    //tnorm.z = tnorm.x;
    //tnorm.x = temp;
    
    float3 n = normalize(mul(tbn , tnorm ));
    
    color0 = tex2D(baseColor, tex);
    color1 = float4(n * 0.5f + 0.5f, normap_sample.w);
    color2 = float4(zw.x / zw.y, 0, 0, 0);
}

technique gbuffer
{
    pass p0
    {
        vertexshader = compile vs_3_0 vs_gbuffer();
        pixelshader = compile ps_3_0 ps_gbuffer();
    }
}

technique gbuffer_tbn
{
    pass p0
    {
        vertexshader = compile vs_3_0 vs_gbuffer_tbn();
        pixelshader = compile ps_3_0 ps_gbuffer_tbn();
    }
}
