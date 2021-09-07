/**
 * @file tools.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Header file for usefull tools for testing
 * @version 0.1
 * @date 2021-08-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <serek/serek.hpp>
#include <boost/ut.hpp>

namespace serek
{
	/**
	 * @brief Contatins shared structures used in many diffrent tests
	 * 
	 */
	namespace tests
	{
		/**
		 * @brief stores numbers
		 * 
		 * @tparam _N stored number
		 */
		template<size_t _N>
		struct number_t
		{
			static inline constexpr size_t N = _N;
			inline friend std::ostream& operator<<(std::ostream& os, const number_t&) { return os << number_t::N; }
		};

		/**
		 * @brief verifies that both typesd has same size
		 * 
		 * @tparam T1 first type to get sizeof
		 * @tparam T2 second type to get sizeof
		 */
		template<typename T1, typename T2>
		inline constexpr void compare_size()
		{
			boost::ut::expect(boost::ut::eq(sizeof(T1), sizeof(T2)));
		}
	}	 // namespace tests
}	 // namespace serek

namespace but = boost::ut;
using namespace but::literals;

/**
 * @brief a bit workaround, to add main to all tests :)
 */
int main();