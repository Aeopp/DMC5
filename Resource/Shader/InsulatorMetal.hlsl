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
#define Fdielectric 0.04

// Shlick's approximation of the Fresnel factor.
float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


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
   
        // 월드 노말 . 
        float3 albedo = albm.rgb;
        // 월드 위치 언패킹
        float4 wpos = float4(tex.x * 2 - 1, 1 - 2 * tex.y, depth, 1);
        wpos = mul(wpos, matViewProjInv);
        wpos /= wpos.w;
        float metal = saturate(pow(albm.a, abs(1.0 / 2.2)));
        float roughness = saturate(nrmr.a);
    
        float3 v       = normalize(eyePos.xyz - wpos.xyz);
        float3 n       = normalize(nrmr.xyz * 2.0f - 1.0f);
        float3 r = normalize(reflect(-v, n));
    
        float ndotv    = max(dot(n, v), 0.0);
        float miplevel = roughness * (NUM_MIPS - 1);
    
        // 반사된 방향으로 디퓨즈 스페큘러 조도 구함
        float3 diff_irrad = texCUBE(irradiance1, r).rgb;
        float3 spec_irrad = texCUBElod(irradiance2, float4(r, miplevel)).rgb;
    
        // 룩업 테이블 ...  
        // 노말과 시선과의 차이가 작을수록 u 는 0 에 가까움
    
        // 거칠기가 강한 물체일수록 
        // v 가 강해짐 
        float2 f0_scale_bias = tex2D(brdfLUT, float2(ndotv, roughness)).rg;
    
        // 지연 쉐이딩 값으로 교체하기 바람 ..
        float light_diffuse = 1.f;
        float light_specular = 1.f;
        
        // 조도 * 알베도 *  PI ;
        float3 brdf_diff = diff_irrad * albedo.rgb * ONE_OVER_PI * light_diffuse;
        float3 brdf_spec = spec_irrad * ( 0.04*f0_scale_bias.x + f0_scale_bias.y) * light_specular;
        
        Color.rgb = (brdf_diff) + (brdf_spec) * 1.f;
        
        
        
        
  //      // pbr ibl 시작  
        
		//// Calculate Fresnel term for ambient lighting.
		//// Since we use pre-filtered cubemap(s) and irradiance is coming from many directions
		//// use cosLo instead of angle with light's half-vector (cosLh above).
		//// See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
  //      float3 F0 = lerp(Fdielectric, albedo, metal);
  //      float3 Lo = normalize(eyePos - wpos);
  //      float cosLo = max(0.0, dot(n, Lo));
  //      float3 F = fresnelSchlick(F0, cosLo);

		//// Get diffuse contribution factor (as with direct lighting).
  //      float3 kd = lerp(1.0 - F, 0.0, metal);

		//// Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
  //      float3 diffuseIBL = kd * albedo * diff_irrad;
        
		//// Total specular IBL contribution.
  //      float3 specularIBL = (F0 * f0_scale_bias.x + f0_scale_bias.y) * spec_irrad;

		//// Total ambient lighting contribution.
  //      Color.rgb = diffuseIBL + specularIBL; 
        // 끝
        
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


