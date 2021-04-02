#include <cmath>
#include <cstdio>

#include <chrono>
#include <sstream>
#include <system_error>

#include "AStar.hpp"
#include "BestFirst.hpp"
#include "GraphInfo.hpp"

struct Parameters
{
	std::string name, a, b;
	std::function<double(GraphInfo*, Vertex*, Vertex*)> hf;
	int paramIndex;
	bool greater;
};

static double heuristicSLD(GraphInfo *graph, Vertex *a, Vertex *b)
{
	std::pair<double, double> acoord = graph->coordinates[a], bcoord = graph->coordinates[b];
	return sqrt(pow(acoord.first - bcoord.first, 2) + pow(acoord.second - bcoord.second, 2));
}

static double heuristicSLDRomaniaTest(GraphInfo *graph, Vertex *a, Vertex *b)
{
	return floor(heuristicSLD(graph, a, b) / 1.5559335655336 + 0.5);
}

static double heuristicDangerous(GraphInfo *graph, Vertex *a, Vertex *b)
{
	// Let "A" and "B" be the adjacent of vertex "a" and "b" respectively.
	// Let s(x, y) be the function that detrmines dangerous level of connected node x and y
	//
	// d = SLD(a, b)
	// d_a = (Sum of: for every a' in "A": SLD(a, a')) / n(A)
	// d_b = (Sum of: for every b' in "B": SLD(b, b')) / n(B)
	// s_a = (Sum of: for every a' in "A": s(a, a')) / n(A)
	// s_b = (Sum of: for every b' in "B": s(b, b')) / n(B)
	// result = (s_a + s_b) * d / (d_a + d_b)
	double dist = heuristicSLD(graph, a, b);
	double asafety = 0, bsafety = 0;
	double adist = 0, bdist = 0;

	for (Adjacent &adj: graph->adjacent[a])
	{
		asafety += adj.param[1];
		adist += heuristicSLD(graph, a, adj.target);
	}
	for (Adjacent &adj: graph->adjacent[b])
	{
		bsafety += adj.param[1];
		bdist += heuristicSLD(graph, a, adj.target);
	}

	//return ((asafety + bsafety) / 2) * dist / ((adist + bdist) / 2);
	return (asafety + bsafety) * dist / (adist + bdist);
}

static void printPaths(PathResult &r, size_t mem, double t)
{
	if (r.cost == -1 && r.expansion == size_t(-1))
		puts("Not found.");
	else
	{
		std::stringstream ss;

		for (size_t i = 0; i < r.paths.size() - 1; ss << r.paths[i++]->operator std::string() << " --> ");
		ss << r.paths.back()->operator std::string();

		puts(ss.str().c_str());
	}

	printf("%.2f ns, %zu bytes, %d, %zu\n\n", t, mem, r.cost, r.expansion);
}

