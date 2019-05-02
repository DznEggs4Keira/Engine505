////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	//PRIMARY
	m_Input = 0;
	m_Direct3D = 0;
	m_Camera = 0;
	m_Timer = 0;
	m_Position = 0;

	//UI
	m_Text = 0;
	m_Fps = 0;
	m_Cpu = 0;
	m_FontShader = 0;

	//SCENE
	m_Frustum = 0;
	m_QuadTree = 0;
	m_Terrain = 0;
	m_TerrainShader = 0;

	m_SkyDome = 0;
	m_SkyDomeShader = 0;

	m_Water = 0;
	m_WaterShader = 0;
	m_RefractionTexture = 0;
	m_ReflectionTexture = 0;
	m_ReflectionShader = 0;

		//SUN/MOON
		m_Model = 0;
		m_LightShader = 0;

		//Volumetric Models
		m_Volumetric.reserve(10); //reserve 10 spaces in memory for 10 balls
		posVal.reserve(10);
		scaleVal.reserve(10);
		volDest.reserve(10);

		m_VolumetricShader = 0;

		//THROWABLE models
		m_Knives = 0;

	//PP
	m_PostProcessTarget = 0;
	m_PostProcessWindow = 0;
	m_TextureShader = 0;
	m_OtherTextureShader = 0;

	m_Collision = 0;

	m_Light = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	hwnd_APP = hwnd;

	bool result;
	float cameraX, cameraY, cameraZ;
	D3DXMATRIX TextViewMatrix;
	
	char videoCard[128];
	int videoMemory;
	
	//Save the Initial Position of the Camera
	cameraX = 54.0f;
	cameraY = 34.0f;
	cameraZ = 8.0f;

							/*------------------------------------- INPUT -------------------------------------------------------*/

	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	m_Input = new InputClass;
	m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);

							/*------------------------------------- D3D Class -------------------------------------------------------*/

	// Create and initialise the Direct3D object.
	m_Direct3D = new D3DClass;
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED,
		hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

							/*------------------------------------- CAMERA CLASS -------------------------------------------------------*/

	// Create the camera object.
	m_Camera = new CameraClass;

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(TextViewMatrix);

							/*------------------------------------- TEXT CLASS -------------------------------------------------------*/

	// Create and initialise the text object.
	m_Text = new TextClass;
	m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		hwnd, screenWidth, screenHeight, TextViewMatrix);

	// Retrieve the video card information.
	m_Direct3D->GetVideoCardInfo(videoCard, videoMemory);

	// Set the video card information in the text object.
	m_Text->SetVideoCardInfo(videoCard, videoMemory, m_Direct3D->GetDeviceContext());

	// Create the fps object.
	m_Fps = new FpsClass;
	m_Fps->Initialize();

	// Create the cpu object.
	m_Cpu = new CpuClass;
	m_Cpu->Initialize();

	// Create the font shader object.
	m_FontShader = new FontShaderClass;
	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);

							/*------------------------------------- TIMER CLASS -------------------------------------------------------*/

	// Create the timer object.
	m_Timer = new TimerClass;
	m_Timer->Initialize();

							/*------------------------------------- POSITION CLASS -------------------------------------------------------*/

	// Create the position object.
	m_Position = new PositionClass;

	//set the initial position and rotation of the camera
	m_Position->SetPosition(cameraX, cameraY, cameraZ);

								/*------------------------------------- TERRAIN CLASS -------------------------------------------------------*/

	// Create and initialise the terrain object.
	m_Terrain = new TerrainClass;
	m_Terrain->InitializeTerrain(m_Direct3D->GetDevice(), 250, 250, L"../Engine/data/grass.dds", L"../Engine/data/rock.dds",
		L"../Engine/data/slope.dds");   //initialise the flat terrain.

	// Create and Initialise the terrain shader object.
	m_TerrainShader = new TerrainShaderClass;
	m_TerrainShader->Initialize(m_Direct3D->GetDevice(), hwnd);

							/*--------------------- QUAD TREE CLASS SETUP (TIED TO THE TERRAIN CLASS FOR NOW) ---------------*/

	// Create the frustum object.
	m_Frustum = new FrustumClass;

	// Create and initialise the quad tree object.
	m_QuadTree = new QuadTreeClass;
	m_QuadTree->Initialize(m_Terrain, m_Direct3D->GetDevice());

							/*------------------------------------- MODELS -------------------------------------------------------*/

	/*SUN/MOON MODEL*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_Model = new ModelClass;
	m_Model->Initialize(m_Direct3D->GetDevice(), "../Engine/data/skydome.txt", L"../Engine/data/moon.dds");

	m_LightShader = new LightShaderClass;
	m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*VOLUMETRIC BALLS*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Volumetric Models
	for (int i = 0; i < 10; ++i)
	{
		m_Volumetric.push_back(new ModelClass);
		if (m_Volumetric.at(i) == 0)
		{
			return false;
		}

		result = m_Volumetric.at(i)->Initialize(m_Direct3D->GetDevice(), "../Engine/data/cube.txt", L"../Engine/data/water.dds");
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the Volumetric object.", L"Error", MB_OK);
			return false;
		}

		//RANDOM GENERATE POS AND SCALE OF BALL AND STORE
		scaleVal.push_back(m_Volumetric.at(i)->GetRandomScale());
		posVal.push_back(m_Volumetric.at(i)->GetRandomPos());

		//to make sure that no balls are rendered below the terrain or too high above it
		posVal.at(i).y = (m_Terrain->GetHeight(posVal.at(i).x, posVal.at(i).z) + 2.0f);

		m_Volumetric.at(i)->SetModelMatrix(posVal.at(i), scaleVal.at(i));

		volDest.push_back(false);
	}
	
	m_VolumetricShader = new VolumetricShaderClass;
	if (!m_VolumetricShader)
	{
		return false;
	}

	result = m_VolumetricShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*THROWABLE*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_Knives = new ModelClass;
	m_Knives->Initialize(m_Direct3D->GetDevice(), "../Engine/data/Knife.txt", L"../Engine/data/rock.dds");

	//the shader used to color this throwable is the same as the post process one for now
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


							/*------------------------------------------------------------ SKY -------------------------------------------------------*/

	// Create and initialise the sky dome object.
	m_SkyDome = new SkyDomeClass;
	m_SkyDome->Initialize(m_Direct3D->GetDevice());

	// Create and initialise the sky dome shader object.
	m_SkyDomeShader = new SkyDomeShaderClass;
	m_SkyDomeShader->Initialize(m_Direct3D->GetDevice(), hwnd);

							/*------------------------------------------------------------ WATER CLASS -------------------------------------------------------*/

	// Create and initialise the water object.
	m_Water = new WaterClass;
	m_Water->Initialize(m_Direct3D->GetDevice(), L"../Engine/data/waternormal.dds", 250, 250);

	// Create and initialise the water shader object.
	m_WaterShader = new WaterShaderClass;
	m_WaterShader->Initialize(m_Direct3D->GetDevice(), hwnd);

						/*------------------------------------------------------------ REFLECTION AND REFRACTION FOR WATER -------------------------------------------------------*/

	// Create and initialise the refraction render to texture object.
	m_RefractionTexture = new RenderTextureClass;
	m_RefractionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 
		SCREEN_DEPTH, SCREEN_NEAR);

	// Create and initialise the reflection render to texture object.
	m_ReflectionTexture = new RenderTextureClass;
	m_ReflectionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 
		SCREEN_DEPTH, SCREEN_NEAR);

	// Create and initialise the reflection shader object.
	m_ReflectionShader = new ReflectionShaderClass;
	result = m_ReflectionShader->Initialize(m_Direct3D->GetDevice(), hwnd);

								/*------------------------------------------------------------ POST PROCESSING -------------------------------------------------------*/

	//Initialise postprocess render texture and window quad
	m_PostProcessTarget = new RenderTextureClass;
	m_PostProcessTarget->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 
		SCREEN_DEPTH, SCREEN_NEAR);

	m_PostProcessWindow = new OrthoWindowClass;
	m_PostProcessWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight);
	
	//Render the view matrix for the post process effect
	m_PostProcessWindow->RenderPostProcessMatrixMatrix();

	m_TextureShader = new TextureShaderClass;
	m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);

	m_OtherTextureShader = new OtherTextureShaderClass;
	m_OtherTextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);

							/*------------------------------------------------------------ COLLISION OBJECT-------------------------------------------------------*/

	m_Collision = new CollisionClass;
	if (!m_Collision)
	{
		return false;
	}

								/*------------------------------------- SETUP INITIAL LIGHT -------------------------------------------------------*/

	// Create the light object.
	m_Light = new LightClass;

	// Initialize the light object.
	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, -1.0f, 1.0f);

	////////////////////////////////////////////////////////////////////////////// END ////////////////////////////////////////////////////////////////////////////////////////

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	if (m_Collision)
	{
		delete m_Collision;
		m_Collision = 0;
	}

	////////////////////////////////////////// POST PROCESS EFFECTS //////////////////////////////

	if (m_OtherTextureShader)
	{
		m_OtherTextureShader->Shutdown();
		delete m_OtherTextureShader;
		m_OtherTextureShader = 0;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_PostProcessWindow)
	{
		m_PostProcessWindow->Shutdown();
		delete m_PostProcessWindow;
		m_PostProcessWindow = 0;
	}

	if (m_PostProcessTarget)
	{
		m_PostProcessTarget->Shutdown();
		delete m_PostProcessTarget;
		m_PostProcessTarget = 0;
	}

	///////////////////////////////////////////// MODELS ///////////////////////////////////////////////////
	/*THROWABLE MODEL*/
	if (m_Knives)
	{
		m_Knives->Shutdown();
		delete m_Knives;
		m_Knives = 0;
	}

	/*VOLUMETRIC BALLS*/
	if (m_VolumetricShader)
	{
		m_VolumetricShader->Shutdown();
		delete m_VolumetricShader;
		m_VolumetricShader = 0;
	}

	for (int i = 0; i < 10; ++i)
	{
		if (!volDest.at(i))
		{
			m_Volumetric.at(i)->Shutdown();
			m_Volumetric.at(i) = 0;
		}
	}

	/*SUN/MOON MODEL*/
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	///////////////////////////////////////// WATER ////////////////////////////////////////

	// Release the reflection shader object.
	if (m_ReflectionShader)
	{
		m_ReflectionShader->Shutdown();
		delete m_ReflectionShader;
		m_ReflectionShader = 0;
	}

		// Release the reflection render to texture object.
	if (m_ReflectionTexture)
	{
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = 0;
	}

	// Release the refraction render to texture object.
	if (m_RefractionTexture)
	{
		m_RefractionTexture->Shutdown();
		delete m_RefractionTexture;
		m_RefractionTexture = 0;
	}

	// Release the water shader object.
	if (m_WaterShader)
	{
		m_WaterShader->Shutdown();
		delete m_WaterShader;
		m_WaterShader = 0;
	}

	// Release the water object.
	if (m_Water)
	{
		m_Water->Shutdown();
		delete m_Water;
		m_Water = 0;
	}

	/////////////////////////////////////////////// SKY /////////////////////////////////////////////////

	// Release the sky dome shader object.
	if (m_SkyDomeShader)
	{
		m_SkyDomeShader->Shutdown();
		delete m_SkyDomeShader;
		m_SkyDomeShader = 0;
	}

	// Release the sky dome object.
	if (m_SkyDome)
	{
		m_SkyDome->Shutdown();
		delete m_SkyDome;
		m_SkyDome = 0;
	}

	/////////////////////////////////////////////// TERRAIN /////////////////////////////////////////////////

	// Release the terrain shader object.
	if (m_TerrainShader)
	{
		m_TerrainShader->Shutdown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}

	// Release the terrain object.
	if (m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	// Release the quad tree object.
	if (m_QuadTree)
	{
		m_QuadTree->Shutdown();
		delete m_QuadTree;
		m_QuadTree = 0;
	}

	// Release the frustum object.
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	////////////////////////////////////////////// UI ///////////////////////////////////////////////

	// Release the font shader object.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the cpu object.
	if (m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the text object.
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}


	///////////////////////////////////////////// PRIMARY //////////////////////////////////////////////

	// Release the position object.
	if (m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}
}


