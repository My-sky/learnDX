#ifndef D3DUTIL_H
#define D3DUTIL_H

#include <D3DX11.h>
#include <xnamath.h>
#include <DxErr.h>
#include <cassert>

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

#endif