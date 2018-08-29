#ifndef EFFECTS_H
#define EFFECTS_H

#include "D3DUtil.h"

#pragma region Effect
class Effect
{
public:
	Effect(ID3D11Device* device, const std::wstring& filename);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3DX11Effect* pFX;
};
#pragma endregion

#pragma region BasicEffect
class BasicEffect :public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicEffect();

	void SetWorldViewProject(CXMMATRIX M)				{ pWorldViewProject->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)							{ pWorld->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)				{ pWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)					{ pTexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& V)					{ pEyePosW->SetRawValue(&V,0,sizeof(XMFLOAT3)); }
	void SetDirLights(const DirectionalLight lights)	{ pDirLights->SetRawValue(&lights,0,sizeof(DirectionalLight)); }
	void SetMinTessDistance(const float min)			{ pMinTessDistance->SetFloat(min); }
	void SetMaxTessDistance(const float max)			{ pMaxTessDistance->SetFloat(max); }
	void SetMinTessFactor(const float minFactor)		{ pMinTessFactor->SetFloat(minFactor); }
	void SetMaxTessFactor(const float maxFactor)		{ pMaxTessFactor->SetFloat(maxFactor); }
	void SetHeightScale(const float scale)				{ pHeightScale->SetFloat(scale); }
	void SetMaterial(const Material& mat)				{ pMat->SetRawValue(&mat,0,sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)	{ pDiffuseMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)    { pNormalMap->SetResource(tex); }
	void SetHeightMap(ID3D11ShaderResourceView* tex)	{ pHeightMap->SetResource(tex); }




	ID3DX11EffectTechnique* pLight1Tech;
	ID3DX11EffectTechnique* pLight2Tech;
	ID3DX11EffectTechnique* pLight3Tech;

	ID3DX11EffectTechnique* pLight0TexTech;
	ID3DX11EffectTechnique* pLight1TexTech;
	ID3DX11EffectTechnique* pLight2TexTech;
	ID3DX11EffectTechnique* pLight3TexTech;

	ID3DX11EffectMatrixVariable*	pWorldViewProject;
	ID3DX11EffectMatrixVariable*	pWorld;
	ID3DX11EffectMatrixVariable*	pWorldInvTranspose;
	ID3DX11EffectMatrixVariable*	pTexTransform;
	ID3DX11EffectVectorVariable*	pEyePosW;
	ID3DX11EffectScalarVariable*	pMinTessDistance;
	ID3DX11EffectScalarVariable*	pMaxTessDistance;
	ID3DX11EffectScalarVariable*	pMinTessFactor;
	ID3DX11EffectScalarVariable*	pMaxTessFactor;
	ID3DX11EffectScalarVariable*	pHeightScale;
	ID3DX11EffectVariable*	pDirLights;
	ID3DX11EffectVariable*	pMat;

	ID3DX11EffectShaderResourceVariable* pDiffuseMap;
	ID3DX11EffectShaderResourceVariable* pNormalMap;
	ID3DX11EffectShaderResourceVariable* pHeightMap;

};
#pragma endregion

#pragma region Effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BasicEffect* pBasicFX;
};
#pragma endregion

#endif
