matrix Ortho;
matrix Perspective;  // 메시, xy 회전이 필요한 경우 
matrix ScreenMat;    // (-width/2 ~ +width/2, +height/2 ~ -height/2)

float3 LightDirection = float3(0.f, 0.f, 1.f);

float _TotalAccumulateTime;
float _AccumulationTexU;
float _AccumulationTexV;
float _SliceAmount = 0.f;

float _HP_Degree = 0.f; // 0 ~ 360 범위
float2 _HP_StartPt;
float2 _HP_Normal0;
float2 _HP_Normal1;

float _HPGaugeCurXPosOrtho;
float _BossGaugeCurXPosOrtho;
float _TDTGaugeCurXPosOrtho;
float _RankGaugeCurYPosOrtho;

float _HPGlassDirt = 0.f;

float2 _MinTexUV = float2(0.f, 0.f);
float2 _MaxTexUV = float2(1.f, 1.f);

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

texture ALB0Map;
sampler ALB0 = sampler_state
{
    texture = ALB0Map;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    sRGBTexture = true;
    MaxAnisotropy = 4;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture ALB1Map;
sampler ALB1 = sampler_state
{
    texture = ALB1Map;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    sRGBTexture = true;
    MaxAnisotropy = 4;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture NRMR0Map;
sampler NRMR0 = sampler_state
{
    texture = NRMR0Map;
    minfilter = point;
    magfilter = point;
    mipfilter = point;
    sRGBTexture = false;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture ATOS0Map;
sampler ATOS0 = sampler_state
{
    texture = ATOS0Map;
    minfilter = point;
    magfilter = point;
    mipfilter = point;
    sRGBTexture = false;
    AddressU = Wrap;
    AddressV = Wrap;
};


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
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
};

struct VsOut_Clip
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
    float4 Clip : TEXCOORD4;
};

struct VsOut_NoiseClip
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
    float2 NoiseCoord0 : TEXCOORD4;
    float2 NoiseCoord1 : TEXCOORD5;
    float2 NoiseCoord2 : TEXCOORD6;
    float Clip : TEXCOORD7;
};

struct VsOut_GUI
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
};


VsOut VsMain(VsIn In)
{
    VsOut Out = (VsOut) 0;

    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Ortho);
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.f), ScreenMat));
    Out.Tangent = normalize(mul(float4(In.BiNormal.xyz, 0.f), ScreenMat));
    Out.BiNormal = normalize(mul(float4(In.Position.xyz, 0.f), ScreenMat));
    Out.UV = In.UV;
    
    return Out;
};

VsOut VsMain_Perspective(VsIn In)
{
    VsOut Out = (VsOut) 0;

    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Perspective);
    Out.Position.xyz /= Out.Position.w;
    
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.f), ScreenMat));
    Out.Tangent = normalize(mul(float4(In.BiNormal.xyz, 0.f), ScreenMat));
    Out.BiNormal = normalize(mul(float4(In.Position.xyz, 0.f), ScreenMat));
    Out.UV = In.UV;
    
    return Out;
};

VsOut_NoiseClip VsMain_TargetHP(VsIn In)
{
    VsOut_NoiseClip Out = (VsOut_NoiseClip) 0;

    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Ortho);
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.f), ScreenMat));
    Out.Tangent = normalize(mul(float4(In.BiNormal.xyz, 0.f), ScreenMat));
    Out.BiNormal = normalize(mul(float4(In.Position.xyz, 0.f), ScreenMat));
    Out.UV = In.UV;
       
    // noise
    Out.NoiseCoord0 = Out.UV;
    Out.NoiseCoord0.y += _TotalAccumulateTime * 0.2f; // scrollspeed;
    Out.NoiseCoord1 = Out.UV;
    Out.NoiseCoord1.y += _TotalAccumulateTime * 0.1f; // scrollspeed;
    Out.NoiseCoord2 = Out.UV;
    Out.NoiseCoord2.y += _TotalAccumulateTime * 0.3f; // scrollspeed;
    
    // clip
    float2 EndPtVec = Out.Position.xy - _HP_StartPt;
    float Result = dot(normalize(EndPtVec), _HP_Normal0);
    float Result2 = dot(normalize(EndPtVec), _HP_Normal1);
    
    if (_HP_Degree < 180.f)
    {
        if (Result < 0.f && Result2 > 0.f)
            Out.Clip = -1.f;
        else
            Out.Clip = 1.f;
    }
    else
    {
        if (Result > 0.f && Result2 < 0.f)
            Out.Clip = 1.f;
        else
            Out.Clip = -1.f;
    }
    
     return Out;
};

