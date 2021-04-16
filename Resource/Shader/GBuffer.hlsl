uniform sampler2D ALBM : register(s0);
uniform sampler2D NRMR : register(s1);

uniform matrix World;
uniform matrix ViewProjection;
uniform matrix PrevWorldViewProjection;

uniform bool bVelocityRecord = false;
uniform float2 UVScale = { 1, 1 };

struct VsIn
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
    float2 UV : TEXCOORD0;
};

struct VsOut
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
    float2 UV : TEXCOORD0;
    float2 ZW : TEXCOORD1;
    float2 Velocity : TEXCOORD2;
};

VsOut VsGBuffer(VsIn In)
{
    VsOut Out = (VsOut) 0;
    
    float4x4 WVP = mul(World, ViewProjection);
    Out.UV = In.UV * UVScale;
    Out.Normal = mul(float4(In.Normal.xyz, 0.f), World);
    Out.Tangent = mul(float4(In.Tangent.xyz, 0.f), World);
    Out.BiNormal = mul(float4(In.BiNormal.xyz, 0.f), World);
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    Out.ZW = Out.Position.zw;
    
    
    return Out;
}

struct PsIn
{
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
    float2 UV : TEXCOORD0;
    float2 ZW : TEXCOORD1;
    float2 Velocity : TEXCOORD2;
};

struct PsOut
{
    vector Albm :  COLOR0;  // Albedo , Metalness
    vector Nrmr :  COLOR1;  // Normal xy , Unknown , Roughness
    vector Depth : COLOR2;  // Depth , 채널 X
};

PsOut PsGBuffer(PsIn In)
{
    PsOut Out = (PsOut)0;
    
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));
    
    float4 sampleNrmr = tex2D(ALBM, In.UV);
    float2 NormalXY   = sampleNrmr.xy * 2.0 - 1.0f;
    const float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
    
    float3 PackNormal=normalize(mul(
                normalize(float3(NormalXY, NormalZ)), TBN));
    
    Out.Albm = tex2D(ALBM, In.UV);
    Out.Nrmr = float4(PackNormal.xyz *0.5f+0.5f,sampleNrmr.w);
    /*원근나누기*/
    Out.Depth = float4(In.ZW.x / In.ZW.y,0 ,0 ,0 );
    
    return Out;
};

technique GBuffer
{
    pass
    {
        vertexshader = compile vs_3_0 VsGBuffer();
        pixelshader = compile ps_3_0 PsGBuffer();
    }
}

