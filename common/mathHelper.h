#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <windows.h>
#include <xnamath.h>

class MathHelper 
{
public:
	//return random float in [0,1]
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	//return random float in [a,b]
	static float RandF(float a, float b)
	{
		return a + (b - a) * (float)(rand()) / (float)RAND_MAX;
	}

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}
	 template<typename T>
	 static T Max(const T& a, const T& b)
	 {
		 return a < b ? b : a;
	 }

	 template<typename T>
	 static T Lerp(const T& a, const T& b, const T& t)
	 {
		 return a + (b - a)*t;
	 }

	 template<typename T>
	 static T Clamp(const T& x, const T& low, const T& high)
	 {
		 return x < low ? low : (x > high ? high : x);
	 }

	 //return the polar angle of the point (x,y) in [0,2*PI]
	 static float AngleFromXY(float x, float y);

	 static XMMATRIX InverseTranspose(CXMMATRIX M)
	 {
		 //Inverse-transpose without translation
		 XMMATRIX A = M;
		 A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		 XMVECTOR det = XMMatrixDeterminant(A);
		 return XMMatrixTranspose(XMMatrixInverse(&det, A));
	 }

	 static XMVECTOR RandUnitVec3();
	 static XMVECTOR RandHemisphereUnitVec3(XMVECTOR n);

	 static const float Infinity;
	 static const float Pi;
};

#endif