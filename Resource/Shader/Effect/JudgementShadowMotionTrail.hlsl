uniform sampler2D baseColor : register(s0);

uniform matrix matWorld;
uniform matrix ViewProjection;

uniform float exposure_corr;

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

void VsMain(
	in out float4 pos : POSITION ,
	in out float2 tex : TEXCOORD0,
    in out float2 tex2 : TEXCOORD1,
    in float4 BoneIds0 : BLENDINDICES0,
    in float4 BoneIds1 : BLENDINDICES1,
    in float4 Weights0 : BLENDWEIGHT0,
    in float4 Weights1 : BLENDWEIGHT1,

	out float2 zw : TEXCOORD2)
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
    
    pos = mul(float4(AnimPos.xyz, 1), matWorld);
    pos = mul(pos, ViewProjection);
    zw = pos.zw;
    tex2 = tex2;
};


void PsMain(
	in float2 tex : TEXCOORD0,
    in float2 tex2 : TEXCOORD1,
	in float2 zw : TEXCOORD2,

	out float4 color0 : COLOR0)	
    // depth
{
    //노말맵 g채널 뒤집어주자 . 
     //tnorm.y *= -1.f;
    ////tnorm.x *= -1.f;
    //float temp = tnorm.z;
    //tnorm.z = tnorm.x;
    //tnorm.x = temp;
    
    color0 = tex2D(baseColor, tex);
    // color0.rgb *= float3(1.0f, 0.1f, 0.1f);
    color0.rgb *= 0.003f;
    color0.rgb *= exposure_corr;
};

technique Default
{
    pass p0
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        //zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = false;
        cullmode = none;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};