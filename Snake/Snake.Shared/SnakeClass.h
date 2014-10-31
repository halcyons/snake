#pragma once
#include "SnakeBase.h"
#include "DXSnakeNode.h"

class SnakeClass : public SnakeBase<DXSnakeNode>
{
public:
	SnakeClass();
	~SnakeClass();

	virtual bool IsCollideWithSnake(const DXSnakeNode& node, int snakeStartIndex, int snakeEndIndex);
	virtual bool IsCollideWithBody();
	virtual bool IsCollideWithNode(const DXSnakeNode& node);
};

