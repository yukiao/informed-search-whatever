#ifndef _INFORMED_BASE_HPP_
#define _INFORMED_BASE_HPP_

#include <functional>

#include "GraphInfo.hpp"
#include "PathResult.hpp"
#include "Vertex.hpp"

class InformedBase
{
public:
	using HF = std::function<double(GraphInfo*, Vertex*, Vertex*)>;

	virtual PathResult find(Vertex *a, Vertex *b, HF hf, int paramIndex = 0, bool greater = false) = 0;
	virtual ~InformedBase();
};

#endif
