////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

#include "PostProcess.h"

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "terrainclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "terrainshaderclass.h"
#include "lightclass.h"

#include "SkyDomeClass.h"
#include "SkyDomeShaderClass.h"

#include "rendertextureclass.h" //water reflection and refraction
#include "reflectionshaderclass.h"
#include "waterclass.h"
#include "watershaderclass.h"

#include "frustumclass.h"
#include "quadtreeclass.h"

#include "Modelclass.h"
#include "lightshaderclass.h"

// Volumetric Models
/*For volumetric, i will load in a basic model using the model class and fill the shaders using the volumetric shader class. 
will use light.vs but make Volumetric.ps*/
#include "volumetricShaderClass.h"

//Post Processing
#include "orthowindowclass.h"
#include "textureshaderclass.h"
#include "othertextureshaderclass.h"

//Collison Class
#include "collisionClass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	float elapsedTime = 0.0f;

private:
	bool HandleInput(float);

	//Refraction and Reflection of the water
	void RenderRefractionToTexture();
	void RenderReflectionToTexture();
	
	//Renders the Entire Scene
	bool RenderGraphics(bool, bool);
	bool SceneRender();
	void ThrowKnives();

private:
	InputClass* m_Input;
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	TerrainClass* m_Terrain;
	TimerClass* m_Timer;
	PositionClass* m_Position;
	FpsClass* m_Fps;
	CpuClass* m_Cpu;
	FontShaderClass* m_FontShader;
	TextClass* m_Text;
	TerrainShaderClass* m_TerrainShader;
	LightClass* m_Light;

	SkyDomeClass* m_SkyDome;
	SkyDomeShaderClass* m_SkyDomeShader;

	RenderTextureClass *m_RefractionTexture, *m_ReflectionTexture;
	ReflectionShaderClass* m_ReflectionShader;
	WaterClass* m_Water;
	WaterShaderClass* m_WaterShader;

	FrustumClass* m_Frustum;
	QuadTreeClass* m_QuadTree;

	ModelClass* m_Model;
	LightShaderClass* m_LightShader;

	RenderTextureClass* m_PostProcessTarget;
	OrthoWindowClass* m_PostProcessWindow;
	TextureShaderClass* m_TextureShader;
	OtherTextureShaderClass* m_OtherTextureShader;
	DirectX::XMMATRIX m_PostProcessMatrix;

	VolumetricShaderClass* m_VolumetricShader;

	ModelClass* m_Knives; // model being passed is broken, must find another one

	CollisionClass* m_Collision;

	HWND hwnd_APP;

	//Collisison Models
	std::vector<ModelClass*> m_Volumetric; //balls

	std::vector<DirectX::XMFLOAT3> scaleVal; //scale of balls
	std::vector<DirectX::XMFLOAT3> posVal; // position of balls
	
	std::vector<bool> volDest; // destroyed or not

	//PostProcessing bools
	bool isEffectOn1 = false;
	bool isEffectOn2 = false;
};

#endif