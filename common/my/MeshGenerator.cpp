#include "MeshGenerator.h"

void MeshGenerator::CreateGrid(float width, float depth, UINT m, UINT n, MeshData& mesh)
{
	UINT vertexCount = m*n;
	UINT faceCount = (m - 1)*(n - 1) * 2;//the face of triangle

	//Create vetex

	//obtain the upper left point
	float halfWidth = 0.5*width;
	float halfDepth = 0.5*depth;

	//step vertex
	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	//step uv coordinate
	float du = 1.0 / (n - 1);
	float dv = 1.0 / (m - 1);

	mesh.Vertices.resize(vertexCount);
	for (UINT i = 0; i < m; i++)
	{
		float z = halfDepth - dz*i;
		for (UINT j = 0; j < n; j++)
		{
			float x = -halfWidth + j*dx;

			//create variable or copy data?
			/*XMFLOAT3 pos(x, 0.0f, z);
			XMFLOAT3 normal(0.0f, 1.0f, 0.0f);
			XMFLOAT3 tangentU(0.0f, 0.0f, 0.0f);
			XMFLOAT2 texUV= XMFLOAT2(j*du, i*dv);
			
			Vertex vetices(pos,normal,tangentU,texUV);
			mesh.Vertices.push_back(vetices);*/
			mesh.Vertices[i*n + j].Position = XMFLOAT3(x, 0.0f, z);
			mesh.Vertices[i*n + j].Normal   = XMFLOAT3(0.0f, 1.0f, 0.0f);
			mesh.Vertices[i*n + j].TangentU = XMFLOAT3(1.f, 0.0f, 0.f);

			//uv coordinate
			mesh.Vertices[i*n + j].TexC.x = j*du;
			mesh.Vertices[i*n + j].TexC.y = i*dv;
		}
	}

	//create index
	mesh.Indices.resize(faceCount*3);
	UINT index = 0;
	for (UINT i = 0; i <m-1; i++)
		for(UINT j=0; j <n-1; j++)
		{
			mesh.Indices[index]   = i*n+j;
			mesh.Indices[index+1] = i*n+j+1;
			mesh.Indices[index+2] = (i+1)*n+j;

			mesh.Indices[index+3] = (i + 1)*n +j;
			mesh.Indices[index+4] = i*n+j+1;
			mesh.Indices[index+5] = (i + 1)*n +j+1;
			index += 6;
		}
}