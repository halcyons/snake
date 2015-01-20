#pragma once
#include "NodeBase.h"
#include "DirectXCollision.h"

class DXSnakeNode :public NodeBase
{
public:
	DXSnakeNode();
	DXSnakeNode(float3 pos);
	DXSnakeNode(XMFLOAT3 extents);
	~DXSnakeNode();

	DirectX::BoundingBox boundingBox;
};