#pragma once
#include "d3d11.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include <vector>

class CollisionClass
{
public:
	CollisionClass();
	CollisionClass(const CollisionClass&);
	~CollisionClass();

	void KnifeHit(bool keyDown, DirectX::XMMATRIX viewMat);

	bool DistanceCollision(DirectX::XMMATRIX ball);

	DirectX::XMMATRIX GetKnifePos();
	DirectX::XMMATRIX GetKnifeDir();

private:

	//transforms of ball and knife
	DirectX::XMFLOAT3 knifeLoc, ballLoc;
	DirectX::XMVECTOR knifeRot, ballRot;
	DirectX::XMFLOAT3 knifeScale, ballScale;

	//the knife
	DirectX::XMMATRIX KnifeWorldPos;
	DirectX::XMMATRIX KnifeDir;
	DirectX::XMFLOAT3 tempKnifePos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
};

