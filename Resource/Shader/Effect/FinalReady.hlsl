matrix matWorld;
matrix ViewProjection;

uniform matrix InverseProjection;
uniform float SoftParticleDepthScale;

uniform float exposure_corr;

uniform float ColorIntencity;
uniform float AlphaFactor;

texture AlpgMap;
sampler Alpg = sampler_state
{
    texture = AlpgMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = true;
}; 


texture DepthMap;
sampler Depth = sampler_state
{
    texture = DepthMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = none;
    sRGBTexture = false;
};


void VsMain(in out float4 Position : POSITION0,
            in out float2 UV       : TEXCOORD0 ,
            out float4 ClipPosition: TEXCOORD1 )
{
    Position = mul(Position, matWorld);
    ClipPosition = Position = mul(Position, ViewProjection);
};

void PsMain(out float4 Color : COLOR0,
            in float2 UV : TEXCOORD0 ,
            in float4 ClipPosition : TEXCOORD1)
{
    Color = tex2D(Alpg, UV);
    Color.a *= AlphaFactor;
    
    Color.rgb *= ColorIntencity;
    Color.rgb *= exposure_corr;
    
      // ����Ʈ ��ƼŬ ��� .... 
    // NDC ���� ��ǥ�� Depth UV ��ǥ�� ��ȯ ( ���� XY ���󿡼� ������ ���� ã�� ) 
    float2 vDepthUV = float2(
         (ClipPosition.x / ClipPosition.w) * 0.5f + 0.5f,
         (ClipPosition.y / ClipPosition.w) * -0.5f + 0.5f
                );
    
    // ���� ��ƼŬ�� �� �����̽� �󿡼��� ��ġ�� ���Ѵ��� �Ÿ��� ���Ѵ�. 
    float particledistance = length(mul(ClipPosition, InverseProjection).xyz);
   
    // scene depth ������ ( ���� xy ���󿡼� scene �� �׷��� ���� ���� ���� ) 
    float4 scenepos = mul(float4(ClipPosition.x, ClipPosition.y,
                     tex2D(Depth, vDepthUV).r, 1.f),
                    InverseProjection);
    // ���� �����⸦ �����ؼ� ���� ��ǥ���� �� ��ǥ�� ����ȯ �Ѵ�. 
    scenepos.xyzw /= scenepos.w;
    
    float scenedistance = length(scenepos.xyz);
    Color.a = Color.a * saturate((scenedistance - particledistance) * SoftParticleDepthScale);
    // ����Ʈ ��ƼŬ ��
    
    // Color = float4(1, 1, 1, 1);
  
};

technique Default
{
	pass p0
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
        //zenable = false;
        cullmode = none;
        zwriteenable = false;
        sRGBWRITEENABLE = false;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};