static void benchmark(const char *filename, std::vector<Parameters> &params, size_t runs)
{
	using Algo = std::pair<std::string, std::function<InformedBase*(GraphInfo*)>>;
	struct FILEWrap
	{
		FILE *f;
		FILEWrap(const char *name, const char *mode)
		{
			f = fopen(name, mode);
			if (f == nullptr)
				throw std::system_error(errno, std::generic_category());
		}
		~FILEWrap() {fclose(f);}
		operator FILE*() {return f;}
	};

	static std::vector<Algo> constructor = {
		{"Best-First Search", [](GraphInfo *info) -> InformedBase* {
			return new BestFirst(info);
		}},
		{"A* Search", [](GraphInfo *info) -> InformedBase* {
			return new AStar(info);
		}},
	};
	
	FILEWrap f(filename, "rb");
	if (fseek(f, 0, SEEK_END))
		throw std::system_error(errno, std::generic_category());

	std::vector<char> buffer;
	buffer.resize(ftell(f));

	if (fseek(f, 0, SEEK_SET))
		throw std::system_error(errno, std::generic_category());
	if (fread(buffer.data(), 1, buffer.size(), f) != buffer.size())
		throw std::system_error(errno, std::generic_category());
	
	GraphInfo graph = GraphInfo::parse(std::string(buffer.begin(), buffer.end()));

	for (Algo &algo: constructor)
	{
		std::unique_ptr<InformedBase> search{algo.second(&graph)};

		for (Parameters &param: params)
		{
			Vertex *va = graph.nameLookup[param.a];
			Vertex *vb = graph.nameLookup[param.b];
			size_t maxMemory = 0;
			std::chrono::duration<double> duration = std::chrono::duration<double>::zero();

			PathResult result;

			printf("%s - %s from \"%s\" to \"%s\"\n", algo.first.c_str(), param.name.c_str(), param.a.c_str(), param.b.c_str());

			for (size_t i = 0; i < runs; i++)
			{
				tallocator::resetMaxMemory();
				auto start = std::chrono::high_resolution_clock::now();
				result = search->find(va, vb, param.hf, param.paramIndex, param.greater);
				duration += (std::chrono::high_resolution_clock::now() - start);
				maxMemory += tallocator::maxMem;
			}

			printPaths(result, maxMemory / runs, (duration.count() * 1000000000.0) / double(runs));
		}
	}
}

int main(int argc, char *argv[])
{
	size_t runs = 1;
	try
	{
		if (argc >= 2)
			runs = std::stoul(argv[1]);

		static std::vector<Parameters> teyvatTest = {
			// name, vertexA, vertexB, heuristicFunc, paramIndex, largest?
			{"Shortest Path",  "Mondstadt",     "Dawn Winery",        heuristicSLD,       0, false},
			//{"Longest Path",   "Mondstadt",     "Dawn Winery",        heuristicSLD,       0, true},
			{"Safest Path",    "Mondstadt",     "Dawn Winery",        heuristicDangerous, 1, false},
			//{"Dangerous Path", "Mondstadt",     "Dawn Winery",        heuristicDangerous, 1, true},
			{"Shortest Path",  "Starfell Lake", "Stormterror's Lair", heuristicSLD,       0, false},
			//{"Longest Path",   "Starfell Lake", "Stormterror's Lair", heuristicSLD,       0, true},
			{"Safest Path",    "Starfell Lake", "Stormterror's Lair", heuristicDangerous, 1, false},
			//{"Dangerous Path", "Starfell Lake", "Stormterror's Lair", heuristicDangerous, 1, true},
		};
		benchmark("teyvat.txt", teyvatTest, runs);

		std::unordered_map<std::string, double> romaniaDistAradBucharest = {
			{"Arad", 366},
			{"Bucharest", 0},
			{"Craiova", 160},
			{"Dobreta", 242},
			{"Eforie", 161},
			{"Fagaras", 176},
			{"Giurgiu", 77},
			{"Hirsowa", 151},
			{"Lasi", 226},
			{"Lugoj", 244},
			{"Mehadia", 241},
			{"Neamt", 234},
			{"Oradea", 380},
			{"Pitesti", 100},
			{"Rimnicu Vilcea", 193},
			{"Sibiu", 253},
			{"Timisoara", 329},
			{"Urziceni", 80},
			{"Vaslui", 199},
			{"Zerind", 374}
		};
		static auto heuristicSLDRomania = [&romaniaDistAradBucharest](GraphInfo *info, Vertex *a, Vertex *b)
		{
			// Assume b = Bucharest
			return double(romaniaDistAradBucharest[a->operator std::string()]);
		};

		static std::vector<Parameters> romaniaTest = {
			{"Shortest Path", "Arad", "Bucharest", heuristicSLDRomania, 0, false},
			//{"Longest Path", "Arad", "Bucharest", heuristicSLDRomania, 0, true},
		};
		benchmark("romania.txt", romaniaTest, runs);

		printf("Runs: %zu\n", runs);

		return 0;
	}
	catch (std::exception &e)
	{
		fprintf(stderr, "%s\n", e.what());
		return 1;
	}

	return 0;
}
