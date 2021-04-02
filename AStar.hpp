#ifndef _ASTAR_HPP_
#define _ASTAR_HPP_

#include <functional>

#include "InformedBase.hpp"

class AStar: public InformedBase
{
public:
	AStar(GraphInfo *info);
	PathResult find(Vertex *a, Vertex *b, HF hf, int paramIndex = 0, bool greater = false) override;

private:
	GraphInfo *graph;
};

#endif
