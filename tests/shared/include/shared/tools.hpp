#pragma once

#include <serek/serek.hpp>
#include <boost/ut.hpp>

namespace serek
{
	namespace tests
	{
		template<size_t _N>
		struct number_t
		{
			static inline constexpr size_t N = _N;
			inline friend std::ostream& operator<<(std::ostream& os, const number_t&) { return os << number_t::N; }
		};
	}	 // namespace tests
}	 // namespace serek

namespace but = boost::ut;
using namespace but::literals;

int main();