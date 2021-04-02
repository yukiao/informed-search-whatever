#ifndef _GRAPH_INFO_HPP_
#define _GRAPH_INFO_HPP_

#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

#include "Vertex.hpp"
#include "Adjacent.hpp"

struct GraphInfo
{
	size_t width, height, nparam;

	std::vector<Vertex> vertex;
	std::unordered_map<Vertex*, std::vector<Adjacent>> adjacent;
	std::unordered_map<Vertex*, std::pair<double, double>> coordinates;
	std::unordered_map<std::string, Vertex*> nameLookup;

	static GraphInfo parse(const std::string &data);
};

#endif
