#define QUAD_PI		12.566370614359172
static const float PI = 3.141592;
static const float Epsilon = 0.00001;

// ��� ��ü�� ���� ������ ���� �Ի� ���� �� ���.
static const float3 Fdielectric = 0.04;

uniform sampler2D albedo : register(s0);
uniform sampler2D normals : register(s1);
uniform sampler2D depth : register(s2);
uniform sampler2D shadowMap : register(s3);
uniform samplerCUBE cubeShadowMap : register(s4);

uniform matrix matViewProjInv;
uniform matrix lightViewProj;

uniform float4 lightPos;
uniform float3 lightColor = { 1, 1, 1 };
uniform float lightFlux = 10.0f; // lumen
uniform float lightIlluminance = 1.5f; // lux
uniform float lightRadius = 5.0f; // meter
uniform float specularPower = 80.0f;

uniform float4 eyePos;
uniform float2 pixelSize;
uniform float2 clipPlanes;

uniform float sinAngularRadius = 0.0046251;
uniform float cosAngularRadius = 0.9999893;

#define PCFCount 3

bool IsPoint;

float ShadowDepthMapHeight;
float ShadowDepthMapWidth;
float ShadowDepthBias;

float shadowmin = 0.0f;

// �ؾ��Ұ� 
// ���� �Ű����� �Է��ϱ� .
// ������ �Է��ϱ� . 
float3 LightDirection;
float3 Lradiance;



// �����
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

// Smith �� �������� ���� �Լ� (�ٻ�ġ ) 
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; //Epic �������  ���� �Ͻ� �������� �м� ���� ����� ���� ����.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}


// Shlick�� ������ ��� (�ٻ�ġ)
float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// �ݻ� Image based lighting ȯ�� �ʿ� ���� �Ӹ� ���� �� ��ȯ .
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
    float variance = max(moments.y - moments.x * moments.x, 1e-5f);
    float md = mean - d;
    float chebychev = variance / (variance + md * md);

    chebychev = smoothstep(0.1f, 1.0f, chebychev);

	// NEVER forget that d > mean in Chebychev's inequality!!!
    return max(((d <= mean) ? 1.0f : 0.0f), chebychev);
}

float3 pbr_direction(
// albm
in float3 albedo,
in float metalness,
// nrmr 
in float3 wnorm,
in float roughness,
// ������ ���� 
in float3 wpos)
{
    // �ȼ� ���� -> ī�޶� 
    float3 Lo = normalize(eyePos.xyz - wpos);
    // ���� �븻
    float N = normalize(wnorm);
    
    // ǥ�� ���� ������ �� ���� ����
    float cosLo = max(0.0, dot(N, Lo));
    
    // ����ŧ�� �ݻ纤�� 
    float3 Lr = 2.0 * cosLo * N - Lo;
    
    // ���� �Ի翡���� ���� �� �ݻ��� (�ݼ��� ��� �˺��� ���� ���).
    float3 F0 = lerp(Fdielectric, albedo, metalness);
    
    // ���� ���� ��� ���� ...
    float3 Li = -LightDirection;
    
    // Li Lo ��������
    float3 Lh = normalize(Li + Lo);
    
    // ǥ�� ������ �پ��� �� ���ͻ����� ������ ��� . 
    float cosLi = max(0.0, dot(N, Li));
    float cosLh = max(0.0, dot(N, Lh));
    
    // ���̷�Ʈ ���� ���� ������ �� ��� .
    float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
    
    // �ݻ� BRDF�� ���� ���� ������ ���.
    float D = ndfGGX(cosLh, roughness);

    // �ݻ� BRDF ���� ���� ��� .
    float G = gaSchlickGGX(cosLi, cosLo, roughness);
    
    // ��ü�� ���� ���� ���� �� �����Ǿ� Ȯ�� ����� �߻�.
    // ��Ż�� �������� �ݻ��ϰų� ����ϹǷ� Ȯ�� �⿩���� �׻� 0.
    // �������� �����Ϸ��� ���� �� ��� �� �ݼӼ��� ���� Ȯ�� BRDF �⿩���� �����ؾ���.
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);

    // ����Ʈ Ȯ�� BRDF.
    // ���� �� ���� ������ �� ���ϰ��ϱ� ���� 1 / PI�� Ȯ������ �ʽ��ϴ�. (���Ȱ���� �⺻ ������ ������ ) 
	// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
    float3 diffuseBRDF = kd * albedo;
    
    // ���䷻�� specular microfacet BRDF.
    float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);
    
    
    // ������ ��� 
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
    
    return (diffuseBRDF + specularBRDF) * lightColor * Lradiance * cosLi * ShadowFactor;
}


