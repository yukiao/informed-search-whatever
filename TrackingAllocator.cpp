#include "TrackingAllocator.hpp"

namespace tallocator
{

size_t count = 0;
size_t maxMem = 0;

void resetMaxMemory()
{
	maxMem = count;
}

}
