
uniform matrix matWorld;
uniform matrix matViewProj;

uniform float4 lightPos;
uniform float2 clipPlanes;
uniform bool isPerspective; // can't decide from LH matrices

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

void vs_variance(
	in out float4 pos : POSITION, 

    in float4 BoneIds0 : BLENDINDICES0,
    in float4 BoneIds1 : BLENDINDICES1,
    in float4 Weights0 : BLENDWEIGHT0,
    in float4 Weights1 : BLENDWEIGHT1,

	out float2 zw : TEXCOORD0,
	out float4 wpos : TEXCOORD1)
{
    float4 AnimPos = float4(0, 0, 0, 1);
    
    pos.w = 1.0f;
    
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
        }
    }
    
    wpos = mul(float4(AnimPos.xyz, 1), matWorld);
    pos = mul(wpos, matViewProj);

    zw.xy = pos.zw;
}

void ps_variance(
	in float2 zw : TEXCOORD0,
	in float4 wpos : TEXCOORD1,
	out float4 color : COLOR0)
{
    float d = zw.x;

    if (isPerspective)
    {
        float z = length(lightPos.xyz - wpos.xyz);
        d = (z - clipPlanes.x) / (clipPlanes.y - clipPlanes.x);
    }

    color = float4(d, d * d, 0, 1);
}

technique variance
{
    pass p0
    {
        vertexshader = compile vs_3_0 vs_variance();
        pixelshader = compile ps_3_0 ps_variance();
    }
}