VsOut_Clip VsMain_ClipPos(VsIn In)
{
    VsOut_Clip Out = (VsOut_Clip) 0;

    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Ortho);
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.f), ScreenMat));
    Out.Tangent = normalize(mul(float4(In.BiNormal.xyz, 0.f), ScreenMat));
    Out.BiNormal = normalize(mul(float4(In.Position.xyz, 0.f), ScreenMat));
    Out.UV = In.UV;        
    Out.Clip = Out.Position;

    return Out;
};

VsOut_GUI VsMain_GUI(VsIn In)
{
    VsOut_GUI Out = (VsOut_GUI) 0;
    
    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Ortho);
    Out.UV.x = (_MaxTexUV.x - _MinTexUV.x) * In.UV.x + _MinTexUV.x;
    Out.UV.y = (_MaxTexUV.y - _MinTexUV.y) * In.UV.y + _MinTexUV.y;

    return Out;
}

VsOut_Clip VsMain_Rank(VsIn In)
{
    VsOut_Clip Out = (VsOut_Clip) 0;

    Out.Position = mul(float4(In.Position.xyz, 1.f), ScreenMat);
    Out.Position = mul(float4(Out.Position.xyz, 1.f), Perspective);
    Out.Position.xyz /= Out.Position.w;
    
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.f), ScreenMat));
    Out.Tangent = normalize(mul(float4(In.BiNormal.xyz, 0.f), ScreenMat));
    Out.BiNormal = normalize(mul(float4(In.Position.xyz, 0.f), ScreenMat));
    Out.UV = In.UV;
    Out.Clip = Out.Position;
    
    return Out;
};


struct PsIn
{
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
};

struct PsIn_Clip
{
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
    float4 Clip : TEXCOORD4;
};

struct PsIn_NoiseClip
{
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
    float2 NoiseCoord0 : TEXCOORD4;
    float2 NoiseCoord1 : TEXCOORD5;
    float2 NoiseCoord2 : TEXCOORD6;
    float2 Clip : TEXCOORD7;
};

struct PsIn_GUI
{
    float2 UV : TEXCOORD0;
};

struct PsOut
{
    float4 Color : COLOR0;
};


PsOut PsMain_Plane(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float4 ALB0Sample = tex2D(ALB0, In.UV);
    float4 ATOSSample = tex2D(ATOS0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));

    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    
    float Diffuse = saturate(dot(WorldNormal, -normalize(LightDirection)));
    float Ambient = ATOSSample.b * 0.1f;

    Out.Color = (Diffuse + Ambient) * float4(ALB0Sample.rgb, 1.f);
    Out.Color.a = ATOSSample.r;

    return Out;
};

PsOut PsMain_Mesh(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float4 ALB0Sample = tex2D(ALB0, In.UV);
    //float4 ATOSSample = tex2D(ATOS0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));
    
    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    
    float Diffuse = saturate(dot(WorldNormal, -normalize(LightDirection)));

    Out.Color = Diffuse * float4(ALB0Sample.rgb, 1.f);
    Out.Color.a = 1.f;
    
    return Out;
};

PsOut PsMain_Mesh_Dissolve(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float4 NoiseSample = tex2D(Noise, In.UV).bbbb; // r,g,b 각각 다른 노이즈. 그 중 하나만 사용
    NoiseSample.rgb -= _SliceAmount;
    clip(NoiseSample);
    
    
    float4 ALB0Sample = tex2D(ALB0, In.UV);
    //float4 ATOSSample = tex2D(ATOS0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));
    
    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    
    float Diffuse = saturate(dot(WorldNormal, -normalize(LightDirection)));

    Out.Color = Diffuse * float4(ALB0Sample.rgb, 1.f);
    Out.Color.a = 1.f;
    
    return Out;
};


