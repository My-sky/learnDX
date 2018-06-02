#include "mathHelper.h"
#include <float.h>
#include <cmath>

const float MathHelper::Infinity = FLT_MAX;
const float MathHelper::Pi = 3.1415926535f;

float MathHelper::AngleFromXY(float x, float y)
{
	float thea = 0.0;

	if (x >= 0.0)
	{
		thea = atanf(y / x);
		if (thea < 0.0)
			thea += 2 * Pi;
	}
	else
		thea = atanf(y / x) + Pi;
	return thea;
}

XMVECTOR MathHelper::RandUnitVec3()
{
	XMVECTOR one = XMVectorSet(1.0f,1.0f,1.0f,1.0f);

	while (true)
	{
		XMVECTOR vec = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);

		if (XMVector3Greater(XMVector3LengthSq(vec), one))
			continue;
		return XMVector3Normalize(vec);
	}
}

XMVECTOR MathHelper::RandHemisphereUnitVec3(XMVECTOR n)
{
	XMVECTOR one = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR zero = XMVectorZero();

	while (true)
	{
		XMVECTOR vec = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);

		if (XMVector3Greater(XMVector3LengthSq(vec), one))
			continue;
		if (XMVector3Less(XMVector3Dot(n, vec), zero));//不明白这步判断的用意
			continue;
		return XMVector3Normalize(vec);
	}
}