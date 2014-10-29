#pragma once
#include "SnakeNode.h"
#include <deque>
enum Direction
{
	up = 1,
	right = 2,
	in = 3,
	down = -1,
	left = -2,
	out = -3
};
class SnakeBase
{
public:
	SnakeBase();
	virtual ~SnakeBase();
	int GetSize();
	std::deque<BaseNode> GetSnakeList();
	void Eat();
	void Move();
	void Move(const Direction& dir);
	bool IsCollideWithWall();
	bool IsCollideWithBody();
	bool IsCollideWithSnake(const BaseNode& node, int snakeStartIndex, int snakeEndIndex);
	bool IsCollideWithNode(const BaseNode& node);
	
private:
	void AddHeaderTo(const Direction& dir);
	void DelTail();
	std::deque<BaseNode>	m_snakeList;
	Direction				m_dir;	
	const float				Left = -11.f;
	const float				Right = 11.f;
	const float				Up = 11.f;
	const float				Down = -11.f;
	const float				In = 11.f;
	const float				Out = -11.f;
};

