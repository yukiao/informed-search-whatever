#ifndef _ADJACENT_HPP_
#define _ADJACENT_HPP_

#include <vector>

#include "Vertex.hpp"

struct Adjacent
{
	Vertex *target;
	std::vector<int> param;
};

#endif
