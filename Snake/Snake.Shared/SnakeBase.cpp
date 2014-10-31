#include "pch.h"
//#include "SnakeBase.h"
//
//
//Base::Base() :m_dir(Direction::up)
//{
//	/*NodeBase node = NodeBase(float3(0.f, 0.f, 0.f));
//	m_snakeList.push_front(node);
//	AddHeaderTo(m_dir);
//	AddHeaderTo(m_dir);
//	AddHeaderTo(m_dir);
//	AddHeaderTo(m_dir);
//	m_dir = Direction::right;
//	AddHeaderTo(m_dir);
//	AddHeaderTo(m_dir);
//	AddHeaderTo(m_dir);
//	AddHeaderTo(m_dir);
//	m_dir = Direction::down;
//	AddHeaderTo(m_dir);
//	m_dir = Direction::left;
//	AddHeaderTo(m_dir);
//	AddHeaderTo(m_dir);
//	AddHeaderTo(m_dir);*/
//}
//
//
//
//
//
//
//void SnakeBase::Move()
//{
//	Move(m_dir);
//}
//
//void SnakeBase::Move(const Direction& dir)
//{
//	AddHeaderTo(dir);
//	DelTail();
//	m_dir = dir;
//}
//
//void SnakeBase::Eat()
//{
//	AddHeaderTo(m_dir);
//}
//
//bool SnakeBase::IsCollideWithWall()
//{
//	float x = m_snakeList.front().position.x;
//	float y = m_snakeList.front().position.y;
//	float z = m_snakeList.front().position.z;
//	if (x > Right || x < Left 
//		|| y > Up || y < Down
//		|| z > In || z < Out)
//	{
//		return true;
//	}
//	return false;
//}
//
//bool SnakeBase::IsCollideWithBody()
//{
//	//float3 header = .position;
//	return IsCollideWithSnake(m_snakeList.front(), 1, m_snakeList.size());
//	
//}
//
//bool SnakeBase::IsCollideWithSnake(const NodeBase& node, int snakeStartIndex, int snakeEndIndex)
//{
//	for (int i = snakeStartIndex; i < snakeEndIndex; i++)
//	{
//		NodeBase tempNode = m_snakeList[i];
//		if (tempNode == node)
//		{
//			return true;
//		}
//	}
//	return false;
//}
//
//bool SnakeBase::IsCollideWithNode(const NodeBase& node)
//{
//	if (m_snakeList.front() == node)
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}