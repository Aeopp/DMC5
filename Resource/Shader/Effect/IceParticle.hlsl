matrix matWorld;
matrix ViewProjection;

uniform float exposure_corr;

uniform float ColorIntencity;
uniform float EmissiveIntencity;
uniform float AlphaFactor = 1.f;
uniform float SpecularPow;

uniform float3 LightDirection;
uniform float3 EyePos;

texture AlbedoMap;
sampler Albedo = sampler_state
{
    texture = AlbedoMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = true;
};

texture EmssiveMskMap;
sampler Emissive = sampler_state
{
    texture = EmssiveMskMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = false;
};

texture NrmrMap;
sampler Nrmr = sampler_state
{
    texture = NrmrMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    AddressU = wrap;
    AddressV = wrap;
    sRGBTexture = false;
};

void VsMain(in out float4 Position : POSITION0,
            in out float2 UV0 : TEXCOORD0,
            in float3 norm : NORMAL,
	        in float3 tang : TANGENT,
	        in float3 bin : BINORMAL,
            
            out float3 wnorm : TEXCOORD1,
	        out float3 wtan : TEXCOORD2,
	        out float3 wbin : TEXCOORD3,
            out float3 viewdir : TEXCOORD4)
{
    Position = mul(Position, matWorld);
    viewdir = Position.xyz - EyePos;
    Position = mul(Position, ViewProjection);
    
    wnorm = mul(float4(norm, 0), matWorld).xyz;
    wtan = mul(float4(tang, 0), matWorld).xyz;
    wbin = mul(float4(bin, 0), matWorld).xyz;
    
};

void PsMain(out float4 Color : COLOR0,

            in float2 UV0 : TEXCOORD0,
            in float3 wnorm : TEXCOORD1,
	        in float3 wtan : TEXCOORD2,
	        in float3 wbin : TEXCOORD3,
            in float3 viewdir : TEXCOORD4 )
{
    // 파티클 노말매핑 ... 
    float3x3 tbn =
                float3x3(normalize(float3(wtan)),
                         normalize(float3(wbin)),
                         normalize(float3(wnorm)));
    tbn = transpose(tbn);
    float4 normap_sample = tex2D(Nrmr, UV0);
    float3 tnorm = normap_sample.xyz * 2.0f - 1.0f;
    float3 N = normalize(mul(tbn, tnorm));
    // ...............
    
    
    Color = tex2D(Albedo, UV0);
    float Diffuse = saturate(dot(-LightDirection, N));
    
    float3 _reflect = normalize(reflect(LightDirection, N));
    float3 specular = saturate(dot(-viewdir, _reflect));
    specular = pow(specular, SpecularPow);
    
    Color.rgb *= Diffuse;
    Color.rgb += specular;
    Color.rgb *= ColorIntencity;
    float4 EmissiveSample = tex2D(Emissive, UV0);
    Color.rgb += (EmissiveSample.rgb *= EmissiveIntencity);
    Color.rgb *= exposure_corr;
    Color.a *= AlphaFactor;
    
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
        zwriteenable = false;
        sRGBWRITEENABLE = false;
        cullmode = ccw;
        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};


