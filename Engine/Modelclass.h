////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <fstream>
#include <random>

using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"

//USING AN EXTERNAL HEADER FOR OBJ MODEL LOADING
//#include "OBJLoader.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	D3DXVECTOR3 GetRandomScale();
	D3DXVECTOR3 GetRandomPos();

	//will use these two for the volumetric balls only for now
	D3DXMATRIX GetModelMatrix();
	void SetModelMatrix(D3DXVECTOR3, D3DXVECTOR3);


private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	bool LoadModel(char*);
	void ReleaseModel();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	ModelType* m_model;

	D3DXMATRIX modelWorldMatrix;

	///////////////////////////////////////
	// Randomiser
	//////////////////////////////////////

	random_device rd;
	mt19937 e2;
	uniform_real_distribution<float> scale;
	uniform_real_distribution<float> pos;
};

#endif