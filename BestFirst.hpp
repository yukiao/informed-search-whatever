#ifndef _BESTFIRST_HPP_
#define _BESTFIRST_HPP_

#include <functional>

#include "InformedBase.hpp"

class BestFirst: public InformedBase
{
public:
	BestFirst(GraphInfo *graph);
	PathResult find(Vertex *a, Vertex *b, HF hf, int paramIndex = 0, bool greater = false) override;
private:
	GraphInfo *graph;
};

#endif
