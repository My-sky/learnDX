#include "Waves.h"
#include <algorithm>
#include <vector>
#include <cassert>

Waves::Waves()
	:mNumRows(0), mNumCols(0), mVertexCount(0), mTriangleCount(0),
	mK1(0.0f), mK2(0.0f), mK3(0.0f), mTimeStep(0.0f), mSpatialStep(0.0f),
	mPrevSolution(0), mCurrSolution(0), mNormals(0), mTangentX(0)
{

}

Waves::~Waves()
{
	delete[] mPrevSolution;
	delete[] mCurrSolution;
	delete[] mNormals;
	delete[] mTangentX;
}