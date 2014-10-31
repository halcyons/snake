#include "pch.h"
#include "SnakeClass.h"


SnakeClass::SnakeClass()
{
}


SnakeClass::~SnakeClass()
{
}


bool SnakeClass::IsCollideWithSnake(const DXSnakeNode& node, int snakeStartIndex, int snakeEndIndex)
{
	DirectX::BoundingBox tempBB(node.boundingBox.Center, 
		DirectX::XMFLOAT3(node.boundingBox.Extents.x / 2, node.boundingBox.Extents.y / 2, node.boundingBox.Extents.z / 2));
	std::deque<DXSnakeNode> snakeList = GetSnakeList();
	for (int i = snakeStartIndex; i < snakeEndIndex; i++)
	{
		DXSnakeNode tempNode = snakeList[i];
		if (tempNode.boundingBox.Contains(tempBB) == DirectX::ContainmentType::CONTAINS)
			return true;
	}
	return false;
}

bool SnakeClass::IsCollideWithBody()
{
	std::deque<DXSnakeNode> snakeList = GetSnakeList();
	return IsCollideWithSnake(snakeList.front(), 1, snakeList.size());
}

bool SnakeClass::IsCollideWithNode(const DXSnakeNode& node)
{
	DirectX::BoundingBox tempBB(node.boundingBox.Center,
		DirectX::XMFLOAT3(node.boundingBox.Extents.x / 2, node.boundingBox.Extents.y / 2, node.boundingBox.Extents.z / 2));
	DXSnakeNode tempNode = GetSnakeList()[0];
	if (tempNode.boundingBox.Contains(tempBB) == DirectX::ContainmentType::CONTAINS)
		return true;
	else
	{
		return false;
	}
}