#define QUAD_PI		12.566370614359172
#define PI 3.141592
#define Epsilon 0.00001
#define Fdielectric 0.04

// 모든 물체에 대한 일정한 수직 입사 프레 넬 계수.

uniform sampler2D   albedo : register(s0);
uniform sampler2D   normals : register(s1);
uniform sampler2D   depth : register(s2);
uniform sampler2D   shadowMap : register(s3);
uniform samplerCUBE cubeShadowMap : register(s4);

uniform matrix matViewProjInv;
uniform matrix lightViewProj;

uniform float4 lightPos;
uniform float3 lightColor = { 1, 1, 1 };
uniform float  lightFlux = 10.0f; // lumen
uniform float  lightIlluminance = 1.5f; // lux
uniform float  lightRadius = 5.0f; // meter
uniform float  specularPower = 80.0f;

uniform float4 EyePosition;
uniform float2 pixelSize;
uniform float2 clipPlanes;
uniform float ao = 0.1f;
uniform float sinAngularRadius = 0.0046251;
uniform float cosAngularRadius = 0.9999893;

#define PCFCount 2

bool IsPoint;

float ShadowDepthMapHeight;
float ShadowDepthMapWidth;
float ShadowDepthBias = 1e-5f;


float shadowmin = 0.0f;

// 해야할것 
// 방향 매개변수 입력하기 .
// 라디언즈 입력하기 . 
float3 LightDirection;
float3 Lradiance;

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}


float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

// 디즈니
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Smith 의 기하학적 감쇠 함수 (근사치 ) 
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; //Epic 게임즈는  러프 니스 리매핑을 분석 조명에 사용할 것을 제안.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}


// Shlick의 프레넬 계수 (근사치)
float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, abs(5.0) );
}

float3 fresnelSchlickF3(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), abs(5.0) );
}

float DistributionGGX(float3  N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

// 반사 Image based lighting 환경 맵에 대한 밉맵 레벨 수 반환 .
//uint querySpecularTextureLevels()
//{
//    uint width, height, levels;
//    specularTexture.GetDimensions(0, width, height, levels);
//    return levels;
//}

void vs_main(
	in out float4 pos : POSITION,
	in out float2 tex : TEXCOORD0)
{
    pos.xy -= pixelSize.xy;
}

float ShadowVariance(float2 moments, float d)
{
    float mean = moments.x;
    float variance = max(moments.y - moments.x * moments.x, ShadowDepthBias);
    float md = mean - d;
    float chebychev = variance / (variance + md * md);

    chebychev = smoothstep(0.1f, 1.0f, chebychev);

	// NEVER forget that d > mean in Chebychev's inequality!!!
    return max(((d <= mean) ? 1.0f : 0.0f), chebychev);
}

float3 pbr_direction_imfpt(
// albm
in float3 albedo,
in float metalness,
// nrmr 
in float3 wnorm,
in float roughness,
// 언팩한 월드 
in float3 wpos)
{
    float3 N = normalize(wnorm);
    float3 V = normalize(EyePosition.xyz - wpos);
    
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo, metalness);
    
    float3 Lo = float3(0, 0, 0);
    
    float3 L = normalize(-LightDirection);
    float3 H = normalize(V + L);
   
  
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = fresnelSchlickF3(max(dot(H, V), 0.0), F0);

    float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - metalness;
        
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    float3 specular = numerator / max(denominator, 0.001);
    float NdotL = max(dot(N, L), 0.0);
    
    // 쉐도우 계산 
    float4 LightClipPosition = mul(float4(wpos.xyz, 1.f), lightViewProj);
    LightClipPosition.xyz = LightClipPosition.xyz / LightClipPosition.w;
    LightClipPosition.y *= -1.f;
    LightClipPosition.xy *= 0.5f;
    LightClipPosition.xy += 0.5f;
    
    float ShadowFactor = 1.0f + shadowmin;
    
    if (ShadowDepthMapHeight > 0)
    {
        float LookUpCount = (PCFCount * 2.0f + 1) * (PCFCount * 2.0f + 1);
        
        float Shadow = 0.0;
        float TexelSizeU = 1.0 / ShadowDepthMapWidth;
        float TexelSizeV = 1.0 / ShadowDepthMapHeight;
        for (int x = -PCFCount; x <= PCFCount; ++x)
        {
            for (int y = -PCFCount; y <= PCFCount; ++y)
            {
                float2 UVOffset = float2(x * TexelSizeU, y * TexelSizeV);
                
                float pcfDepth = tex2D(shadowMap, LightClipPosition.xy + UVOffset).x;
                if (LightClipPosition.z > (pcfDepth + ShadowDepthBias))
                {
                    Shadow += 1.0f;
                }
            }
        }
        Shadow /= LookUpCount;
        ShadowFactor -= Shadow;
    }
    ShadowFactor = saturate(ShadowFactor);
    
    Lo = (kD * albedo / PI + specular) * lightFlux * lightColor * NdotL * ShadowFactor + (lightColor *albedo * ao);
    
    return Lo;
}


