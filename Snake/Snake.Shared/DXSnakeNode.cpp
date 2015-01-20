#include "pch.h"
#include "DXSnakeNode.h"
#include <directxmath.h>
DXSnakeNode::DXSnakeNode() : boundingBox(DirectX::BoundingBox(DirectX::XMFLOAT3(0.f, 0.f, 0.f), DirectX::XMFLOAT3(0.f, 0.f, 0.f)))
{
	boundingBox.Center = DirectX::XMFLOAT3(position.x, position.y, position.z);
}

DXSnakeNode::DXSnakeNode(float3 pos) : NodeBase(pos) // !!!Note: Must call the base class' constructor in initializer list.
{
	// Can not call the constructor at here.
	// NodeBase(pos);
	boundingBox.Center = DirectX::XMFLOAT3(position.x, position.y, position.z);
}

DXSnakeNode::DXSnakeNode(DirectX::XMFLOAT3 extents)
{
	boundingBox.Extents = extents;
}

DXSnakeNode::~DXSnakeNode()
{

}