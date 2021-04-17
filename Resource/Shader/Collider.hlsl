float4x4 World;
float4x4 ViewProjection;
float4 DebugColor;

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

VsOut VsDebug(VsIn In)
{
    VsOut Out = (VsOut) 0;
    float4x4 WVP = mul(World, ViewProjection);
    Out.UV = In.UV;
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    
    return Out;
}

struct PsIn
{
    float2 UV : TEXCOORD0;
};

struct PsOut
{
    vector Color : COLOR0;
};

PsOut PsDebug(PsIn In)
{
    PsOut Out = (PsOut) 0;
    Out.Color = DebugColor;
  
    
    return Out;
};

technique DebugSK
{
    pass
    {
        alphablendenable = true;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;
        cullmode = none;
        fillmode = wireframe;
        vertexshader = compile vs_3_0 VsDebug();
        pixelshader = compile ps_3_0 PsDebug();
    }
}

