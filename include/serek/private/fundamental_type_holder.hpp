#pragma once

#include <utility>
#include <type_traits>

namespace serek
{
	namespace detail
	{
		template<typename T>
		struct fundamental_type_holder
		{
			T value;

			operator T&() { return get(); }

			template<typename ... other_types>
			fundamental_type_holder(other_types&& ... argv) : value{ std::forward<other_types>(argv)... } {}

			template<typename other_t>
			inline friend auto operator<=>(const fundamental_type_holder me, const other_t& other) { return me.value <=> other; }

		private:
			T& get() { return value; }
			const T& get() const { return value; }

			static_assert( std::is_fundamental_v<T> );
		};
	}
}