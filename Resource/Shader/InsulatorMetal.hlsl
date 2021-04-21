#define NUM_MIPS 8
#define ONE_OVER_PI	0.3183098861837906

uniform sampler2D   albmsp        : register(s0);
uniform sampler2D   scene         : register(s1);
uniform sampler2D   nrmrsp        : register(s2);
uniform sampler2D   depthsp       : register(s3);

uniform samplerCUBE irradiance1 : register(s4);
uniform samplerCUBE irradiance2 : register(s5);
uniform sampler2D   brdfLUT     : register(s6);

uniform matrix matViewProjInv;

uniform float4 eyePos;
uniform float2 pixelSize;

void vs_main(
	in out float4 pos : POSITION,
	in out float2 tex : TEXCOORD0)
{
    pos.xy -= pixelSize.xy;
};

void ps_main(
	in float2 tex    : TEXCOORD0,
    out float4 Color : COLOR0)
{
    float depth = tex2D(depthsp, tex).r;
    
    if (depth > 0.0f)
    {
        float4 albm = tex2D(albmsp, tex);
        float4 nrmr = tex2D(nrmrsp, tex);
   
        // ���� �븻 . 
        float3 albedo = albm.rgb;
        // ���� ��ġ ����ŷ
        float4 wpos = float4(tex.x * 2 - 1, 1 - 2 * tex.y, depth, 1);
        wpos = mul(wpos, matViewProjInv);
        wpos /= wpos.w;
        float metal = saturate(albm.a);
        float roughness = saturate(nrmr.a);
    
        float3 v       = normalize(eyePos.xyz - wpos.xyz);
        float3 n       = normalize(nrmr.xyz * 2.0f - 1.0f);
        float3 r       = reflect(-v, n);
    
        float ndotv    = max(dot(n, v), 0.0);
        float miplevel = roughness * (NUM_MIPS - 1);
    
        // �ݻ�� �������� ��ǻ�� ����ŧ�� ���� ����
        float3 diff_irrad = texCUBE(irradiance1, r).rgb;
        float3 spec_irrad = texCUBElod(irradiance2, float4(r, miplevel)).rgb;
    
        // ��� ���̺� ...  
        // �븻�� �ü����� ���̰� �������� u �� 0 �� �����
    
        // ��ĥ�Ⱑ ���� ��ü�ϼ��� 
        // v �� ������ 
        float2 f0_scale_bias = tex2D(brdfLUT, float2(ndotv, roughness)).rg;
    
        // ���� ���̵� ������ ��ü�ϱ� �ٶ� ..
        float light_diffuse = 1.f;
        float light_specular = 1.f;
        
        // ���� * �˺��� *  PI ;
        float3 brdf_diff = diff_irrad * albedo.rgb * ONE_OVER_PI * light_diffuse;
        float3 brdf_spec = spec_irrad * (f0_scale_bias.x + f0_scale_bias.y) * light_specular;
        
        Color.rgb = (brdf_diff * (1.0f - metal)) + (brdf_spec * metal) * 1.f;
        
        
        Color.a = 1.f;
    }
    else
    {
        Color.rgba = (0, 0, 0, 0);
    }
      
}

technique InsulatorMetal
{
    pass p0
    {
        vertexshader = compile vs_3_0 vs_main();
        pixelshader = compile ps_3_0 ps_main();
    }
}


