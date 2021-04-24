matrix World;
matrix ViewProjection;

float _AccumulationTexU = 0.f;
float _AccumulationTexV = 0.f;
float _SliceAmount = 0.f;

texture NoiseMap;
sampler Noise = sampler_state
{
    texture = NoiseMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    sRGBTexture = false;
    AddressU = Wrap;
    AddressV = Wrap;
};


struct VsIn
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
};

struct VsOut
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
};

VsOut VsMain(VsIn In)
{
    VsOut Out = (VsOut) 0;
    
    matrix WVP = World;
    WVP = mul(WVP, ViewProjection);
    
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    Out.UV = In.UV;
        
    return Out;
};


struct PsIn
{
    float2 UV : TEXCOORD0;
};

struct PsOut
{
    float4 Color : COLOR0;
};

PsOut PsMain(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float2 newUV = In.UV;
    newUV.x += _AccumulationTexU;
    newUV.y += _AccumulationTexV;
    
    float4 NoiseSample = tex2D(Noise, newUV);

    Out.Color = float4(0.1f * NoiseSample.g, 0.f, 0.f, saturate(NoiseSample.b * 0.1f + 0.9f) * (1.f - _SliceAmount));
  
    return Out;
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

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};