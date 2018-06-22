#include  "Effects.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
{
	std::ifstream fin(filename, std::ios::binary);
	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, device, &pFX));

}

Effect::~Effect()
{
	ReleaseCOM(pFX);
}
#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	:Effect(device,filename)
{
	pLight1Tech		= pFX->GetTechniqueByName("Light1");
	pLight2Tech		= pFX->GetTechniqueByName("Light2");
	pLight3Tech		= pFX->GetTechniqueByName("Light3");

	pLight0TexTech	= pFX->GetTechniqueByName("Light0Tex");
	pLight1TexTech	= pFX->GetTechniqueByName("Light1Tex");
	pLight2TexTech	= pFX->GetTechniqueByName("Light2Tex");
	pLight3TexTech	= pFX->GetTechniqueByName("Light3Tex");

	Light0TexAlphaClipTech = pFX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = pFX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = pFX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = pFX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech = pFX->GetTechniqueByName("Light1Fog");
	Light2FogTech = pFX->GetTechniqueByName("Light2Fog");
	Light3FogTech = pFX->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = pFX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = pFX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = pFX->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = pFX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = pFX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = pFX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = pFX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = pFX->GetTechniqueByName("Light3TexAlphaClipFog");

	pWorldViewProject	= pFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	pWorld				= pFX->GetVariableByName("gWorld")->AsMatrix();
	pWorldInvTranspose	= pFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	pTexTransform		= pFX->GetVariableByName("gTexTransform")->AsMatrix();
	pEyePosW			= pFX->GetVariableByName("gEyePosW")->AsVector();
	pFogColor			= pFX->GetVariableByName("gFogColor")->AsVector();
	pFogStart			= pFX->GetVariableByName("gFogStart")->AsScalar();
	pFogRange			= pFX->GetVariableByName("gFogRange")->AsScalar();
	pDirLights			= pFX->GetVariableByName("gDirLights");
	pMat				= pFX->GetVariableByName("gMaterial");
	pDiffuseMap			= pFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	pCompositeMap		= pFX->GetVariableByName("gCompositeMap")->AsShaderResource();
}

BasicEffect::~BasicEffect()
{

}
#pragma endregion

#pragma region Effects

BasicEffect* Effects::pBasicFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	pBasicFX = new BasicEffect(device, L"../shaders/Basic.fxo");
}

void Effects::DestroyAll()
{
	SafeDelete(pBasicFX);
}
#pragma endregion