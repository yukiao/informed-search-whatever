#include "Vertex.hpp"

Vertex::Vertex(const std::string &name)
: name(name)
{}

Vertex::operator std::string() const
{
	return name;
}

bool Vertex::operator==(const Vertex &v) const noexcept
{
	return v.name == name;
}
