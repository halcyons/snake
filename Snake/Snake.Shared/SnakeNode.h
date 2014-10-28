#pragma once
#include "BaseMath.h"
class BaseNode
{
public:
	BaseNode();
	BaseNode(float3 pos);
	bool operator == (const BaseNode& node)
	{
		return position == node.position;
	}
	float3 position;
};

class SnakeNode :public BaseNode
{
	//virtual void SetCoordinate(float3) override;
};