bool ApplicationClass::Frame()
{
	bool result, foundHeight;
	D3DXVECTOR3 position;
	static bool isEKeyPressed = false;
	static bool isQKeyPressed = false;
	float height;

	// Read the user input.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsEscapePressed() == true)
	{
		return false;
	}

				/*-------------------- Check if Keys were pressed for PP --------------------*/
	//TOGGLE POST PROCESS EFFECTS
	if (m_Input->IsEPressed())
	{
		isEKeyPressed = true;
	}

	if (!m_Input->IsEPressed())
	{
		if (isEKeyPressed)
		{
			isEffectOn1 = !isEffectOn1;
		}
		isEKeyPressed = false;
	}

	
	if (m_Input->IsQPressed())
	{
		isQKeyPressed = true;
	}

	if (!m_Input->IsQPressed())
	{
		if (isQKeyPressed)
		{
			isEffectOn2 = !isEffectOn2;
		}
		isQKeyPressed = false;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////

	// Update the system stats.
	m_Timer->Frame();

	// Do the frame input processing.
	result = HandleInput(m_Timer->GetTime());
	if (!result)
	{
		return false;
	}

							/* Controls Height Based Movement */

	// Get the current position of the camera.
	position = m_Camera->GetPosition();

	// Get the height of the triangle that is directly underneath the given camera position.
	foundHeight = m_QuadTree->GetHeightAtPosition(position.x, position.z, height);
	if (foundHeight)
	{
		// If there was a triangle under the camera then position the camera just above it by two units.
		m_Camera->SetPosition(position.x, height + 3.0f, position.z);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////

							/*Randomise Balls*/
	if (m_Input->isRPressed())
	{
		for (int i = 0; i < 10; i++)
		{
			posVal.at(i) = m_Volumetric.at(i)->GetRandomPos();
			scaleVal.at(i) = m_Volumetric.at(i)->GetRandomScale();
			volDest.at(i) = false;
		}
	}

	m_Fps->Frame();
	m_Cpu->Frame();

	// Update the FPS value in the text object.
	result = m_Text->SetFps(m_Fps->GetFps(), m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Update the CPU usage value in the text object.
	result = m_Text->SetCpu(m_Cpu->GetCpuPercentage(), m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	/*--------------- check if any post process effect is true then send that, 
											else send false for normal scene --------------------*/

	// Render the graphics.
	result = RenderGraphics(isEffectOn1, isEffectOn2);
	if (!result)
	{
		return false;
	}

	return result;
}


bool ApplicationClass::HandleInput(float frameTime)
{
	bool keyDown, result;
	float posX, posY, posZ, rotX, rotY, rotZ;
	D3DXVECTOR3 Rot, tempRot;

	//need this for the water to make it wave
	elapsedTime += frameTime;

	// Set the frame time for calculating the updated position.
	m_Position->SetFrameTime(frameTime);

	//Camera rotation via key board
	/*keyDown = m_Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown); //Turn Left

	keyDown = m_Input->IsRightPressed();
	m_Position->TurnRight(keyDown); //Turn Right

	keyDown = m_Input->IsUpPressed();
	m_Position->LookUpward(keyDown); //Look Up

	keyDown = m_Input->IsDownPressed();
	m_Position->LookDownward(keyDown); //Look Down*/
	
	//Camera rotation via mouse
	Rot = m_Input->GetMouseMovement();
	tempRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	tempRot.x += Rot.x * 1 / 10;
	tempRot.y += Rot.y * 1 / 10;
	m_Position->RotateCamera(tempRot);

	// Handle the input
	keyDown = m_Input->IsWPressed();
	m_Position->MoveForward(keyDown); // Move Forward

	keyDown = m_Input->IsSPressed();
	m_Position->MoveBackward(keyDown); // Move Backward

	keyDown = m_Input->IsAPressed();
	m_Position->MoveUpward(keyDown); //Move Up

	keyDown = m_Input->IsDPressed();
	m_Position->MoveDownward(keyDown); // Move Down


	// Set the position and rotation of the camera based on input taken from user

	m_Position->GetPosition(posX, posY, posZ);
	m_Camera->SetPosition(posX, posY, posZ);

	m_Position->GetRotation(rotX, rotY, rotZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	// Update the text shown on screen
	// Update the position values in the text object.
	result = m_Text->SetCameraPosition(posX, posY, posZ, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Update the rotation values in the text object.
	result = m_Text->SetCameraRotation(rotX, rotY, rotZ, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	return true;
}

void ApplicationClass::RenderRefractionToTexture()
{
	D3DXVECTOR4 clipPlane;
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;


	// Setup a clipping plane based on the height of the water to clip everything above it to create a refraction.
	clipPlane = D3DXVECTOR4(0.0f, -1.0f, 0.0f, /*Water Height is 1*/1.0f + 0.1f);

	// Set the render target to be the refraction render to texture.
	m_RefractionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the refraction render to texture.
	m_RefractionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Render the terrain using the reflection shader and the refraction clip plane to produce the refraction effect.
	// Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// Set the terrain shader parameters that it will use for rendering.
	m_TerrainShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		m_Terrain->GetGrassTexture(), m_Terrain->GetSlopeTexture(), m_Terrain->GetRockTexture(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection());

	// Render the terrain using the quad tree and terrain shader.
	m_QuadTree->Render(m_Frustum, m_Direct3D->GetDeviceContext(), m_TerrainShader);

	m_ReflectionShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetVertexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Terrain->GetGrassTexture(), m_Terrain->GetSlopeTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), 2.0f,
		clipPlane);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return;
}

//Here is where we render the reflection of the scene to a texture. We render everything above the water to a texture.

void ApplicationClass::RenderReflectionToTexture()
{
	D3DXVECTOR4 clipPlane;
	D3DXMATRIX reflectionViewMatrix, worldMatrix, projectionMatrix, pos;
	D3DXVECTOR3 cameraPosition;


	// Setup a clipping plane based on the height of the water to clip everything below it.
	clipPlane = D3DXVECTOR4(0.0f, 1.0f, 0.0f, -1.0f/*1 is the water height*/);

	// Set the render target to be the reflection render to texture.
	m_ReflectionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the reflection render to texture.
	m_ReflectionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to render the reflection and create a reflection view matrix.
	m_Camera->RenderReflection(1.0f/*1 is the water height*/);

	// Get the camera reflection view matrix instead of the normal view matrix.
	m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);

	// Get the world and projection matrices from the d3d object.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetWorldMatrix(pos);

	// Get the position of the camera.
	cameraPosition = m_Camera->GetPosition();

	// Invert the Y coordinate of the camera around the water plane height for the reflected camera position.
	cameraPosition.y = -cameraPosition.y + (1.0f/*1 is the water height*/ * 2.0f);

	// Translate the sky dome and sky plane to be centered around the reflected camera position.
	D3DXMatrixTranslation(&worldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Turn off back face culling and the Z buffer.
	m_Direct3D->TurnOffCulling();	m_Direct3D->TurnOnAlphaBlending();	m_Direct3D->TurnZBufferOff();

	// Render the sky dome using the reflection view matrix.
	m_SkyDome->Render(m_Direct3D->GetDeviceContext());

	m_SkyDomeShader->Render(m_Direct3D->GetDeviceContext(), m_SkyDome->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix,
		m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor());

	// Enable back face culling.
	m_Direct3D->TurnOnCulling();	m_Direct3D->TurnZBufferOn();

	// Reset the world matrix.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	for (int i = 0; i < 10; ++i)
	{
		if (!volDest.at(i))
		{
			m_Volumetric.at(i)->Render(m_Direct3D->GetDeviceContext());

			m_VolumetricShader->Render(m_Direct3D->GetDeviceContext(), m_Volumetric.at(i)->GetIndexCount(), m_Volumetric.at(i)->GetModelMatrix(), reflectionViewMatrix, projectionMatrix,
				m_Volumetric.at(i)->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), elapsedTime);
		}
	}

	m_Direct3D->TurnOffAlphaBlending();

	// Reset the world matrix.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Render the terrain using the reflection view matrix and reflection clip plane.
	// Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, reflectionViewMatrix);

	// Set the terrain shader parameters that it will use for rendering.
	m_TerrainShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, reflectionViewMatrix, projectionMatrix,
		m_Terrain->GetGrassTexture(), m_Terrain->GetSlopeTexture(), m_Terrain->GetRockTexture(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection());

	// Render the terrain using the quad tree and terrain shader.
	m_QuadTree->Render(m_Frustum, m_Direct3D->GetDeviceContext(), m_TerrainShader);
	
	m_ReflectionShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetVertexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix,
		m_Terrain->GetSlopeTexture(), m_Terrain->GetRockTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), 2.0f,
		clipPlane);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return;
}

bool ApplicationClass::RenderGraphics(bool PPE1, bool PPE2)
{
	D3DXMATRIX renderTextureMatrix, textMatrix, orthoMatrix;
	bool result;

	//There are three render steps. First render the refraction of the scene to a texture. Next render the reflection of the scene to a texture.
	//And then finally render the entire scene using the refraction and reflection textures.

	// Render the refraction of the scene to a texture.
	RenderRefractionToTexture();

	// Render the reflection of the scene to a texture.
	RenderReflectionToTexture();

	// Set the render target to be the render to texture.
	m_PostProcessTarget->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_PostProcessTarget->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	result = SceneRender();
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->ResetViewport();
	m_Direct3D->SetBackBufferRenderTarget();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(1.0f, 0.0f, 0.5f, 0.0f);

	/////////////////////////////////////////// RENDER POST PROCESSED SCENE /////////////////////////////////////////////////////////////////////////////

	if(PPE1 || PPE2)
	{

		//Get the view matrix which we set up in the initialize
		m_PostProcessWindow->GetPostProcessMatrix(m_PostProcessMatrix);
		//Render to texture is an identity matrix
		D3DXMatrixIdentity(&renderTextureMatrix);
		//Get the ortho matrix
		m_PostProcessTarget->GetOrthoMatrix(orthoMatrix);

		// Turn off the Z buffer to begin all 2D rendering.
		m_Direct3D->TurnZBufferOff();

		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_PostProcessWindow->Render(m_Direct3D->GetDeviceContext());

		if (PPE1)
		{
			// Render the debug window using the texture shader.
			result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_PostProcessWindow->GetIndexCount(), renderTextureMatrix, m_PostProcessMatrix,
				orthoMatrix, m_PostProcessTarget->GetShaderResourceView());
			if (!result)
			{
				return false;
			}
		}
		
		if (PPE2)
		{
			// Render the debug window using the texture shader.
			result = m_OtherTextureShader->Render(m_Direct3D->GetDeviceContext(), m_PostProcessWindow->GetIndexCount(), renderTextureMatrix, m_PostProcessMatrix,
				orthoMatrix, m_PostProcessTarget->GetShaderResourceView());
			if (!result)
			{
				return false;
			}
		}

		// Turn the Z buffer back on now that all 2D rendering has completed.
		m_Direct3D->TurnZBufferOn();

		//////////////////////////////////////////////////////////////// RENDER THE TEXT ONTO THE SCREEN ///////////////////////////////////////////

		// Reset the world matrix.
		m_Direct3D->GetWorldMatrix(textMatrix);

		// Turn off the Z buffer to begin all 2D rendering.
		m_Direct3D->TurnZBufferOff();	m_Direct3D->TurnOnAlphaBlending();

		// Render the text user interface elements.
		m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, textMatrix, orthoMatrix);

		// Turn off alpha blending after rendering the text.
		m_Direct3D->TurnOffAlphaBlending();		m_Direct3D->TurnZBufferOn();
	}

	else
	{
		/////////////////////////////////////////// RENDER NORMAL SCENE /////////////////////////////////////////////////////////////////////////////
		SceneRender();

		//////////////////////////////////////////////////////////////// RENDER THE TEXT ONTO THE SCREEN ///////////////////////////////////////////
		// Reset the world matrix.
		m_Direct3D->GetWorldMatrix(textMatrix);
		m_Direct3D->GetOrthoMatrix(orthoMatrix);

		// Turn off the Z buffer to begin all 2D rendering.
		m_Direct3D->TurnZBufferOff();	m_Direct3D->TurnOnAlphaBlending();

		// Render the text user interface elements.
		m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, textMatrix, orthoMatrix);

		// Turn off alpha blending after rendering the text.
		m_Direct3D->TurnOffAlphaBlending();		m_Direct3D->TurnZBufferOn();
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

bool ApplicationClass::SceneRender()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, reflectionViewMatrix;
	D3DXVECTOR3 cameraPosition;
	D3DXMATRIX pos;
	static float rotation = 0.0f;

	bool result;

	// Update the rotation variable each frame. --- for sun/moon model
	rotation += ((float)D3DX_PI * 0.3f);
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	//add new variables which control in what axis the cube rotates
	float rotationX = rotation * 0.02f;
	float rotationY = rotation * 0.03f;
	float rotationZ = rotation * 0.4f;

								/* ------------------------------------- CAMERA -----------------------------------------------*/

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);


							/* ---------------------------------------- RENDER SKY -----------------------------------------*/
	//Here is where we get the position of the camera and then translate our sky dome to be centered around the camera position.

	// Get the position of the camera.
	cameraPosition = m_Camera->GetPosition();

	// Translate the sky dome to be centered around the camera position.
	D3DXMatrixTranslation(&worldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	//Before rendering the sky dome we turn off both back face culling and the Z buffer.

	// Turn off back face culling.
	m_Direct3D->TurnOffCulling();	m_Direct3D->TurnZBufferOff();

	// Render the sky dome using the sky dome shader.
	m_SkyDome->Render(m_Direct3D->GetDeviceContext());
	m_SkyDomeShader->Render(m_Direct3D->GetDeviceContext(), m_SkyDome->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor());

	// Turn back face culling back on.
	m_Direct3D->TurnOnCulling();	m_Direct3D->TurnZBufferOn();

									/* ------------------------------------------- MODEL ------------------------------------------ */

	///////////////////////////////////////////////////////////// SUN/MOON MODEL ///////////////////////////////////////////////////////////////
	// Reset the world matrix.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	m_Model->Render(m_Direct3D->GetDeviceContext());

	D3DXMatrixScaling(&worldMatrix, 40.0f, 40.0f, 40.0f);
	D3DXMatrixTranslation(&pos, 40.0f, 50.0f, 800.0f);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &pos);

	m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());


						/* ------------------------------------------------- TERRAIN -----------------------------------------*/

	// Reset the world matrix.
	m_Direct3D->GetWorldMatrix(worldMatrix);


	// Set the terrain shader parameters that it will use for rendering.
	m_TerrainShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		m_Terrain->GetGrassTexture(), m_Terrain->GetSlopeTexture(), m_Terrain->GetRockTexture(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection());

	// Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// Render the terrain using the quad tree and terrain shader.
	m_QuadTree->Render(m_Frustum, m_Direct3D->GetDeviceContext(), m_TerrainShader);

	// Set the number of rendered terrain triangles since some were culled.
	result = m_Text->SetRenderCount(m_QuadTree->GetDrawCount(), m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

									/* ----------------------------- WATER ---------------------------------------*/

	m_Direct3D->TurnZBufferOn();	m_Direct3D->TurnOnAlphaBlending();

	// Reset the world matrix.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Translate to the location of the water and render it.
	D3DXMatrixTranslation(&worldMatrix, 0.0f, 1.0f/*1 is the water height*/, 0.0f);

	m_Camera->RenderReflection(1.0f);
	m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);

	//Render Water
	m_Water->Render(m_Direct3D->GetDeviceContext());

	//Render Water using Water Shader
	m_WaterShader->Render(m_Direct3D->GetDeviceContext(), m_Water->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, reflectionViewMatrix,
		m_RefractionTexture->GetShaderResourceView(), m_ReflectionTexture->GetShaderResourceView(), m_Water->GetTexture(),
		m_Camera->GetPosition(), m_Water->GetReflectRefractScale(),
		m_Water->GetRefractionTint(), m_Light->GetDirection(), m_Water->GetSpecularShininess(), elapsedTime);

	// Turn off the Z buffer.
	m_Direct3D->TurnZBufferOff();	m_Direct3D->TurnOffAlphaBlending();

									/* ------------------------------------------- MODELS ------------------------------------------ */

	//////////////////////////////////////////////////////////// VOLUMETRIC MODEL //////////////////////////////////////////////////////////////
	
	for (int i = 0; i < 10; ++i)
	{
		if (!volDest.at(i))
		{
			m_Direct3D->TurnOnAlphaBlending();

			m_Volumetric.at(i)->Render(m_Direct3D->GetDeviceContext());

			m_VolumetricShader->Render(m_Direct3D->GetDeviceContext(), m_Volumetric.at(i)->GetIndexCount(), m_Volumetric.at(i)->GetModelMatrix(), viewMatrix, projectionMatrix,
				m_Volumetric.at(i)->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), elapsedTime);

			// Turn off alpha blending after rendering the text.
			m_Direct3D->TurnOffAlphaBlending();
		}
	}

	/////////////////////////////////////////////////////////////// THROWABLE /////////////////////////////////////////////////////////////////
	ThrowKnives();

	//////////////////////////////////////////////////////////////// CHECK FOR COLLISION /////////////////////////////////////////////////////////
	//check for collision with each ball
	if (m_Input->IsFPressed())
	{
		for (int i = 0; i < 10; ++i)
		{
			if (m_Collision->DistanceCollision(m_Volumetric.at(i)->GetModelMatrix()))
			{
				volDest.at(i) = true;
			}
		}
	}

	return true;
}

void ApplicationClass::ThrowKnives()
{
	bool result, keyDown = false;

	D3DXMATRIX knifeViewMat, projectionMatrix;

	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Camera->Render();
	m_Camera->GetViewMatrix(knifeViewMat);

	//position the knife where it needs to be
	keyDown = m_Input->IsFPressed();
	m_Collision->KnifeHit(keyDown, knifeViewMat);

	m_Knives->Render(m_Direct3D->GetDeviceContext());
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Knives->GetIndexCount(), 
		m_Collision->GetKnifePos(), knifeViewMat, projectionMatrix,
		m_Knives->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result)
	{
		MessageBox(hwnd_APP, L"Could not render the knife object.", L"Error", MB_OK);
	}
}
