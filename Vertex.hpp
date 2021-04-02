#ifndef _NP_VERTEX_H_
#define _NP_VERTEX_H_

#include <string>

struct Vertex
{
	Vertex(const std::string &name);
	inline Vertex(const char *name): Vertex(std::string(name)) {}

	operator std::string() const;
	bool operator==(const Vertex &v) const noexcept;

private:
	std::string name;
};

#endif
