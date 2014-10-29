#include "pch.h"
#include "SnakeBase.h"


SnakeBase::SnakeBase() :m_dir(Direction::up)
{
	m_snakeList = std::deque<BaseNode>();
	BaseNode node = BaseNode(float3(0.f, 0.f, 0.f));
	m_snakeList.push_front(node);
	AddHeaderTo(m_dir);
	AddHeaderTo(m_dir);
}

SnakeBase::~SnakeBase()
{
}

int SnakeBase::GetSize()
{
	return static_cast<int>(m_snakeList.size());
}

std::deque<BaseNode> SnakeBase::GetSnakeList()
{
	return m_snakeList;
}

void SnakeBase::AddHeaderTo(const Direction& dir)
{
	if (dir == static_cast<Direction>(-m_dir))
	{
		return;
	}
	float x = m_snakeList.front().position.x;
	float y = m_snakeList.front().position.y;
	float z = m_snakeList.front().position.z;
	switch (dir)
	{
	case up:
		m_snakeList.push_front(BaseNode(float3(x, y + 1.0f, z)));
		break;
	case right:
		m_snakeList.push_front(BaseNode(float3(x + 1.0f, y, z)));
		break;
	case down:
		m_snakeList.push_front(BaseNode(float3(x, y - 1.0f, z)));
		break;
	case left:
		m_snakeList.push_front(BaseNode(float3(x - 1.0f, y, z)));
		break;
	case in:
		m_snakeList.push_front(BaseNode(float3(x, y, z + 1.0f)));
		break;	
	case out:
		m_snakeList.push_front(BaseNode(float3(x, y, z - 1.0f)));
		break;
	default:
		break;
	}	
}

void SnakeBase::DelTail()
{
	m_snakeList.pop_back();
}

void SnakeBase::Move()
{
	Move(m_dir);
}

void SnakeBase::Move(const Direction& dir)
{
	AddHeaderTo(dir);
	DelTail();
	m_dir = dir;
}

void SnakeBase::Eat()
{
	AddHeaderTo(m_dir);
}

bool SnakeBase::IsCollideWithWall()
{
	float x = m_snakeList.front().position.x;
	float y = m_snakeList.front().position.y;
	float z = m_snakeList.front().position.z;
	if (x > Right || x < Left 
		|| y > Up || y < Down
		|| z > In || z < Out)
	{
		return true;
	}
	return false;
}

bool SnakeBase::IsCollideWithBody()
{
	//float3 header = .position;
	return IsCollideWithSnake(m_snakeList.front(), 1, m_snakeList.size());
	
}

bool SnakeBase::IsCollideWithSnake(const BaseNode& node, int snakeStartIndex, int snakeEndIndex)
{
	for (int i = snakeStartIndex; i < snakeEndIndex; i++)
	{
		BaseNode tempNode = m_snakeList[i];
		if (tempNode == node)
		{
			return true;
		}
	}
	return false;
}

bool SnakeBase::IsCollideWithNode(const BaseNode& node)
{
	if (m_snakeList.front() == node)
	{
		return true;
	}
	else
	{
		return false;
	}
}