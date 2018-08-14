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
	float4x4 gViewProj;
	Material gMaterial;
};

cbuffer cbFixed
{
	//texture coordinate
	float2 gTexC[4] =
	{
		float2(0.0f,1.0f),
		float2(0.0f,0.0f),
		float2(1.0f,1.0f),
		float2(1.0f,0.0f)
	};
};

//texture array
Texture2DArray gTreeMapArray;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 PosW:POSITION;
	float2 SizeW:PSIZE;
};

struct VertexOut
{
	float3 CenterW:POSITION;
	float2 SizeW:PSIZE;
};

struct GeoOut
{
	float4 PosH		:SV_POSITION;
	float3 PosW		:POSITION;
	float3 NormalW	:NORMAL;
	float2 Tex		:TEXCOORD;
	uint   PrimID	:SV_PrimitiveID;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.CenterW = vin.PosW;
	vout.SizeW = vin.SizeW;
	return vout;
}

[maxvertexcount(4)]
void GS(point VertexOut gin[1],
	uint primID:SV_PrimitiveID,
	inout TriangleStream<GeoOut> triStream)
{
	//compute the coordinate system of the sprite
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = gEyePosW - gin[0].CenterW;
	look.y = 0.0f;//project to xz plane
	look = normalize(look);
	float3 right = cross(up, look);

	//compute the output triangle vertex
	float halfWidth  = gin[0].SizeW.x / 2.0f;
	float halfHeight = gin[0].SizeW.y / 2.0f;

	float4 v[4];
	v[0] = float4(gin[0].CenterW + halfWidth*right - halfHeight*up, 1.0f);
	v[1] = float4(gin[0].CenterW + halfWidth*right + halfHeight*up, 1.0f);
	v[2] = float4(gin[0].CenterW - halfWidth*right - halfHeight*up, 1.0f);
	v[3] = float4(gin[0].CenterW - halfWidth*right + halfHeight*up, 1.0f);

	//transform quad vertices to world space
	GeoOut gout;
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		gout.PosH		= mul(v[i],gViewProj);
		gout.PosW		= v[i].xyz;
		gout.NormalW	= look;
		gout.Tex		= gTexC[i];
		gout.PrimID		= primID;
		triStream.Append(gout);
	}
}

float4 PS(GeoOut pin, uniform int gLightCount, uniform bool gUseTexture, uniform bool gAlphaClip, uniform bool gFogEnabled) :SV_Target
{

	//normalize the normal vector
	pin.NormalW = normalize(pin.NormalW);

float3 toEyeW = gEyePosW - pin.PosW;

float distToEye = length(toEyeW);

toEyeW /= distToEye;

//default texColor
float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.f);
if (gUseTexture)
{
	//sample texture
	float3 uvw = float3(pin.Tex, pin.PrimID % 4);
	texColor = gTreeMapArray.Sample(samLinear, uvw);

	if (gAlphaClip)
	{
		clip(texColor.a - 0.05f);
	}
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
	lightColor = texColor*(ambient + diffuse) + spec;
}

//fog
if (gFogEnabled)
{
	float fogLerp = saturate((distToEye - gFogStart) / gFogRange);

	lightColor = lerp(lightColor, gFogColor, fogLerp);
}

lightColor.a = gMaterial.Diffuse.a*texColor.a;
return lightColor;

}

technique11 Light3
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		SetPixelShader(CompileShader(ps_5_0, PS(3, false, false, false)));
	}
}

technique11 Light3TexAlphaClip
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		SetPixelShader(CompileShader(ps_5_0, PS(3, true, true, false)));
	}
}

technique11 Light3TexAlphaClipFog
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0,GS()));
		SetPixelShader(CompileShader(ps_5_0, PS(3, true, true, true)));
	}
}