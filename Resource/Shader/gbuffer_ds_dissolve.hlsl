uniform sampler2D baseColor : register(s0);
uniform sampler2D normalMap : register(s1);

uniform matrix matWorld;
uniform matrix matWorldInv;
uniform matrix matViewProj;

uniform float2 uv = { 1, 1 };
uniform float handedness = -1.0f;

// 디졸브 .
texture DissolveMap;
texture BurnMap;

sampler BurnSampler = sampler_state
{
    texture = BurnMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    sRGBTexture = true;
};

sampler DissolveSampler = sampler_state
{
    texture = DissolveMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    sRGBTexture = false;
};

uniform float3 BurnColor = float3(1, 1, 1);
uniform float  BurnSize = 0.15f;
uniform float  EmissionAmount = 2.f;

float SliceAmount;

// 디졸브 끝 


void vs_gbuffer_tbn(
	in out float4 pos : POSITION,
	in float3 norm : NORMAL,
	in float3 tang : TANGENT,
	in float3 bin : BINORMAL,
	in out float2 tex :  TEXCOORD0,
    in out float2 tex2 : TEXCOORD1,
	out float2 zw :    TEXCOORD2,
	out float3 wnorm : TEXCOORD3,
	out float3 wtan : TEXCOORD4,
	out float3 wbin : TEXCOORD5               )
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
    in float2 tex2 : TEXCOORD1,
	in float2 zw : TEXCOORD2,
	in float3 wnorm : TEXCOORD3,
	in float3 wtan : TEXCOORD4,
	in float3 wbin : TEXCOORD5,
	out float4 color0 : COLOR0, // albedo
	out float4 color1 : COLOR1, // normals
	out float4 color2 : COLOR2    )	// depth
{
    float3x3 tbn =
                float3x3(normalize(float3(wtan)),
                         normalize(float3(wbin)),
                         normalize(float3(wnorm)));
    tbn = transpose(tbn);
    float4 normap_sample = tex2D(normalMap, tex);
    
    float3 tnorm = normap_sample.xyz * 2.0f - 1.0f;
    //노말맵 g채널 뒤집어주자 . 
     //tnorm.y *= -1.f;
    ////tnorm.x *= -1.f;
    //float temp = tnorm.z;
    //tnorm.z = tnorm.x;
    //tnorm.x = temp;
    
    float3 n = normalize(mul(tbn, tnorm));
    
    float4 _Color = tex2D(baseColor, tex);
     // 디졸브.
    float Dissolve = tex2D(DissolveSampler, tex).r - SliceAmount;
    clip(Dissolve);
    float3 CurBurnColor = float3(1, 1, 1);
    if (Dissolve < BurnSize && SliceAmount > 0.0f)
    {
        CurBurnColor = tex2D(BurnSampler, float2(Dissolve * (1.f / BurnSize), 0.f)).rgb;
        CurBurnColor = CurBurnColor * BurnColor * EmissionAmount;
        // 여기서 선택지 컬러를 번 컬러로 할것 인가 컬러에 번 컬러를 곱할 것인가 ?? 
        // DiffuseColor.rgb  = CurBurnColor;
        // DiffuseColor.rgb *= CurBurnColor;
        _Color.rgb += CurBurnColor;
    }
    // 디졸브 끝.

    color0 = _Color;
    color1 = float4(n * 0.5f + 0.5f, normap_sample.w);
    color2 = float4(zw.x / zw.y, 0, 0, 0);
};

technique gbuffer_tbn
{
    pass p0
    {
        StencilEnable = true;
        StencilFunc = always;
        StencilPass = replace;
        StencilRef = 1;
        vertexshader = compile vs_3_0 vs_gbuffer_tbn();
        pixelshader = compile ps_3_0 ps_gbuffer_tbn();
    }
}
