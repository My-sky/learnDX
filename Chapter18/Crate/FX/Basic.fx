
#include "lightingHelper.fx"

cbuffer cbPerFrame
{
	DirectionalLight gDirLights[3];
	float3 gEyePosW;

	float gFogStart;
	float gFogRange;
	float4 gFogColor;

	float gMinTessDistance;
	float gMaxTessDistance;
	float gMinTessFactor;
	float gMaxTessFactor;

	float gHeightScale;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
};

Texture2D gDiffuseMap;
Texture2D gNormalMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 NormalL	: NORMAL;
	float3 TangentU : TANGENT;
	float2 Tex		: TEXCOORD;
};

struct VertexOut
{
	float3 PosW		: POSITION;
	float3 NormalW  : NORMAL;
	float3 TangentU : TANGENT;
	float2 Tex		: TEXCOORD;
	float  Tess		: TESSFACTOR;
	
};

struct PatchTess
{
	float TessEdgeFactor[3] : SV_TessFactor;
	float TessInsideFactor  : SV_InsideTessFactor;
};

struct HullOut
{
	float3 PosW		:POSITION;
	float3 NormalW	:NORMAL;
	float3 TangentU	:TANGENT;
	float2 Tex		:TEXCOORD;
};

struct DomainOut
{
	float4 PosH		:SV_POSITION;
	float3 PosW		:POSITION;
	float3 Normal	:NORMAL;
	float3 TangentU :TANGENT;
	float2 Tex		:TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL,(float3x3)gWorldInvTranspose);
	vout.TangentU = mul(vin.TangentU, (float3x3)gWorld);

	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	float d = distance(vout.PosW, gEyePosW);
	float tess = saturate((gMinTessDistance - d) / (gMinTessDistance - gMaxTessDistance));
	vout.Tess = gMinTessFactor + tess*(gMaxTessFactor - gMinTessFactor);

	return vout;

}

PatchTess ConstantHull(InputPatch<VertexOut, 3>patch, uint patchID:SV_PrimitiveID)
{
	PatchTess tess;
	tess.TessEdgeFactor[0] = 0.5f*(patch[1].Tess + patch[2].Tess);
	tess.TessEdgeFactor[1] = 0.5f*(patch[0].Tess + patch[2].Tess);
	tess.TessEdgeFactor[2] = 0.5f*(patch[1].Tess + patch[0].Tess);

	tess.TessInsideFactor = tess.TessEdgeFactor[0];

	return tess;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHull")]
HullOut HS(InputPatch<VertexOut, 3>patch, uint i:SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
	HullOut hout;
	hout.PosW		= patch[i].PosW;
	hout.NormalW	= patch[i].NormalW;
	hout.TangentU	= patch[i].TangentU;
	hout.Tex		= patch[i].Tex;

	return hout;
}

[domain("tri")]
DomainOut DS(float3 uvw:SV_DomainLocation, PatchTess patchTess,const OutputPatch<HullOut,3>tri)
{
	DomainOut dout;
	dout.PosW		= tri[0].PosW*uvw.x + tri[1].PosW*uvw.y + tri[2].PosW*uvw.z;
	dout.Normal		= tri[0].NormalW*uvw.x + tri[1].NormalW*uvw.y + tri[2].NormalW*uvw.z;
	dout.TangentU	= tri[0].TangentU*uvw.x + tri[1].TangentU*uvw.y + tri[2].TangentU*uvw.z;
	dout.Tex		= tri[0].Tex*uvw.x + tri[1].Tex*uvw.y + tri[2].Tex*uvw.z;

	dout.Normal = normalize(dout.Normal);

	//choose mipmap level 
	const float MipInterval = 20.0f;
	float MipLevel = clamp((distance(dout.PosW, gEyePosW) - MipInterval) / MipInterval, 0.0f, 6.0f);

	//sample height map 
	float height = gNormalMap.SampleLevel(samLinear, dout.Tex, MipLevel).a;

	dout.PosW += gHeightScale*(height - 1.0f);// *dout.Normal;

	dout.PosH = mul(float4(dout.PosW, 1.0f), gWorldViewProj);
	return dout;
}
float3 ConverNormalToWorldSpace(float3 normal, float3 normalW, float3 tangentW)
{
	float3 normalT = 2.0f*normal - 1.0f;

	float3 N = normalW;
	float3 T = normalize(tangentW - dot(N, tangentW)*tangentW);
	float3 B = cross(N, T);
	/*float3 B = cross(N, tangentW);
	float3 T = cross(B, N);*/

	float3x3 mat = float3x3(T, B, N);
	float3 normalWS = mul(normalT, mat);
	return normalWS;
}

float4 PS(DomainOut pin,uniform int gLightCount,uniform bool gUseTexture) :SV_Target
{
	
	//normalize the normal vector
	pin.Normal = normalize(pin.Normal);

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	//default texColor
	float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.f);
	if (gUseTexture)
	{
		//sample texture
		//float4 composColor = gCompositeMap.Sample(samAnisotropic, pin.Tex);
		texColor = gDiffuseMap.Sample(samLinear, pin.Tex);
		//texColor = texColor*composColor;
	}

	//lighting
	float4 lightColor = texColor;
	float3 normalMap = gNormalMap.Sample(samLinear, pin.Tex).xyz;
	float3 normal = ConverNormalToWorldSpace(normalMap, pin.Normal, pin.TangentU);

	if (gLightCount > 0)
	{
		//start with zero
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

		//sum the light contribution from each direction light
		[unroll]
		for (int i = 0; i < gLightCount; i++)
		{
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], normal, toEyeW, A, D, S);//pin.NormalW
			ambient += A;
			diffuse += D;
			spec += S;
		}
		lightColor = texColor*( ambient + diffuse ) + spec;
	}

	lightColor.a = gMaterial.Diffuse.a*texColor.a;
	return lightColor;

}

technique11 Light2Tex
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader(CompileShader(ds_5_0, DS()));
	    SetPixelShader(CompileShader(ps_5_0, PS(2,true)));
	}
}