float3 Luminance_Blinn_Directional(float3 albedo,
float3 wpos, float3 wnorm)
{
	// the sun has an angular diameter between [0.526, 0.545] degrees
    float3 v = normalize(eyePos.xyz - wpos);
    float3 n = normalize(wnorm);

	// closest point to disk (approximation)
    float3 D = normalize(lightPos.xyz);
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
    float f_specular = pow(ndoth, specularPower);
    
    float costheta = saturate(dot(n, D));
    float illuminance = lightIlluminance * costheta;

	// calculate shadow (assumes ortho projection)
    // float4 lspos = mul(float4(wpos, 1), lightViewProj);
	
    // d = lspos.z;

    // float2 ptex = (lspos.xy / lspos.w) * float2(0.5f, -0.5f) + 0.5f;
    // float2 moments = tex2D(shadowMap, ptex).rg;
    
    // ������ ����
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
    // ������ �� 
    
    return (f_diffuse + f_specular) *
        lightColor * illuminance * ShadowFactor;
}

float3 Luminance_Blinn_Point(float3 albedo, float3 wpos, float3 wnorm)
{
    float3 ldir = lightPos.xyz - wpos;

    float3 l = normalize(ldir);
    float3 v = normalize(eyePos.xyz - wpos);
    float3 h = normalize(l + v);
    float3 n = normalize(wnorm);

    float dist2 = max(dot(ldir, ldir), 1e-4f);
    float ndotl = saturate(dot(n, l));
    float ndoth = saturate(dot(n, h));

    float3 f_diffuse = albedo;
    float f_specular = pow(ndoth, specularPower);
    
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

    float illuminance = (lightFlux / (QUAD_PI * dist2)) * ndotl;
    float attenuation = max(0, 1 - sqrt(dist2) / lightRadius);

    return (f_diffuse + f_specular) * lightColor * illuminance * attenuation * (shadow);
}

void ps_deferred(
	in float2 tex : TEXCOORD0,
	out float4 color : COLOR0)
{
    // ���� ������ Device ������ �����ؼ� ���� !
    // �˺��� + ��Ż
    float4 albm = tex2D(albedo,  tex);
    // ���� �븻 + ��ĥ�� 
    float4 nrmr = tex2D(normals, tex); 
    float3 wnorm = nrmr.xyz * 2.0f - 1.0f;
    float  d = tex2D(depth, tex).r;
    float4 wpos = float4(tex.x * 2 - 1, 1 - 2 * tex.y, d, 1);

    if (d > 0.0f)
    {
        // ���� ��ġ ���� . 
        wpos = mul(wpos, matViewProjInv);
        wpos /= wpos.w;

        // ���� ����
        if (IsPoint==false)
        {
            // directional light

            color.rgb = Luminance_Blinn_Directional(
            albm.rgb,
            wpos.xyz,
            wnorm);
            
            //color.rgb = 
            //pbr_direction(
            //albm.rgb,
            //albm.a,
            //wnorm,
            //nrmr.a,
            //wpos.xyz);
        }
        else 
        {
			// �� ����
            color.rgb = 
            Luminance_Blinn_Point(
            albm.rgb, 
            wpos.xyz, 
            wnorm);
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

