#pragma once
#include <utility>

namespace mix {
namespace internal {

template<typename Listener, typename CallbackMember, typename... Args>
void InvokeListener(
	Listener* listener,
	CallbackMember callback,
	Args&&... args)
{
	if (listener)
	{
		(listener->*callback)(std::forward<Args>(args)...);
	}
}

} // namespace internal
} // namespace mix
