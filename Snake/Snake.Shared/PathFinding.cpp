#include "pch.h"
#include "PathFinding.h"
#include <algorithm>


bool compa(std::shared_ptr<PathFindNode> n1, std::shared_ptr<PathFindNode> n2)
{
	if (n1->weight == n2->weight)
	{
		return n1->position.x > n2->position.x;
	}
	else
	{
		return n1->weight > n2->weight;
	}
	
}

PathFinding::PathFinding()
{
	m_openList = std::vector<std::shared_ptr<PathFindNode>>();
	m_closeList = std::vector<std::shared_ptr<PathFindNode>>();
}

// Use reference type to prevent copying the parameter(nodes), so that the retured iterator is compatible.
std::vector<std::shared_ptr<PathFindNode>>::iterator PathFinding::Contains(std::vector<std::shared_ptr<PathFindNode>> &nodes, std::shared_ptr<PathFindNode> node)
{
	if (nodes.size() == 0)
	{
		return nodes.end();
	}
	auto it = std::find_if(nodes.begin(), nodes.end(), [=](std::shared_ptr<PathFindNode> tempNode){
		return tempNode->position.x == node->position.x &&
			tempNode->position.y == node->position.y &&
			tempNode->position.z == node->position.z;
	});


	return it;

}

std::vector<std::shared_ptr<PathFindNode>> PathFinding::SurroundPoints(std::shared_ptr<PathFindNode> node, std::function<bool(int, int)> isAvailable)
{
	std::vector<std::shared_ptr<PathFindNode>> nodes = std::vector<std::shared_ptr<PathFindNode>>();
	std::shared_ptr<PathFindNode> rightNode = std::make_shared<PathFindNode>();
	rightNode->position = XMFLOAT3(node->position.x + 1, node->position.y, node->position.z);
	if (Contains(m_closeList, rightNode) == m_closeList.end() && isAvailable((int)rightNode->position.x, (int)rightNode->position.y))
	{
		if (Contains(m_openList, rightNode) == m_openList.end())
		{
			rightNode->parent = node;
		}
		nodes.push_back(rightNode);
	}

	std::shared_ptr<PathFindNode> leftNode = std::make_shared<PathFindNode>();
	leftNode->position = XMFLOAT3(node->position.x - 1, node->position.y, node->position.z);
	if (Contains(m_closeList, leftNode) == m_closeList.end() && isAvailable((int)leftNode->position.x, (int)leftNode->position.y))
	{
		if (Contains(m_openList, leftNode) == m_openList.end())
		{
			leftNode->parent = node;
		}
		nodes.push_back(leftNode);
	}

	std::shared_ptr<PathFindNode> upNode = std::make_shared<PathFindNode>();

	upNode->position = XMFLOAT3(node->position.x, node->position.y + 1, node->position.z);
	if (Contains(m_closeList, upNode) == m_closeList.end() && isAvailable((int)upNode->position.x, (int)upNode->position.y))
	{
		if (Contains(m_openList, upNode) == m_openList.end())
		{
			upNode->parent = node;
		}
		nodes.push_back(upNode);
	}


	std::shared_ptr<PathFindNode> downNode = std::make_shared<PathFindNode>();

	downNode->position = XMFLOAT3(node->position.x, node->position.y - 1, node->position.z);
	if (Contains(m_closeList, downNode) == m_closeList.end() && isAvailable((int)downNode->position.x, (int)downNode->position.y))
	{
		if (Contains(m_openList, downNode) == m_openList.end())
		{
			downNode->parent = node;
		}
		nodes.push_back(downNode);
	}

	return nodes;
}

std::shared_ptr<PathFindNode> PathFinding::FindPath(std::shared_ptr<PathFindNode> start, std::shared_ptr<PathFindNode> goal, 
	std::function<bool(int, int)> isAvailable)
{
	std::shared_ptr<PathFindNode> tempNode = start;
	while (tempNode->position.x != goal->position.x ||
		tempNode->position.y != goal->position.y ||
		tempNode->position.z != goal->position.z)
	{
		m_closeList.push_back(tempNode);
		for (std::shared_ptr<PathFindNode> node : SurroundPoints(tempNode, isAvailable))
		{

			// Find that node is in the open list.
			if (Contains(m_openList, node) != m_openList.end())
			{
				auto it = Contains(m_openList, node);
				if (node->GetgWeight(tempNode->position) < node->gWeight)
				{
					(*it)->parent = tempNode;
					(*it)->gWeight = node->GetgWeight(tempNode->position);
					(*it)->CalculateWeight(tempNode->position, goal->position);
				}
			}
			else
			{
				node->CalculateWeight(tempNode->position, goal->position);
				m_openList.push_back(node);
			}
		}

		std::sort(m_openList.begin(), m_openList.end(), compa);
		// The node whose weight is the smallest.
		std::shared_ptr<PathFindNode> node = *(m_openList.end() - 1);
		m_openList.erase(m_openList.end() - 1);
		tempNode = node;
		// Not find the path.
		if (m_openList.size() == 0)
		{
			return nullptr;
		}
	}
	return tempNode;
}
