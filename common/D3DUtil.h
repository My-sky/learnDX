#ifndef D3DUTIL_H
#define D3DUTIL_H

#include <D3DX11.h>
#include <xnamath.h>
#include <DxErr.h>
#include <cassert>

//���d3d errors 
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

//����ɾ��COM objects
#define ReleaseCOM(x) { if(x) { x->Release(); x = 0; }}//ָ���ͷź� Ҫ����ֵ����ֵ

#endif