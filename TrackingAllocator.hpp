#ifndef _TRACKING_ALLOCATOR_HPP_
#define _TRACKING_ALLOCATOR_HPP_

#include <memory>
#include <set>
#include <queue>
#include <unordered_map>
#include <vector>

namespace tallocator
{

extern size_t count, maxMem;

void resetMaxMemory();

template<typename T>
struct talloc
{
	using value_type = T;

	constexpr talloc() noexcept {}
    constexpr talloc(const talloc&) noexcept = default;
    template <class U>
    constexpr talloc(const talloc<U>&) noexcept {}

	T *allocate(size_t count, const void *hint = 0)
	{
		T *result = std::allocator<T>().allocate(count, hint);
		if (result)
		{
			tallocator::count += sizeof(T) * count;
			tallocator::maxMem = std::max(tallocator::count, tallocator::maxMem);
		}

		return result;
	}

	void deallocate(T *ptr, size_t count)
	{
		std::allocator<T>().deallocate(ptr, count);
		tallocator::count -= sizeof(T) * count;
	}
};

template<typename T, typename C = std::less<T>> using taset = std::set<T, C, talloc<T>>;
template<typename T> using tavector = std::vector<T, talloc<T>>;
template<typename T> using taqueue = std::queue<T, std::deque<T, talloc<T>>>;
template<typename Key, typename Value, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>>
using taumap = std::unordered_map<Key, Value, Hash, Pred, talloc<std::pair<Key, Value>>>;

}

#endif
