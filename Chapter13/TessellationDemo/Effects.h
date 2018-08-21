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
	void SetFogColor(const XMVECTOR& V)					{ pFogColor->SetFloatVector(reinterpret_cast<const float*>(&V)); }
	void SetFogStart(float f)							{ pFogStart->SetFloat(f); }
	void SetFogRange(float f)							{ pFogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)	{ pDirLights->SetRawValue(lights,0,3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)				{ pMat->SetRawValue(&mat,0,sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)	{ pDiffuseMap->SetResource(tex); }
	void SetCompositeMap(ID3D11ShaderResourceView* tex) { pCompositeMap->SetResource(tex); }




	ID3DX11EffectTechnique* pLight1Tech;
	ID3DX11EffectTechnique* pLight2Tech;
	ID3DX11EffectTechnique* pLight3Tech;

	ID3DX11EffectTechnique* pLight0TexTech;
	ID3DX11EffectTechnique* pLight1TexTech;
	ID3DX11EffectTechnique* pLight2TexTech;
	ID3DX11EffectTechnique* pLight3TexTech;

	ID3DX11EffectTechnique* pLight0TexAlphaClipTech;
	ID3DX11EffectTechnique* pLight1TexAlphaClipTech;
	ID3DX11EffectTechnique* pLight2TexAlphaClipTech;
	ID3DX11EffectTechnique* pLight3TexAlphaClipTech;

	ID3DX11EffectTechnique* pLight1FogTech;
	ID3DX11EffectTechnique* pLight2FogTech;
	ID3DX11EffectTechnique* pLight3FogTech;

	ID3DX11EffectTechnique* pLight0TexFogTech;
	ID3DX11EffectTechnique* pLight1TexFogTech;
	ID3DX11EffectTechnique* pLight2TexFogTech;
	ID3DX11EffectTechnique* pLight3TexFogTech;

	ID3DX11EffectTechnique* pLight0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* pLight1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* pLight2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* pLight3TexAlphaClipFogTech;

	ID3DX11EffectMatrixVariable*	pWorldViewProject;
	ID3DX11EffectMatrixVariable*	pWorld;
	ID3DX11EffectMatrixVariable*	pWorldInvTranspose;
	ID3DX11EffectMatrixVariable*	pTexTransform;
	ID3DX11EffectVectorVariable*	pEyePosW;
	ID3DX11EffectVectorVariable*	pFogColor;
	ID3DX11EffectScalarVariable*	pFogStart;
	ID3DX11EffectScalarVariable*	pFogRange;
	ID3DX11EffectVariable*	pDirLights;
	ID3DX11EffectVariable*	pMat;

	ID3DX11EffectShaderResourceVariable* pDiffuseMap;
	ID3DX11EffectShaderResourceVariable* pCompositeMap;
};
#pragma endregion

#pragma region BillboardEffect
class BillboardEffect:public Effect
{
public:
	BillboardEffect(ID3D11Device* device, const std::wstring& filename);
	~BillboardEffect();

	void SetWorldViewProject(CXMMATRIX M) { pWorldViewProject->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& V) { pEyePosW->SetRawValue(&V, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const XMVECTOR& V) { pFogColor->SetFloatVector(reinterpret_cast<const float*>(&V)); }
	void SetFogStart(float f) { pFogStart->SetFloat(f); }
	void SetFogRange(float f) { pFogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights) { pDirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat) { pMat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetTreeArraySRV(ID3D11ShaderResourceView* tex) { pTreeArraySRV->SetResource(tex); }





	ID3DX11EffectTechnique* pLight3Tech;
	ID3DX11EffectTechnique* pLight3TexAlphaTech;
	ID3DX11EffectTechnique* pLight3TexAlphaClipFogTech;

	ID3DX11EffectMatrixVariable*	pWorldViewProject;
	ID3DX11EffectVectorVariable*	pEyePosW;
	ID3DX11EffectVectorVariable*	pFogColor;
	ID3DX11EffectScalarVariable*	pFogStart;
	ID3DX11EffectScalarVariable*	pFogRange;
	ID3DX11EffectVariable*	pDirLights;
	ID3DX11EffectVariable*	pMat;

	ID3DX11EffectShaderResourceVariable* pTreeArraySRV;
};

#pragma region Tessellation
class TessellationEffect:public Effect
{
public:
	TessellationEffect(ID3D11Device* device, const std::wstring& filename);
	~TessellationEffect();

	void SetWorld(CXMMATRIX M) { pWorld->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProject(CXMMATRIX M) { pWorldViewProject->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M) { pWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& V) { pEyePosW->SetRawValue(&V, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const XMVECTOR& V) { pFogColor->SetFloatVector(reinterpret_cast<const float*>(&V)); }
	void SetFogStart(float f) { pFogStart->SetFloat(f); }
	void SetFogRange(float f) { pFogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights) { pDirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat) { pMat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex) { pDiffuseMap->SetResource(tex); }

	ID3DX11EffectTechnique* pTessTech;

	ID3DX11EffectMatrixVariable*	pWorld;
	ID3DX11EffectMatrixVariable*	pWorldViewProject;
	ID3DX11EffectMatrixVariable*	pWorldInvTranspose;
	ID3DX11EffectVectorVariable*	pEyePosW;
	ID3DX11EffectVectorVariable*	pFogColor;
	ID3DX11EffectScalarVariable*	pFogStart;
	ID3DX11EffectScalarVariable*	pFogRange;
	ID3DX11EffectVariable*	pDirLights;
	ID3DX11EffectVariable*	pMat;

	ID3DX11EffectShaderResourceVariable* pDiffuseMap;
};
#pragma endregion

#pragma region Effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BasicEffect* pBasicFX;
	static BillboardEffect* pBillboardFX;
	static TessellationEffect* pTessellationFX;
};
#pragma endregion

#endif
