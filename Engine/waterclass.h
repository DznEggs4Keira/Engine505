////////////////////////////////////////////////////////////////////////////////
// Filename: waterclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _WATERCLASS_H_
#define _WATERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"

/////////////
// GLOBALS //
/////////////
const int REPEAT = 100;


////////////////////////////////////////////////////////////////////////////////
// Class name: WaterClass
////////////////////////////////////////////////////////////////////////////////
class WaterClass
{
private:

	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	WaterClass();
	WaterClass(const WaterClass&);
	~WaterClass();

	bool Initialize(ID3D11Device*, WCHAR*, int waterWidth, int waterHeight);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	float GetReflectRefractScale();
	D3DXVECTOR4 GetRefractionTint();
	float GetSpecularShininess();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool CalculateNormals();

	/*Textures*/
	void CalculateTextureCoordinates();

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	HeightMapType* m_heightMap;
	int m_waterWidth, m_waterHeight;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	float m_reflectRefractScale;
	D3DXVECTOR4 m_refractionTint;
	float m_specularShininess;
};

#endif