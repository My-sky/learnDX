
#include "lightingHelper.fx"

cbuffer cbPerFrame
{
	DirectionalLight gDirLights[3];
	float3 gEyePosW;

	float gFogStart;
	float gFogRange;
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

Texture2D gDiffuseMap;
Texture2D gCompositeMap;

SamplerState samAnisotropic
{
	Filter = MIN_MAG_MIP_LINEAR;// ANISOTROPIC;
	//MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 NormalL	: NORMAL;
	float2 Tex		: TEXCOORD;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float4 PosW		: POSITION;
	float3 NormalW  : NORMAL;
	float2 Tex		:TEXCOORD;
	
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.NormalW = mul(vin.NormalL,(float3x3)gWorldInvTranspose);

	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;

}

float4 PS(VertexOut pin,uniform int gLightCount,uniform bool gUseTexture) :SV_Target
{
	
	//normalize the normal vector
	pin.NormalW = normalize(pin.NormalW);

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	//default texColor
	float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.f);
	if (gUseTexture)
	{
		//sample texture
		//float4 composColor = gCompositeMap.Sample(samAnisotropic, pin.Tex);
		texColor = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
		//texColor = texColor*composColor;
	}

	//lighting
	float4 lightColor = texColor;
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
			ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEyeW, A, D, S);
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
	    SetPixelShader(CompileShader(ps_5_0, PS(2,true)));
	}
}
