#pragma once

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


template <typename T>
class SnakeBase
{
public:
	SnakeBase() :m_dir(Direction::up)
	{
		T node = T(float3(0.f, 0.f, 0.f));
		m_snakeList.push_front(node);
		AddHeaderTo(m_dir);
		AddHeaderTo(m_dir);
		AddHeaderTo(m_dir);
		AddHeaderTo(m_dir);
		m_dir = Direction::right;
		AddHeaderTo(m_dir);
		AddHeaderTo(m_dir);
		AddHeaderTo(m_dir);
		AddHeaderTo(m_dir);
		m_dir = Direction::down;
		AddHeaderTo(m_dir);
		m_dir = Direction::left;
		AddHeaderTo(m_dir);
		AddHeaderTo(m_dir);
		AddHeaderTo(m_dir);
	}
	virtual ~SnakeBase()
	{

	}
	int GetSize()
	{
		return static_cast<int>(m_snakeList.size());
	}
	std::deque<T> GetSnakeList()
	{
		return m_snakeList;
	}
	bool IsCollideWithWall()
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
	virtual bool IsCollideWithBody()
	{
		return IsCollideWithSnake(m_snakeList.front(), 1, m_snakeList.size());
	}
	virtual bool IsCollideWithSnake(const T& node, int snakeStartIndex, int snakeEndIndex)
	{
		for (int i = snakeStartIndex; i < snakeEndIndex; i++)
		{
			T tempNode = m_snakeList[i];
			if (tempNode == node)
			{
				return true;
			}
		}
		return false;
	}
	virtual bool IsCollideWithNode(const T& node)
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
	void Eat()
	{
		AddHeaderTo(m_dir);
	}
	void Move()
	{
		Move(m_dir);
	}
	void Move(const Direction& dir)
	{
		AddHeaderTo(dir);
		DelTail();
		m_dir = dir;
	}
	
private:
	std::deque<T>			m_snakeList = std::deque<T>();

	void AddHeaderTo(const Direction& dir)
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
			m_snakeList.push_front(T(float3(x, y + 1.0f, z)));
			break;
		case right:
			m_snakeList.push_front(T(float3(x + 1.0f, y, z)));
			break;
		case down:
			m_snakeList.push_front(T(float3(x, y - 1.0f, z)));
			break;
		case left:
			m_snakeList.push_front(T(float3(x - 1.0f, y, z)));
			break;
		case in:
			m_snakeList.push_front(T(float3(x, y, z + 1.0f)));
			break;
		case out:
			m_snakeList.push_front(T(float3(x, y, z - 1.0f)));
			break;
		default:
			break;
		}
	}
	void DelTail()
	{
		m_snakeList.pop_back();
	}
	Direction				m_dir;
	const float				Left = -11.f;
	const float				Right = 11.f;
	const float				Up = 11.f;
	const float				Down = -11.f;
	const float				In = 11.f;
	const float				Out = -11.f;
};

