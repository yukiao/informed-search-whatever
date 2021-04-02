#include "BestFirst.hpp"

struct EvaluationComparator
{
	GraphInfo *context;
	BestFirst::HF hf;
	Vertex *to;
	bool greater;

	EvaluationComparator(GraphInfo *context, Vertex *to, BestFirst::HF hf, bool greater)
	: context(context)
	, hf(hf)
	, to(to)
	, greater(greater)
	{}

	bool operator()(PathResultInternal &a, PathResultInternal &b)
	{
		if (greater)
			return hf(context, a.paths.back(), to) < hf(context, b.paths.back(), to);
		else
			return hf(context, a.paths.back(), to) > hf(context, b.paths.back(), to);
	}
};

BestFirst::BestFirst(GraphInfo *graph)
: graph(graph)
{}

PathResult BestFirst::find(Vertex *a, Vertex *b, HF hf, int paramIndex, bool greater)
{
	using prq = std::priority_queue<PathResultInternal, tallocator::tavector<PathResultInternal>, EvaluationComparator>;
	prq queue {EvaluationComparator(graph, b, hf, greater)};
	tallocator::taset<Vertex*> visited {a};

	queue.push({{a}, 0, 0});

	while (!queue.empty())
	{
		PathResultInternal u = queue.top();
		queue.pop();

		if (u.paths.back() == b)
			return PathResult(u);

		for (Adjacent &adj: graph->adjacent[u.paths.back()])
		{
			if (visited.find(adj.target) == visited.end())
			{
				tallocator::tavector<Vertex*> paths = u.paths;
				paths.push_back(adj.target);

				visited.insert(adj.target);
				queue.push({std::move(paths), u.expansion + 1, u.cost + adj.param[paramIndex]});
			}
		}
	}

	return nullptr;
}
