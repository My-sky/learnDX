
#include "lightingHelper.fx"


cbuffer cbPerFrame
{
	DirectionalLight gDirLights;
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
Texture2D gHeightMap;

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
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 LightPos : POSITION1;
	float3 ViewPos	: POSITION2;
	float3 TanPos	: POSITION3;
	float2 Tex		: TEXCOORD;	
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	float3 N = normalize(mul(vin.NormalL,(float3x3)gWorldInvTranspose));
	float3 T = normalize(mul(vin.TangentU, (float3x3)gWorld));
	T = normalize(T - dot(T, N)*T);
	float3 B = cross(N, T);

	float3x3 TBN = transpose(float3x3(T, B, N));
	vout.TanPos = mul(vout.PosW, TBN);
	vout.LightPos = mul(gDirLights.Direction, TBN);
	vout.ViewPos = mul(gEyePosW, TBN);
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	return vout;

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
float2 ParallaxMapping(float2 tex, float3 viewDir)
{
	const float numLayers = 10.0f;
	float layerStep = 1.0f / numLayers;
	float currentDepth = 0.0f;
	float2 p = viewDir.xy*0.1f;
	float2 deltTexCoords = p / numLayers;

	float2 currentTex = tex;
	float currentHeight = gHeightMap.Sample(samLinear, tex).r;

	[unroll(100)]
	while (currentDepth < currentHeight)
	{
		currentDepth += layerStep;
		currentTex -= deltTexCoords;//tex - p*currentDepth / viewDir.z;
		currentHeight = gHeightMap.Sample(samLinear, currentTex).r;
	}
	float prevDepth = currentDepth-layerStep;
	float2 prevTex = currentTex + deltTexCoords;//tex - p*prevDepth / viewDir.z
	float prevHeight = gHeightMap.Sample(samLinear, prevTex).r;

	float currWeight = currentDepth - currentHeight;
	float prevWeight = prevHeight - prevDepth;
	float weight = currWeight / (currWeight + prevWeight);
	currentTex = weight*prevTex + (1.0f - weight)*currentTex;

	return currentTex;
	
	//common parallax Mapping
	//float height = gHeightMap.Sample(samLinear, tex).r;
	//float2 p = viewDir.xy*height /viewDir.z*0.1f;//viewDir.z
	//return tex - p;
}
float4 PS(VertexOut pin,uniform int gLightCount,uniform bool gUseTexture) :SV_Target
{
	float3 viewDir = normalize(pin.ViewPos - pin.TanPos);
	float2 texCoords = ParallaxMapping(pin.Tex, viewDir);
	//normalize the normal vector
	//pin.Normal = normalize(pin.Normal);

	//丢弃会导致一半的纹理消失
	//if (texCoords.x > 1.0f || texCoords.y > 1.0f || texCoords.x < 0.0f || texCoords.y < 0.0f)
		//discard;

	//float3 toEyeW = normalize(pin.ViewPos - pin.PosW);

	//default texColor
	float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.f);
	if (gUseTexture)
	{
		//sample texture
		//float4 composColor = gCompositeMap.Sample(samAnisotropic, pin.Tex);
		texColor = gDiffuseMap.Sample(samLinear, texCoords);
		//texColor = texColor*composColor;
	}

	//lighting
	float4 lightColor = texColor;
	float3 normalMap = gNormalMap.Sample(samLinear, texCoords).xyz;
	//float3 normal = ConverNormalToWorldSpace(normalMap, pin.Normal, pin.TangentU);

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
			ComputeDirectionalLight(gMaterial, gDirLights, pin.LightPos,normalMap, viewDir, A, D, S);//pin.NormalW
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
