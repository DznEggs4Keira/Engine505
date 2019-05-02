#include "collisionClass.h"



CollisionClass::CollisionClass()
{
}

CollisionClass::CollisionClass(const CollisionClass&)
{
}

CollisionClass::~CollisionClass()
{
}

void CollisionClass::KnifeHit(bool keyDown, D3DXMATRIX viewMat)
{
	D3DXMatrixIdentity(&KnifeWorldPos);
	D3DXMATRIX initialPos, initialRot, initialScale, cameraMat;
	float determ = 0.0f;

	if (keyDown)
	{
		//scale and rotate
		D3DXMatrixScaling(&initialScale, 0.1f, 0.1f, 0.1f);
		D3DXMatrixRotationYawPitchRoll(&initialRot, 20, 45, 0);
		D3DXMatrixMultiply(&initialRot, &initialScale, &initialRot);

		//initial translate
		D3DXMatrixTranslation(&initialPos, 0.0f, -1.0f, 10.0f);
		D3DXMatrixMultiply(&initialPos, &initialRot, &initialPos);
	}

	else if (!keyDown)
	{
		//scale and rotate
		D3DXMatrixScaling(&initialScale, 0.1f, 0.1f, 0.1f);
		D3DXMatrixRotationYawPitchRoll(&initialRot, -290, 0, 0);
		D3DXMatrixMultiply(&initialRot, &initialScale, &initialRot);

		//initial translate
		D3DXMatrixTranslation(&initialPos, -3.0f, -2.0f, 10.0f);
		D3DXMatrixMultiply(&initialPos, &initialRot, &initialPos);
	}

	//inverse the view matrix to get the camera's world matrix
	D3DXMatrixInverse(&cameraMat, &determ, &viewMat);

	//the knife moves with the camera
	D3DXMatrixMultiply(&KnifeWorldPos, &initialPos, &cameraMat);
}

bool CollisionClass::DistanceCollision(D3DXMATRIX ball)
{
	D3DXMatrixDecompose(&knifeScale, &knifeRot, &knifeLoc, &GetKnifePos());
	//float length = knifeLoc.y;
	float length = 5.0f; //5.0f
	
	D3DXMatrixDecompose(&ballScale, &ballRot, &ballLoc, &ball);
	//float radius = (ballLoc.x / 2.0f);
	float radius = 4.0f; //5.0f

	D3DXVECTOR3 subtract;

	D3DXVec3Subtract(&subtract, &ballLoc, &knifeLoc);

	float distance = D3DXVec3Length(&subtract);

	/*if the distance between the length of the knife and centre of the ball is less than the ball's
	radius then return true for collision*/
	
	if (distance <= radius + length)
	{
		return true;
	}

	return false;
}

D3DXMATRIX CollisionClass::GetKnifePos()
{
	return KnifeWorldPos;
}

D3DXMATRIX CollisionClass::GetKnifeDir()
{
	return KnifeDir;
}