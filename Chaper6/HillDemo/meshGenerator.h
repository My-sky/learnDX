#pragma once
class MeshGenerator
{
public:
	MeshGenerator();
	~MeshGenerator();

	//create grid model
	void CreateGrid(float width, float depth, UINT m, UINT n, MeshData& mesh);
};