PsOut PsMain_TargetCursor(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float2 newUV = In.UV;
    newUV.y -= _AccumulationTexV;
    
    float Alp = tex2D(ATOS0, In.UV).r * tex2D(Noise, newUV);
    
    Out.Color = float4(1.f, 0.4f, 0.4f, Alp);

    return Out;
};

PsOut PsMain_TargetHP(PsIn_NoiseClip In)
{
    PsOut Out = (PsOut) 0;
    
    clip(In.UV.y - 0.8f);
    clip(In.Clip);
    
    float4 Noise0 = tex2D(Noise, In.NoiseCoord0).rrrr;
    float4 Noise1 = tex2D(Noise, In.NoiseCoord1).gggg;
    float4 Noise2 = tex2D(Noise, In.NoiseCoord2).bbbb;
    
    // -1 ~ 1 범위
    Noise0 = (Noise0 - 0.5f) * 2.f;
    Noise1 = (Noise1 - 0.5f) * 2.f;
    Noise2 = (Noise2 - 0.5f) * 2.f;
 
    // distortion
    Noise0.xy *= float2(0.3f, 0.1f);
    Noise1.xy *= float2(-0.1f, 0.2f);
    Noise2.xy *= float2(0.2f, -0.3f);
    
    float4 FinalNoise = Noise0 + Noise1 + Noise2;
    FinalNoise *= 0.5f; // scale;
    float2 NoiseCoord = In.UV + FinalNoise.xy;  // 원본 uv를 밀어냄
       
    Out.Color = float4(0.5f, 0.647f, 0.698f, tex2D(ATOS0, NoiseCoord).r);
    Out.Color.a *= ((In.UV.y - 0.8f) * 5.f);
    
    return Out;
};

PsOut PsMain_BossGauge0(PsIn_Clip In)
{
    PsOut Out = (PsOut) 0;
   
    clip(-In.Clip.y - 0.5f); // 게이지 평면 위쪽에 알파가 남아있는 부분이 있어서 강제로 짜름
    
    float4 ATOSSample = tex2D(ATOS0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));

    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    
    float Shade = saturate(dot(WorldNormal, -normalize(LightDirection))) + 0.2f; // Diffuse + Ambient
    
    Out.Color.rgb = Shade * float3(0.416f, 0.525f, 0.65f);
    Out.Color.a = ATOSSample.a;
    
    return Out;
};

PsOut PsMain_BossGauge1(PsIn_Clip In)
{
    PsOut Out = (PsOut) 0;
    
    clip(-In.Clip.y - 0.5f);
    
    float4 ATOSSample = tex2D(ATOS0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));

    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    
    float Shade = saturate(dot(WorldNormal, -normalize(LightDirection))) + 0.2f; // Diffuse + Ambient
    
    Out.Color.rgb = Shade * float3(0.282f, 0.391f, 0.588f);
    Out.Color.a = ATOSSample.g;
    
    return Out;
};

PsOut PsMain_BossGauge2(PsIn_Clip In)
{
    PsOut Out = (PsOut) 0;
    
    clip(-In.Clip.y - 0.5f);
    clip(_BossGaugeCurXPosOrtho - In.Clip.x);
    
    float4 ATOSSample = tex2D(ATOS0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));

    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    
    float Shade = saturate(dot(WorldNormal, -normalize(LightDirection))) + 0.2f; // Diffuse + Ambient
    
    Out.Color.rgb = Shade * float3(0.263f, 0.227f, 0.733f); // 위
    Out.Color.a = ATOSSample.b;
    
    //Out.Color = Diffuse * float4(0.176f, 0.149f, 0.255f, ATOSSample.b); // 바닥
    
    return Out;
};

