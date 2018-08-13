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

	pLight0TexAlphaClipTech = pFX->GetTechniqueByName("Light0TexAlphaClip");
	pLight1TexAlphaClipTech = pFX->GetTechniqueByName("Light1TexAlphaClip");
	pLight2TexAlphaClipTech = pFX->GetTechniqueByName("Light2TexAlphaClip");
	pLight3TexAlphaClipTech = pFX->GetTechniqueByName("Light3TexAlphaClip");

	pLight1FogTech = pFX->GetTechniqueByName("Light1Fog");
	pLight2FogTech = pFX->GetTechniqueByName("Light2Fog");
	pLight3FogTech = pFX->GetTechniqueByName("Light3Fog");

	pLight0TexFogTech = pFX->GetTechniqueByName("Light0TexFog");
	pLight1TexFogTech = pFX->GetTechniqueByName("Light1TexFog");
	pLight2TexFogTech = pFX->GetTechniqueByName("Light2TexFog");
	pLight3TexFogTech = pFX->GetTechniqueByName("Light3TexFog");

	pLight0TexAlphaClipFogTech = pFX->GetTechniqueByName("Light0TexAlphaClipFog");
	pLight1TexAlphaClipFogTech = pFX->GetTechniqueByName("Light1TexAlphaClipFog");
	pLight2TexAlphaClipFogTech = pFX->GetTechniqueByName("Light2TexAlphaClipFog");
	pLight3TexAlphaClipFogTech = pFX->GetTechniqueByName("Light3TexAlphaClipFog");

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

#pragma region BillboardEffect
BillboardEffect::BillboardEffect(ID3D11Device* device, const std::wstring& filename)
	:Effect(device, filename)
{
	pLight3Tech = pFX->GetTechniqueByName("Light3");
	pLight3TexAlphaTech = pFX->GetTechniqueByName("Light3TexAlphaClip");
	pLight3TexAlphaClipFogTech = pFX->GetTechniqueByName("Light3TexAlphaClipFog");

	pWorldViewProject = pFX->GetVariableByName("gViewProj")->AsMatrix();
	pEyePosW = pFX->GetVariableByName("gEyePosW")->AsVector();
	pFogColor = pFX->GetVariableByName("gFogColor")->AsVector();
	pFogStart = pFX->GetVariableByName("gFogStart")->AsScalar();
	pFogRange = pFX->GetVariableByName("gFogRange")->AsScalar();
	pDirLights = pFX->GetVariableByName("gDirLights");
	pMat = pFX->GetVariableByName("gMaterial");
	pTreeArraySRV = pFX->GetVariableByName("gTreeMapArray")->AsShaderResource();

}

BillboardEffect::~BillboardEffect()
{

}
#pragma endregion


#pragma region Effects

BasicEffect* Effects::pBasicFX = 0;
BillboardEffect* Effects::pBillboardFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	pBasicFX = new BasicEffect(device, L"../shaders/Basic.fxo");
	pBillboardFX = new BillboardEffect(device, L"../shaders/Billboard.fxo");
}

void Effects::DestroyAll()
{
	SafeDelete(pBasicFX);
	SafeDelete(pBillboardFX);
}
#pragma endregion