float3 pbr_point(
// Albedo + Metal
in float3 Albedo,
in float Metalness,
// Normal + Roughness 
in float3 WorldNormal,
in float Roughness,
// 언팩한 월드 포지션
in float3 WorldPosition)
{
    float3 N = normalize(WorldNormal);
    float3 V = normalize(EyePosition.xyz - WorldPosition);
    
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, Albedo, Metalness);
    
    float3 Lo = float3(0, 0, 0);
    
    float3 L = normalize(lightPos.xyz - WorldPosition);
    // 하프벡터 
    float3 H = normalize(V + L);
    
    float Distance = length(lightPos.xyz - WorldPosition.xyz);
    // 감쇠 계수 계산
    float Attenuation = 1.0 / (Distance * Distance);
    float Radiance = lightColor * Attenuation;
    
    // 미세면 분산 함수 Ground Glass Unknown (Kenneth E.Torrance)
    // 거칠기에 의한 근사치로 강하기 조절 . 
    float  NDF = DistributionGGX(N, H, Roughness);
    float  G = GeometrySmith(N, V, L, Roughness);
    float3 F = fresnelSchlickF3(max(dot(H, V), 0.0), F0);

    float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - Metalness;
        
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    float3 specular = numerator / max(denominator, 0.001);
    float NdotL = max(dot(N, L), 0.0);
  
    float ShadowFactor = 1.f;

    // 그림자 적용 부분 검증    
    if (ShadowDepthMapHeight > 0)
    {
        if (Radiance < (clipPlanes.y - -clipPlanes.x)) 
        {
            float2 moments = texCUBE(cubeShadowMap, -L).xy;

            float Z = Radiance;
            float D = (Z  - clipPlanes.x) / (clipPlanes.y - clipPlanes.x);
            float Shadow  = ShadowVariance(moments, D);
            ShadowFactor = saturate(Shadow + shadowmin);
        }
    }
  
    float Radius_att = saturate((lightRadius - Distance) / lightRadius);
    
    Lo = (kD * Albedo / PI + specular) * lightColor * lightFlux * Radiance * Radius_att * NdotL * ShadowFactor;
    
    return Lo;
};