PsOut PsMain_BossGauge3(PsIn_Clip In)
{
    PsOut Out = (PsOut) 0;
    
    clip(-In.Clip.y - 0.5f);
    clip(_BossGaugeCurXPosOrtho - In.Clip.x);
    
    float4 ATOSSample = tex2D(ATOS0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));

    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    
    float Shade = saturate(dot(WorldNormal, -normalize(LightDirection))) + 0.2f; // Diffuse + Ambient
    
    Out.Color.rgb = Shade * float3(0.627f, 0.674f, 0.984f);
    Out.Color.a = ATOSSample.r;
    
    return Out;
};

PsOut PsMain_Glass(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float4 ALB0Sample = tex2D(ALB0, In.UV); // glass
    float4 ALB1Sample = tex2D(ALB1, In.UV); // blood
    float4 ATOSSample = tex2D(ATOS0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));

    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
   
    float Shade = saturate(dot(WorldNormal, -normalize(LightDirection))) + 0.2f;    // Diffuse + Ambient

    float Dirt = ATOSSample.g * _HPGlassDirt;

    Out.Color = float4(Shade * ALB0Sample.rgb, saturate(0.1f + ATOSSample.b));

    if (0.f < Dirt)
        Out.Color = (Out.Color * (1.f - Dirt)) + float4(Shade * ALB1Sample.rgb, Dirt);

    return Out;
};

PsOut PsMain_HPGauge(PsIn_Clip In)
{
    PsOut Out = (PsOut) 0;
    
    clip(_HPGaugeCurXPosOrtho - In.Clip.x);
    
    float4 ALB0Sample = tex2D(ALB0, In.UV);
    float4 ATOSSample = tex2D(ATOS0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));

    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    
    float Shade = saturate(dot(WorldNormal, -normalize(LightDirection))) + 0.2f;
    
    Out.Color.rgb = Shade * (ALB0Sample.rgb * float3(0.027f, 0.78f, 0.478f));
    Out.Color.a = ATOSSample.r;
    
    return Out;
};

PsOut PsMain_TDTGauge0(PsIn In)
{
    PsOut Out = (PsOut) 0;

    float4 ATOSSample = tex2D(ATOS0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));

    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
   
    float Shade = saturate(dot(WorldNormal, -normalize(LightDirection))) + 0.2f;
    
    float2 newUV = In.UV;
    newUV.y += _AccumulationTexV;
 
    Out.Color.rgb = Shade * (float3(0.149f, 0.145f, 0.208f) - 0.15f * float3(NRMRSample.aaa));
    Out.Color.a = (ATOSSample.r + ATOSSample.b) * (1.f - tex2D(ATOS0, newUV).a * 0.8f);
    
    return Out;
};

PsOut PsMain_TDTGauge1(PsIn_Clip In)
{
    PsOut Out = (PsOut) 0;

    clip(_TDTGaugeCurXPosOrtho - In.Clip.x);
    
    float4 ATOSSample = tex2D(ATOS0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));

    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
   
    float Shade = saturate(dot(WorldNormal, -normalize(LightDirection))) + 0.2f;
    
    float2 newUV = In.UV;
    newUV.x += _AccumulationTexU;

    Out.Color.rgb = Shade * (float3(0.478f, 0.074f, 0.028f) - 0.15f * tex2D(ATOS0, newUV).aaa);
    Out.Color.a = ATOSSample.b;
    
    return Out;
};

PsOut PsMain_GUI(PsIn_GUI In)
{
    PsOut Out = (PsOut) 0;
    
    Out.Color = tex2D(ALB0, In.UV);

    return Out;
};

PsOut PsMain_GUI_Dissolve(PsIn_GUI In)
{
    PsOut Out = (PsOut) 0;

    float4 NoiseSample = tex2D(Noise, In.UV).rrrr; // r,g,b 각각 다른 노이즈. 그 중 하나만 사용
    NoiseSample.rgb -= _SliceAmount;
    clip(NoiseSample);
    
    Out.Color = tex2D(ALB0, In.UV);

    return Out;
};

