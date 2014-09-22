#include "pch.h"
#include "PathFinding.h"


bool PathFinding::FindPath(PathFindNode* start, PathFindNode* goal)
{
	for (PathFindNode* node : start->SurroundPoints())
	{
		m_openList.push_back(node);
	}
	m_closeList.push_back(start);

}
