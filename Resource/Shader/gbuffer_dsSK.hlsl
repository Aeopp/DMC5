uniform sampler2D baseColor : register(s0);
uniform sampler2D normalMap : register(s1);

uniform matrix matWorld;
uniform matrix matWorldInv;
uniform matrix matViewProj;

uniform float2 uv = { 1, 1 };
uniform float handedness = -1.0f;

int nMaxBonesRefPerVtx = 4;
int VTFPitch;
texture VTF;
sampler VTFSampler = sampler_state
{
    texture = VTF;

    minfilter = point;
    magfilter = point;
    mipfilter = point;
    sRGBTexture = false;
};

void vs_gbuffer_tbn(
	in out float4 pos : POSITION,
	in float3 Normal: NORMAL,
	in float3 Tangent : TANGENT,
	in float3 BiNormal : BINORMAL,

	in out float2 tex : TEXCOORD0, 
    in float4 BoneIds0 : BLENDINDICES0,
    in float4 BoneIds1 : BLENDINDICES1,
    in float4 Weights0 : BLENDWEIGHT0,
    in float4 Weights1 : BLENDWEIGHT1,

	out float2 zw : TEXCOORD1,
    out float3 WNormal : TEXCOORD2 ,
    out float3 WTangent : TEXCOORD3  ,
    out float3 WBiNormal : TEXCOORD4         )
{
    float4 AnimPos = float4(0, 0, 0, 1);
    
    pos.w = 1.0f;
    float4 AnimNormal = float4(0, 0, 0, 0);
    float4 AnimTanget = float4(0, 0, 0, 0);
    float4 AnimBiNormal = float4(0, 0, 0, 0);
    
    
    const float UVCorrection = 0.5f;
    float FVTFPitch = float(VTFPitch);
    int IVTFPitch = int(VTFPitch);
    
    for (int i = 0; i < 4; ++i)
    {
        int Idx = BoneIds0[i] * 4;
        
        float2 VTFUVRow0 = float2((float(Idx % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float(Idx / IVTFPitch) + UVCorrection) / FVTFPitch);
        
        float2 VTFUVRow1 = float2((float((Idx + 1) % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float((Idx + 1) / IVTFPitch) + UVCorrection) / FVTFPitch);
        
        float2 VTFUVRow2 = float2((float((Idx + 2) % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float((Idx + 2) / IVTFPitch) + UVCorrection) / FVTFPitch);
        
        float2 VTFUVRow3 = float2((float((Idx + 3) % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float((Idx + 3) / IVTFPitch) + UVCorrection) / FVTFPitch);
        
        float4x4 AnimMatrix =
        {
            tex2Dlod(VTFSampler, float4(VTFUVRow0, 0.f, 0.f)),
            tex2Dlod(VTFSampler, float4(VTFUVRow1, 0.f, 0.f)),
            tex2Dlod(VTFSampler, float4(VTFUVRow2, 0.f, 0.f)),
            tex2Dlod(VTFSampler, float4(VTFUVRow3, 0.f, 0.f))
        };
        AnimTanget += (mul(float4(Tangent, 0.f), AnimMatrix) * Weights0[i]);
        AnimNormal += (mul(float4(Normal, 0.f), AnimMatrix) * Weights0[i]);
        AnimBiNormal += (mul(float4(BiNormal, 0.f), AnimMatrix) * Weights0[i]);
        AnimPos += (mul(pos, AnimMatrix) * Weights0[i]);
    }
    
    if (nMaxBonesRefPerVtx > 4)
    {
        for (int i = 0; i < 4; ++i)
        {
            int Idx = BoneIds1[i] * 4;
        
            float2 VTFUVRow0 = float2((float(Idx % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float(Idx / IVTFPitch) + UVCorrection) / FVTFPitch);
        
            float2 VTFUVRow1 = float2((float((Idx + 1) % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float((Idx + 1) / IVTFPitch) + UVCorrection) / FVTFPitch);
        
            float2 VTFUVRow2 = float2((float((Idx + 2) % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float((Idx + 2) / IVTFPitch) + UVCorrection) / FVTFPitch);
        
            float2 VTFUVRow3 = float2((float((Idx + 3) % IVTFPitch) + UVCorrection) / FVTFPitch,
                                  (float((Idx + 3) / IVTFPitch) + UVCorrection) / FVTFPitch);
        
            float4x4 AnimMatrix =
            {
                tex2Dlod(VTFSampler, float4(VTFUVRow0, 0.f, 0.f)),
                 tex2Dlod(VTFSampler, float4(VTFUVRow1, 0.f, 0.f)),
                  tex2Dlod(VTFSampler, float4(VTFUVRow2, 0.f, 0.f)),
                tex2Dlod(VTFSampler, float4(VTFUVRow3, 0.f, 0.f))
            };
            AnimPos += (mul(pos, AnimMatrix) * Weights1[i]);
            AnimTanget += (mul(float4(Tangent, 0.f), AnimMatrix) * Weights1[i]);
            AnimNormal += (mul(float4(Normal, 0.f), AnimMatrix) * Weights1[i]);
            AnimBiNormal += (mul(float4(BiNormal, 0.f), AnimMatrix) * Weights1[i]);
            
        }
    }
    
    pos = mul(float4(AnimPos.xyz, 1), matWorld);
    WNormal= mul(AnimNormal, matWorld).xyz;
    WTangent = mul(AnimTanget, matWorld).xyz;
    WBiNormal = mul(AnimBiNormal, matWorld).xyz;
    
    pos = mul(pos, matViewProj);
    zw = pos.zw;

    tex *= uv;
}

void ps_gbuffer_tbn(
	in float2 tex : TEXCOORD0,
	in float2 zw : TEXCOORD1,
    in float3 WNormal : TEXCOORD2,
    in float3 WTangent : TEXCOORD3,
    in float3 WBiNormal : TEXCOORD4 ,
	
	out float4 color0 : COLOR0, // albedo
	out float4 color1 : COLOR1, // normals
	out float4 color2 : COLOR2)	// depth
{
    float3x3 tbn =
                float3x3(normalize(float3(WTangent)),
                         normalize(float3(WBiNormal)),
                         normalize(float3(WNormal)));
    
    tbn = transpose(tbn);
    float4 normal_sample = tex2D(normalMap, tex);
    float3 tnorm = normal_sample.xyz* 2.0f - 1.0f;
    //노말맵 g채널 뒤집어주자 . 
     //tnorm.y *= -1.f;
    ////tnorm.x *= -1.f;
    //float temp = tnorm.z;
    //tnorm.z = tnorm.x;
    //tnorm.x = temp;
    
    float3 n = normalize(mul(tbn, tnorm));
    
    color0 = tex2D(baseColor, tex);
    color1 = float4(n * 0.5f + 0.5f, normal_sample.w);
    color2 = float4(zw.x / zw.y, 0, 0, 0);
};

technique gbuffer_tbn
{
    pass p0
    {
        vertexshader = compile vs_3_0 vs_gbuffer_tbn();
        pixelshader = compile ps_3_0 ps_gbuffer_tbn();
    }
}