float3 pbr_direction(
// albm
in float3 Albedo,
in float Metalness,
// nrmr 
in float3 wnorm,
in float Roughness,
// 언팩한 월드 
in float3 wpos)
{
    // 픽셀 월드 -> 카메라 
    float3 Lo = normalize(EyePosition.xyz - wpos);
    float N = normalize(wnorm);    
    // 표면 법선 나가는 빛 방향 각도
    float CosLo = max(0.0, dot(N, Lo));
    // 스페큘러 반사벡터 
    float3 Lr = 2.0 * CosLo * N - Lo;
   
    float3 Li = -LightDirection;
    
    // Li Lo 하프벡터
    float3 Lh = normalize(Li + Lo);
    
    // 표면 법선과 다양한 빛 벡터사이의 각도를 계산 . 
    float CosLi = max(0.0, dot(N, Li));
    float CosLh = max(0.0, dot(N, Lh));
     // 수직 입사에서의 프레 넬 반사율 금속에 가까울 수록 알베도 색상 사용 !!.
    float3 F0 = lerp(Fdielectric, Albedo, Metalness);
    
    // 방향 조명에 대한 프레넬 항 계산 .
    float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
    
    // 반사 BRDF에 대한 정규 분포를 계산.
    float D = ndfGGX(CosLh, Roughness);

    // 반사 BRDF 기하 감쇠 계산 (Roughness에 의해 조절)
    float G = gaSchlickGGX(CosLi, CosLo, Roughness);
    
    // 매체에 의해 빛이 여러 번 굴절되어 확산 산란이 발생.
    // 메탈은 에너지를 반사하거나 흡수하므로 확산 기여도는 항상 0.
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), Metalness);
    
    // 램버트 확산 BRDF. 조명 및 재질 단위를 더 편리하게하기 위해 1 / PI로 확장하지 않습니다.(에픽게임즈의 기본 공식) 
	// 참조 : https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
    float3 DiffuseBRDF = kd * Albedo;
    
    // 쿡토렌스의 specular microfacet BRDF.
    float3 SpecularBRDF = (F * D * G) / max(Epsilon, 4.0 * CosLi * CosLo);
    
    // 쉐도우 계산 
    float4 LightClipPosition = mul(float4(wpos.xyz, 1.f), lightViewProj);
    LightClipPosition.xyz = LightClipPosition.xyz / LightClipPosition.w;
    LightClipPosition.y *= -1.f;
    LightClipPosition.xy *= 0.5f;
    LightClipPosition.xy += 0.5f;
    
    float ShadowFactor = 1.0f + shadowmin;
    
    if (ShadowDepthMapHeight > 0)
    {
        float LookUpCount = (PCFCount * 2.0f + 1) * (PCFCount * 2.0f + 1);
        
        float Shadow = 0.0;
        float TexelSizeU = 1.0 / ShadowDepthMapWidth;
        float TexelSizeV = 1.0 / ShadowDepthMapHeight;
        for (int x = -PCFCount; x <= PCFCount; ++x)
        {
            for (int y = -PCFCount; y <= PCFCount; ++y)
            {
                float2 UVOffset = float2(x * TexelSizeU, y * TexelSizeV);
                
                float pcfDepth = tex2D(shadowMap, LightClipPosition.xy + UVOffset).x;
                if (LightClipPosition.z > (pcfDepth + ShadowDepthBias))
                {
                    Shadow += 1.0f;
                }
            }
        }
        Shadow /= LookUpCount;
        ShadowFactor -= Shadow;
    }
    ShadowFactor = saturate(ShadowFactor);
    
    return ((DiffuseBRDF + SpecularBRDF) * Lradiance * CosLi) * ShadowFactor + ao * lightColor;
}


float3 Luminance_Blinn_Directional(float3 albedo,
float3 wpos, float3 wnorm)
{
	// the sun has an angular diameter between [0.526, 0.545] degrees
    float3 v = normalize(EyePosition.xyz - wpos);
    float3 n = normalize(wnorm);

	// closest point to disk (approximation)
    // float3 D = normalize(lightPos.xyz);
    float3 D = -normalize(LightDirection);
    float3 R = reflect(-v, n);

    float DdotR = dot(D, R);
    float r = sinAngularRadius;
    float d = cosAngularRadius;

    float3 S = R - DdotR * D;
    float3 L = ((DdotR < d) ? normalize(d * D + normalize(S) * r) : R);

	// BRDF (NOTE: should match values and exposure)
    float3 h = normalize(L + v);

    // float ndotl = saturate(dot(n, L));
    float ndoth = saturate(dot(n, h));

    float3 f_diffuse = albedo;
    float f_specular = pow(abs(ndoth), abs(specularPower) );
    
    float costheta = saturate(dot(n, D));
    float illuminance = lightIlluminance * costheta;

	// calculate shadow (assumes ortho projection)
    // float4 lspos = mul(float4(wpos, 1), lightViewProj);
	
    // d = lspos.z;

    // float2 ptex = (lspos.xy / lspos.w) * float2(0.5f, -0.5f) + 0.5f;
    // float2 moments = tex2D(shadowMap, ptex).rg;
    
    // 쉐도우 시작
    float4 LightClipPosition = mul(float4(wpos.xyz, 1.f), lightViewProj);
    LightClipPosition.xyz = LightClipPosition.xyz / LightClipPosition.w;
    LightClipPosition.y *= -1.f;
    LightClipPosition.xy *= 0.5f;
    LightClipPosition.xy += 0.5f;
    
   
    float ShadowFactor = 1.0f + shadowmin;
    
    if (ShadowDepthMapHeight > 0)
    {
        float LookUpCount = (PCFCount * 2.0f + 1) * (PCFCount * 2.0f + 1);
        
        float Shadow = 0.0;
        float TexelSizeU = 1.0 / ShadowDepthMapWidth;
        float TexelSizeV = 1.0 / ShadowDepthMapHeight;
        for (int x = -PCFCount; x <= PCFCount; ++x)
        {
            for (int y = -PCFCount; y <= PCFCount; ++y)
            {
                float2 UVOffset = float2(x * TexelSizeU, y * TexelSizeV);
                
                float pcfDepth = tex2D(shadowMap, LightClipPosition.xy + UVOffset).x;
                if (LightClipPosition.z > (pcfDepth + ShadowDepthBias))
                {
                    Shadow += 1.0f;
                }
            }
        }
        Shadow /= LookUpCount;
        ShadowFactor -= Shadow;
    }
    ShadowFactor = saturate(ShadowFactor);
    // 쉐도우 끝 
    
    return (f_diffuse + f_specular) * lightColor * illuminance * ShadowFactor;
}



