#ifndef BLENDINGDEMO_H
#define BLENDINGDEMO_H

#include "D3DApp.h"
#include "d3dx11effect.h"
#include "mathHelper.h"
#include "MeshGenerator.h"
#include "Waves.h"
#include "LightHelper.h"

enum RenderOptions
{
	Lighting = 0,
	Textures = 1,
	TextureAndFog = 2
};

class Stenciling :public D3DApp
{
public:
	Stenciling(HINSTANCE hInstance);
	~Stenciling();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:

	void CreateRoomGeometryBuffers();//vertex index
	void CreateSkullGeometryBuffers();//vertex index

private:
	ID3D11Buffer* pRoomVB;

	ID3D11Buffer* pSkullVB;
	ID3D11Buffer* pSkullIB;

	ID3D11ShaderResourceView* pFloorDiffuseMapSRV;
	ID3D11ShaderResourceView* pWallDiffuseMapSRV;
	ID3D11ShaderResourceView* pMirrorDiffuseMapSRV;


	DirectionalLight mDirLights[3];
	Material mRoomMat;
	Material mSkullMat;
	Material mMirrorMat;

	XMFLOAT4X4 mRoomWorld;
	XMFLOAT4X4 mSkullWorld;

	UINT mSkullIndexCount;
	XMFLOAT3 mSkullTranslation;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProject;

	XMFLOAT3 mEyePosW;

	RenderOptions mRenderOptions;

	float mThea;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};
#endif
