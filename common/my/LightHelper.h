//helper class for lighting

#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include <windows.h>
#include <xnamath.h>

//make sure structure alignment agree with HLSL structure padding rules.
//Element are packed into 4D vectors with the restriction 

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Reflect;
};

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT3	Direction;
	float Pad;
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;

	XMFLOAT3	Position;
	float		Range;

	XMFLOAT3	Att;
	float		Pad;
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;

	XMFLOAT3	Position;
	float		Range;

	XMFLOAT3	Direction;
	float		Spot;

	XMFLOAT3	Att;
	float		Pad;
};


#endif