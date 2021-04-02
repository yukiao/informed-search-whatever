#include <algorithm>

#include "AStar.hpp"

struct AStarComp
{
	GraphInfo *context;
	AStar::HF hf;
	Vertex *to;
	bool greater;

	AStarComp(GraphInfo *context, Vertex *to, AStar::HF hf, bool greater)
	: context(context)
	, hf(hf)
	, to(to)
	, greater(greater)
	{}

	inline double eval(PathResultInternal &r)
	{
		return r.cost + hf(context, r.paths.back(), to);
	}

	bool operator()(PathResultInternal &a, PathResultInternal &b)
	{
		if (greater)
			return eval(a) < eval(b);
		else
			return eval(a) > eval(b);
	}
};

AStar::AStar(GraphInfo *info)
: graph(info)
{}

PathResult AStar::find(Vertex *a, Vertex *b, HF hf, int paramIndex, bool greater)
{
	using prq = std::priority_queue<PathResultInternal, tallocator::tavector<PathResultInternal>, AStarComp>;
	prq queue {AStarComp(graph, b, hf, greater)};

	queue.push({{a}, 0, 0});

	while (!queue.empty())
	{
		PathResultInternal u = queue.top();
		queue.pop();

		if (u.paths.back() == b)
			return PathResult(u);

		for (Adjacent &adj: graph->adjacent[u.paths.back()])
		{
			if (std::find(u.paths.begin(), u.paths.end(), adj.target) == u.paths.end())
			{
				tallocator::tavector<Vertex*> paths = u.paths;
				paths.push_back(adj.target);
				queue.push({std::move(paths), u.expansion + 1, u.cost + adj.param[paramIndex]});
			}
		}
	}

	return nullptr;
}
