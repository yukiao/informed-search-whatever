#include <list>
#include <set>

#include "GraphInfo.hpp"

struct EdgesInfo
{
	std::string a, b;
	std::vector<int> params;

	EdgesInfo(const std::string &a, const std::string &b, std::vector<int> c): a(a), b(b), params(c) {}
};

struct Point
{
	double x, y;
};

struct VertexWrapper
{
	Vertex v;
	Point coord;
};

static std::list<std::string> split(
	const std::string &text,
	const std::string &d,
	std::string(*filter)(std::string *) = nullptr
)
{
	size_t last = 0;
	std::list<std::string> result;

	while (last < text.length())
	{
		size_t s = text.find(d, last);
		std::string temp = text.substr(last, s - last);

		if (filter)
			temp = filter(&temp);
		
		result.push_back(std::move(temp));
		last = (s == std::string::npos ? text.length() : s) + d.length();
	}

	return result;
}

static std::string trim(const std::string &str)
{
	size_t start = 0, end = str.length();

	for (; start < end && str[start] == ' '; start++);
	for (; end != size_t(-1) && str[end - 1] == ' '; end--);

	return str.substr(start, end - start);
}

static std::pair<std::string, std::string> parseSingle(const std::string &str, char d, bool trimValue = false)
{
	size_t s = str.find(d);

	if (s == std::string::npos)
		throw std::runtime_error("unable to parse key-value " + str);
	
	if (trimValue)
		return {str.substr(0, s), trim(str.substr(s + 1))};
	else
		return {str.substr(0, s), str.substr(s + 1)};
}

GraphInfo GraphInfo::parse(const std::string &data)
{
	if (data.substr(0, 19) != "Teyvat Graph Format")
		throw std::runtime_error("invalid header");

	std::list<std::string> lines = split(data, "\n", [](std::string *str)
	{
		return str->back() == '\r' ? str->substr(0, str->length() - 1) : *str;
	});
	lines.pop_front(); // remove the header

	std::list<VertexWrapper> tempVertex;
	std::set<std::string> definedVertex;
	std::list<EdgesInfo> edgesData;
	GraphInfo result;
	result.width = 0;
	result.height = 0;
	result.nparam = size_t(-1);

	char mode = 0; // 1 = info, 2 = vertex, 3 = edge

	for (std::string &line: lines)
	{
		if (line.length() == 0 || line.front() == '#')
			continue;

		if (line == "Info:")
		{
			if (mode != 0)
				throw std::runtime_error("unexpected Info data");
			mode = 1;
		}
		else
		{
			if (mode == 0)
				throw std::runtime_error("Info data is required");
			
			if (line == "Vertex:")
				mode = 2;
			else if (line == "Edges:")
				mode = 3;
			else if (mode == 1)
			{
				// Info data
				std::pair<std::string, std::string> kv = parseSingle(line, ':', true);

				if (kv.first == "Resolution" && result.width == 0 && result.height == 0)
				{
					std::pair<std::string, std::string> resolution = parseSingle(kv.second, 'x');
					result.width = std::stoul(resolution.first);
					result.height = std::stoul(resolution.second);
				}
				else if (kv.first == "Parameter" && result.nparam == size_t(-1))
					result.nparam = std::stoul(kv.second);
				else
					throw std::runtime_error("Info unexpected " + line);
			}
			else if (mode == 2)
			{
				// Vertex data
				std::pair<std::string, std::string> kv = parseSingle(line, ':', true);

				if (definedVertex.find(kv.second) == definedVertex.end())
				{
					std::pair<std::string, std::string> coord = parseSingle(kv.first, 'x');
					//Point p = {std::stod(coord.first), std::stod(coord.second)};

					tempVertex.push_back({kv.second, {std::stod(coord.first), std::stod(coord.second)}});
					definedVertex.insert(kv.second);
				}
				else
					throw std::runtime_error("Duplicate vertex " + kv.second);
			}
			else if (mode == 3)
			{
				// Edges data
				std::list<std::string> edge = split(line, " - ");
				std::vector<int> params;
				if (edge.size() < 2)
					throw std::runtime_error("unexpected edge data " + line);
				
				if (edge.size() >= 3)
				{
					for (std::string &strs: split(*++++edge.begin(), " "))
					{
						if (strs.length() > 0)
							params.push_back(std::stol(strs));
					}
				}

				if (params.size() < result.nparam)
					throw std::runtime_error("insufficient parameter data " + line);
				
				edgesData.emplace_back(*edge.begin(), *++edge.begin(), params);
			}
		}
	}

	// Allocate vertex vectors
	result.vertex.reserve(tempVertex.size());

	// Copy vertex vectors
	for (VertexWrapper &vw: tempVertex)
	{
		std::string name = vw.v;

		result.vertex.push_back(vw.v);

		// Should safe to use the pointer because we've reserved the capacity
		Vertex *key = &result.vertex.back();
		result.nameLookup[name] = key;
		result.coordinates[key] = {vw.coord.x, vw.coord.y};
	}

	// Free memory
	tempVertex.clear();

	// Create adjacency list
	for (EdgesInfo &ei: edgesData)
	{
		std::string a = trim(ei.a);
		Vertex *va = result.nameLookup[a];
		std::string b = trim(ei.b);
		Vertex *vb = result.nameLookup[b];

		if (result.adjacent.find(va) == result.adjacent.end())
			result.adjacent[va] = {};
		if (result.adjacent.find(vb) == result.adjacent.end())
			result.adjacent[vb] = {};
		
		result.adjacent[va].push_back({vb, ei.params});
		result.adjacent[vb].push_back({va, ei.params});
	}

	return result;
}
