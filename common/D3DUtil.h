#ifndef D3DUTIL_H
#define D3DUTIL_H

#include <d3d11.h>
#include <D3DX11.h>
#include <d3dcommon.h>
#include <xnamath.h>
#include <DxErr.h>
#include <cassert>

#include <vector>

//检测d3d errors 
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                      \
     {                                             \
	 HRESULT hr = (x);                              \
	 if (FAILED(hr))                                 \
	   {                                              \
        DXTrace(__FILE__,(DWORD)__LINE__,hr,L#x,true); \
	   }                                                \
     }                                                   
#endif

#else
#ifndef  HR
#define HR(x)  (x)
#endif
#endif

//方便删除COM objects
#define ReleaseCOM(x) { if(x) { x->Release(); x = 0; }}//指针释放后 要将其值赋空值

namespace Colors
{
	XMGLOBALCONST XMVECTORF32 cWhite   = { 1.0f,1.0f,1.0f,1.0f };
	XMGLOBALCONST XMVECTORF32 cBlack   = { 0.0f,0.0f,0.0f,1.0f };
	XMGLOBALCONST XMVECTORF32 cRed     = { 1.0f,0.0f,0.0f,1.0f };
	XMGLOBALCONST XMVECTORF32 cGreen   = { 0.0f,1.0f,0.0f,1.0f };
	XMGLOBALCONST XMVECTORF32 cBlue    = { 0.0f,0.0f,1.0f,1.0f };
	XMGLOBALCONST XMVECTORF32 cYellow  = { 1.0f,1.0f,0.0f,1.0f };
	XMGLOBALCONST XMVECTORF32 cCyan    = { 0.0f,1.0f,1.0f,1.0f };
	XMGLOBALCONST XMVECTORF32 cMagenta = { 1.0f,0.0f,1.0f,1.0f };
		
	XMGLOBALCONST XMVECTORF32 cSilver  = { 0.75f,0.75f,0.75f,1.0f };
	XMGLOBALCONST XMVECTORF32 cLightSteelBlue = { 0.69f,0.77f,0.87f,1.0f };

}

#endif