PsOut PsMain_RankBack(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float4 Noise0 = tex2D(Noise, In.UV).rrrr;
    float4 Noise1 = tex2D(Noise, In.UV).gggg;
    float4 Noise2 = tex2D(Noise, In.UV).bbbb;
    
    // -1 ~ 1 범위
    Noise0 = (Noise0 - 0.5f) * 2.f;
    Noise1 = (Noise1 - 0.5f) * 2.f;
    Noise2 = (Noise2 - 0.5f) * 2.f;
 
    // distortion
    Noise0.xy *= float2(0.2f, 0.1f);
    Noise1.xy *= float2(-0.1f, 0.2f);
    Noise2.xy *= float2(0.2f, -0.1f);
    
    float4 FinalNoise = Noise0 + Noise1 + Noise2;
    FinalNoise *= 0.2f; // scale;
    float2 NoiseCoord = In.UV + FinalNoise.xy; // 원본 uv를 밀어냄
    
    float4 ATOSSample = tex2D(ATOS0, NoiseCoord);

    Out.Color = float4(1.f, 1.f, 1.f, saturate((ATOSSample.a - 0.5f) * 1.2f));
 
    return Out;
};

PsOut PsMain_Rank(PsIn_Clip In)
{
    PsOut Out = (PsOut) 0;
    
    float4 NoiseSample = tex2D(Noise, In.UV).rrrr; // r,g,b 각각 다른 노이즈. 그 중 하나만 사용
    NoiseSample.rgb -= _SliceAmount;
    clip(NoiseSample);
    
    float4 ALB0Sample = tex2D(ALB0, In.UV);
    //float4 ATOSSample = tex2D(ATOS0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));
    
    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    
    float Diffuse = saturate(dot(WorldNormal, -normalize(LightDirection))) * 0.5f;

    float3 Albedo = ALB0Sample.rgb;
    if (In.Clip.y < _RankGaugeCurYPosOrtho)
        Albedo = tex2D(ALB1, tex2D(Noise, (In.UV - float2(_AccumulationTexU, 0.f))).xy).rgb * 1.2f;
    
    Out.Color = Diffuse * float4(Albedo, 1.f);
    Out.Color.a = 1.f;

    return Out;
};


technique Default
{
	pass p0
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain_Plane();
    }
    pass p1
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain_TargetCursor();
    }
    pass p2
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain_TargetHP();
        pixelshader = compile ps_3_0 PsMain_TargetHP();
    }
    pass p3
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain_ClipPos();
        pixelshader = compile ps_3_0 PsMain_BossGauge0();
    }
    pass p4
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain_ClipPos();
        pixelshader = compile ps_3_0 PsMain_BossGauge1();
    }
    pass p5
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain_ClipPos();
        pixelshader = compile ps_3_0 PsMain_BossGauge2();
    }
    pass p6
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain_ClipPos();
        pixelshader = compile ps_3_0 PsMain_BossGauge3();
    }
    pass p7
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain_Glass();
    }
    pass p8
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain_Perspective();
        pixelshader = compile ps_3_0 PsMain_Mesh();
    }
    pass p9
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain_ClipPos();
        pixelshader = compile ps_3_0 PsMain_HPGauge();
    }
    pass p10
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain_TDTGauge0();
    }
    pass p11
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain_ClipPos();
        pixelshader = compile ps_3_0 PsMain_TDTGauge1();
    }
    pass p12
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain_GUI();
        pixelshader = compile ps_3_0 PsMain_GUI();
    }
    pass p13
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain_RankBack();
    }
    pass p14
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain_Perspective();
        pixelshader = compile ps_3_0 PsMain_Mesh_Dissolve();
    }
    pass p15
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;
        
        vertexshader = compile vs_3_0 VsMain_Rank();
        pixelshader = compile ps_3_0 PsMain_Rank();
    }
    pass p16
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain_GUI();
        pixelshader = compile ps_3_0 PsMain_GUI_Dissolve();
    }
};