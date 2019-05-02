////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdio.h>
#include <vector>
#include <random>
using namespace std;

//_________________________________________//
//perlin noise from_______________________//
//https://github.com/Reputeless/PerlinNoise
//_________________________________________//
#include "External/PerlinNoise.hpp"
// use siv::Perlin to access

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


/////////////
// GLOBALS //
/////////////
const int TEXTURE_REPEAT = 100;

////////////////////////////////////////////////////////////////////////////////
// Class name: TerrainClass
////////////////////////////////////////////////////////////////////////////////
class TerrainClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	    D3DXVECTOR3 normal;
	};

	struct HeightMapType 
	{ 
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType 
	{ 
		float x, y, z;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool InitializeTerrain(ID3D11Device*, int terrainWidth, int terrainHeight, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();
	
	/*Quad tree class handles the terrain rendering now*/

	int GetVertexCount();
	void CopyVertexArray(void*);

	/*Various Terrain Generation Techniques*/
	void GenerateHeightMap(int); //with fractal brownian
	
	/*get textures*/
	ID3D11ShaderResourceView* GetGrassTexture();
	ID3D11ShaderResourceView* GetSlopeTexture();
	ID3D11ShaderResourceView* GetRockTexture();

	//get the height of the terrain at that point
	float GetHeight(float, float);

private:
	/*Procedural*/
	void DepositPart(int x, int z);
	float GenerateRandomHeight();
	/////////////////////////////

	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	/*Textures*/
	void CalculateTextureCoordinates();

	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
	void ReleaseTextures();
	///////////////////////////////////////////////

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();

private:
	int m_terrainWidth, m_terrainHeight;
	HeightMapType* m_heightMap;
	TextureClass *m_GrassTexture, *m_SlopeTexture, *m_RockTexture;

	int m_vertexCount;
	VertexType* m_vertices;

	///////////////////////////////////////
	// Randomiser
	//////////////////////////////////////

	random_device rd;
	mt19937 e2;
	uniform_real_distribution<float> height;
	uniform_real_distribution<float> FaultPos;
};

#endif