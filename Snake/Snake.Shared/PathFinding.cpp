#include "pch.h"
#include "PathFinding.h"

bool compa(PathFindNode* n1, PathFindNode* n2)
{
	return n1->weight > n2->weight;
}

bool PathFinding::FindPath(PathFindNode* start, PathFindNode* goal)
{
	for (PathFindNode* node : start->SurroundPoints())
	{
		node->CalculateWeight(start->position, goal->position);
		auto close = std::find(m_closeList.begin, m_closeList.end, node);
		auto open = std::find(m_openList.begin, m_openList.end, node);
		// Find node in the open list.
		if (open != m_openList.end)
		{
			if (node->GetgWeightThrough(m_closeList[0]->position, start->position) < node->GetgWeight(m_closeList[0]->position))
			{

			}
		}
		// Can't find node in close list.
		if (close == m_closeList.end)
		{
			m_openList.push_back(node);
		}
		
	}
	m_closeList.push_back(start);
	std::sort(m_openList.begin(), m_openList.end(), compa);
	PathFindNode* node = *(m_openList.end() - 1);
	m_openList.pop_back();
	FindPath(node, goal);	
}
