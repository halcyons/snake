#pragma once
struct PathFindNode
{
	PathFindNode* parent = nullptr;
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
	const bool operator == (const PathFindNode& node)
	{
		return this->weight == node.weight;
	}
	std::vector<PathFindNode*> SurroundPoints()
	{
		std::vector<PathFindNode*> nodes;
		PathFindNode* rightNode = new PathFindNode();
		rightNode->parent = this;
		rightNode->position = XMFLOAT3(this->position.x + 1, this->position.y, this->position.z);
		nodes.push_back(rightNode);

		PathFindNode* leftNode = new PathFindNode();
		leftNode->parent = this;
		leftNode->position = XMFLOAT3(this->position.x - 1, this->position.y, this->position.z);
		nodes.push_back(leftNode);

		PathFindNode* upNode = new PathFindNode();
		upNode->parent = this;
		upNode->position = XMFLOAT3(this->position.x, this->position.y + 1, this->position.z);
		nodes.push_back(upNode);

		PathFindNode* downNode = new PathFindNode();
		downNode->parent = this;
		downNode->position = XMFLOAT3(this->position.x, this->position.y - 1, this->position.z);
		nodes.push_back(downNode);
		
		return nodes;
	}

	int GetgWeightThrough(XMFLOAT3 start, XMFLOAT3 midPt)
	{
		return std::abs((int)(midPt.x - start.x)) + std::abs((int)(midPt.y - start.y))
			+ std::abs((int)(position.x - midPt.x)) + std::abs((int)(position.y - midPt.y));
	}

	int GetgWeight(XMFLOAT3 start)
	{
		return std::abs((int)(position.x - start.x)) + std::abs((int)(position.y - start.y));
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
	static bool FindPath(PathFindNode* start, PathFindNode* goal);

private:
	static std::vector<PathFindNode*> m_openList;
	static std::vector<PathFindNode*> m_closeList;
};