float3 Luminance_Blinn_Point(float3 albedo, float3 wpos, float3 wnorm)
{
    float3 ldir = lightPos.xyz - wpos;

    float3 l = normalize(ldir);
    float3 v = normalize(EyePosition.xyz - wpos);
    float3 h = normalize(l + v);
    float3 n = normalize(wnorm);

    float dist2 = max(dot(ldir, ldir), 1e-4f);
    float ndotl = saturate(dot(n, l));
    float ndoth = saturate(dot(n, h));

    float3 f_diffuse = albedo;
    float f_specular = pow(abs(ndoth), abs(specularPower));
    
	// calculate shadow
    float shadow = 1.f;
    if (ShadowDepthMapHeight > 0)
    {
        float z = length(ldir);
        float d = (z - clipPlanes.x) / (clipPlanes.y - clipPlanes.x);
        
        {
            float2 moments = texCUBE(cubeShadowMap, -l).xy;
            shadow = saturate(ShadowVariance(moments, d) + shadowmin);
        }
    }
    // shadow end

    float illuminance = (lightFlux / (QUAD_PI * dist2)) * ndotl;
    float attenuation = max(0, 1 - sqrt(dist2) / lightRadius);

    return (f_diffuse + f_specular) * lightColor * illuminance * attenuation * (shadow);
}

void ps_deferred(
	in float2 tex : TEXCOORD0,
	out float4 color : COLOR0)
{
    // 감마 보정은 Device 세팅을 조절해서 결정 !
    // 알베도 + 메탈
    // 0.3 ^ (1 / 2.2) 감마 패킹 ( 높아짐 )
    // 0.3 ^ (2.2) 감마 언팩 ( 낮아짐 )  
    float4 albm = tex2D(albedo,  tex);
   float metal = saturate(pow(abs(albm.a) , abs(1.0 / 2.2)));
    
    // 월드 노말 + 거칠기 
    
    float4 nrmr = tex2D(normals, tex); 
    float3 wnorm = nrmr.xyz * 2.0f - 1.0f;
    float roughness = (nrmr.a);
    
    float  d = tex2D(depth, tex).r;
    float4 wpos = float4(tex.x * 2 - 1, 1 - 2 * tex.y, d, 1);

    if (d > 0.0f)
    {
        // 월드 위치 언팩 . 
        wpos = mul(wpos, matViewProjInv);
        wpos /= wpos.w;

        // 방향 조명
        if (IsPoint==false)
        {
            // directional light

            //color.rgb = Luminance_Blinn_Directional(
            //albm.rgb,
            //wpos.xyz,
            //wnorm);
            
            color.rgb =
            pbr_direction_imfpt(
            albm.rgb,
            metal,
            wnorm,
             roughness,
            wpos.xyz);
        }
        else 
        {
            
            // 점 조명
            // color.rgb =
            // Luminance_Blinn_Point(
            // albm.rgb,
            // wpos.xyz,
            // wnorm);
            
            color.rgb = pbr_point(albm.rgb, metal, wnorm, roughness, wpos.xyz);
            
        }
        
        color.a = 1;
    }
    else
    {
        color = float4(0, 0, 0, 0);
    }
}

technique deferred
{
    pass p0
    {
        vertexshader = compile vs_3_0 vs_main();
        pixelshader = compile ps_3_0 ps_deferred();
    }
}


