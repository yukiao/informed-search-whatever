#ifndef _PATHRESULT_HPP_
#define _PATHRESULT_HPP_

#include "TrackingAllocator.hpp"
#include "Vertex.hpp"

struct PathResultInternal
{
	tallocator::tavector<Vertex*> paths;
	size_t expansion;
	int cost;
};

struct PathResult
{
	std::vector<Vertex*> paths;
	size_t expansion;
	int cost;

	PathResult() = default;
	PathResult(const PathResult &) = default;
	PathResult(PathResult &&) = default;
	explicit PathResult(const PathResultInternal &i);
	PathResult(std::nullptr_t _);
	PathResult &operator=(const PathResult &) = default;
};

#endif
