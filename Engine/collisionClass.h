#pragma once
#include "d3d11.h"
#include "D3DX10math.h"

#include <vector>

class CollisionClass
{
public:
	CollisionClass();
	CollisionClass(const CollisionClass&);
	~CollisionClass();

	void KnifeHit(bool keyDown, D3DXMATRIX viewMat);

	bool DistanceCollision(D3DXMATRIX ball);

	D3DXMATRIX GetKnifePos();
	D3DXMATRIX GetKnifeDir();

private:

	//transforms of ball and knife
	D3DXVECTOR3 knifeLoc, ballLoc;
	D3DXQUATERNION knifeRot, ballRot;
	D3DXVECTOR3 knifeScale, ballScale;

	//the knife
	D3DXMATRIX KnifeWorldPos;
	D3DXMATRIX KnifeDir;
	D3DXVECTOR3 tempKnifePos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
};

