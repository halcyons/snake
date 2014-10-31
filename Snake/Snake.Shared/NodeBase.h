#pragma once
#include "BaseMath.h"
class NodeBase
{
public:
	NodeBase();
	NodeBase(float3 pos);
	virtual ~NodeBase();
	bool operator == (const NodeBase& node)
	{
		return position == node.position;
	}
	float3 position;
};

