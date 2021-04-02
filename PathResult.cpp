#include "PathResult.hpp"

PathResult::PathResult(const PathResultInternal &i)
: paths(i.paths.begin(), i.paths.end())
, expansion(i.expansion)
, cost(i.cost)
{}

PathResult::PathResult(std::nullptr_t _)
: paths()
, expansion(-1)
, cost(-1)
{}
