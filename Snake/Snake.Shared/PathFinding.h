#pragma once
#include <DirectXMath.h>
#include <vector>
#include <functional>
#include <memory>
using namespace DirectX;
struct PathFindNode
{
	std::shared_ptr<PathFindNode> parent = nullptr;
	XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
	int weight = 0;
	int gWeight = 0;
	const bool operator > (const PathFindNode& node)
	{
		return this->weight > node.weight;
	}
	const bool operator < (const PathFindNode& node)
	{
		return this->weight < node.weight;
	}
	const bool operator == (const PathFindNode* node)
	{
		return this->position.x == node->position.x &&
			this->position.y == node->position.y &&
			this->position.z == node->position.z;
	}


	int GetgWeight(XMFLOAT3 start)
	{
		if (parent != nullptr)
		{
			return std::abs((int)(position.x - start.x)) + std::abs((int)(position.y - start.y)) + parent->gWeight;

		}
		else
		{
			return std::abs((int)(position.x - start.x)) + std::abs((int)(position.y - start.y));
		}
	}

	void CalculateWeight(XMFLOAT3 start, XMFLOAT3 goal)
	{
		gWeight = GetgWeight(start);
		int H = std::abs((int)(position.x - goal.x)) + std::abs((int)(position.y - goal.y));
		weight = gWeight + H;
	}
};

class PathFinding
{
public:
	PathFinding();
	std::shared_ptr<PathFindNode> FindPath(std::shared_ptr<PathFindNode> start, std::shared_ptr<PathFindNode> goal, std::function<bool(int, int)> isAvailable = true);

private:
	std::vector<std::shared_ptr<PathFindNode>> SurroundPoints(std::shared_ptr<PathFindNode> node, std::function<bool(int, int)> isAvailable = true);
	std::vector<std::shared_ptr<PathFindNode>>::iterator Contains(std::vector<std::shared_ptr<PathFindNode>> &nodes, std::shared_ptr<PathFindNode> node);
	std::vector<std::shared_ptr<PathFindNode>> m_openList;
	std::vector<std::shared_ptr<PathFindNode>> m_closeList;
};

