#pragma once
struct PathFindNode
{
	PathFindNode* parent = nullptr;
	XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
	int weight = 0;
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

	void CalculateWeight(XMFLOAT3 goal)
	{

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

