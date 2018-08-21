
#include "LightingHelper.fx"

cbuffer cbPerFrame
{
	DirectionalLight gDirLights[3];
	float3 gEyePosW;

	float  gFogStart;
	float  gFogRange;
	float4 gFogColor;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 PosL    : POSITION;
};

struct VertexOut
{
	float3 PosL    : POSITION;
};

// In Tessellation pipeline, VS is mostly used for animation to animate a coarse mesh
// before tessellating it.
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosL = vin.PosL;

	return vout;
}

struct PatchTess
{
	float EdgeTess[3]   : SV_TessFactor;
	float InsideTess : SV_InsideTessFactor;
};

PatchTess ConstantHS(InputPatch<VertexOut, 3> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess pt;

	// Uniform tessellation for this demo.
	int tess = 2;
	int insidetess = 2;

	pt.EdgeTess[0] = tess;
	pt.EdgeTess[1] = tess;
	pt.EdgeTess[2] = tess;
	//pt.EdgeTess[3] = tess;

	pt.InsideTess = insidetess;
	//pt.InsideTess[1] = insidetess;

	return pt;
}

struct HullOut
{
	float3 PosL : POSITION;
};

// This Hull Shader part is commonly used for a coordinate basis change, 
// for example changing from a quad to a Bezier bi-cubic.
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HullOut HS(InputPatch<VertexOut, 3> p,
	uint i : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	HullOut hout;

	hout.PosL = p[i].PosL;

	return hout;
}

struct DomainOut
{
	float4 PosH : SV_POSITION;
};

float4 BernsteinBasis(float t)
{
	float invT = 1.0f - t;

	return float4(invT * invT * invT,
		3.0f * t * invT * invT,
		3.0f * t * t * invT,
		t * t * t);
}

float3 CubicBezierSum(const OutputPatch<HullOut, 16> bezpatch, float4 basisU, float4 basisV)
{
	float3 sum = float3(0.0f, 0.0f, 0.0f);
	sum = basisV.x * (basisU.x*bezpatch[0].PosL + basisU.y*bezpatch[1].PosL + basisU.z*bezpatch[2].PosL + basisU.w*bezpatch[3].PosL);
	sum += basisV.y * (basisU.x*bezpatch[4].PosL + basisU.y*bezpatch[5].PosL + basisU.z*bezpatch[6].PosL + basisU.w*bezpatch[7].PosL);
	sum += basisV.z * (basisU.x*bezpatch[8].PosL + basisU.y*bezpatch[9].PosL + basisU.z*bezpatch[10].PosL + basisU.w*bezpatch[11].PosL);
	sum += basisV.w * (basisU.x*bezpatch[12].PosL + basisU.y*bezpatch[13].PosL + basisU.z*bezpatch[14].PosL + basisU.w*bezpatch[15].PosL);

	return sum;
}

float4 dBernsteinBasis(float t)
{
	float invT = 1.0f - t;

	return float4(-3 * invT * invT,
		3 * invT * invT - 6 * t * invT,
		6 * t * invT - 3 * t * t,
		3 * t * t);
}

// The domain shader is called for every vertex created by the tessellator.  
// It is like the vertex shader after tessellation.
[domain("tri")]
DomainOut DS(PatchTess patchTess,
	float3 uvw : SV_DomainLocation,
	const OutputPatch<HullOut, 3> bezPatch)
{
	DomainOut dout;

	//Bezer
	/*float4 basisU = BernsteinBasis(uv.x);
	float4 basisV = BernsteinBasis(uv.y);

	float3 p = CubicBezierSum(bezPatch, basisU, basisV);*/

	//sphere
	float3 position = float3(0.0f, 0.0f, 0.0f);
	float pi2 = 6.28318530;
	float pi = pi2 / 2;
	float R = 3.0f;
	float fi = pi*uvw.x;
	float theta = pi2*uvw.y;
	float sinFi, cosFi, sinTheta, cosTheta;
	sincos(fi, sinFi, cosFi);
	sincos(theta, sinTheta, cosTheta);
	position = float3(R*sinFi*cosTheta, R*sinFi*sinTheta, R*cosFi);

	//triangle
	position = uvw.x*bezPatch[0].PosL+uvw.y*bezPatch[1].PosL+uvw.z*bezPatch[2].PosL;
	dout.PosH = mul(float4(position, 1.0f), gWorldViewProj);

	return dout;
}

float4 PS(DomainOut pin) : SV_Target
{
	return float4(0.0f, 1.0f,1.0f, 1.0f);
}

technique11 Tess
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader(CompileShader(ds_5_0, DS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
