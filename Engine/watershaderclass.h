////////////////////////////////////////////////////////////////////////////////
// Filename: watershaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _WATERSHADERCLASS_H_
#define _WATERSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: WaterShaderClass
////////////////////////////////////////////////////////////////////////////////
class WaterShaderClass
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX reflection;
	};

	struct CamNormBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float timeElapsed;
		D3DXVECTOR4 padding2;
	};

	struct WaterBufferType
	{
		D3DXVECTOR4 refractionTint;
		D3DXVECTOR3 lightDirection;
		float reflectRefractScale;
		float specularShininess;
		D3DXVECTOR3 padding;
	};

public:
	WaterShaderClass();
	WaterShaderClass(const WaterShaderClass&);
	~WaterShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, 
				ID3D11ShaderResourceView*, D3DXVECTOR3, float, D3DXVECTOR4, D3DXVECTOR3, float, float);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
							 ID3D11ShaderResourceView*, D3DXVECTOR3, float, D3DXVECTOR4, D3DXVECTOR3, float, float);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_camNormBuffer;
	ID3D11Buffer* m_waterBuffer;
};

#endif