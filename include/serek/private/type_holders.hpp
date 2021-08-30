/**
 * @file type_holders.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contatins declarations and definitions of structures responsible for accessing contained value
 * @version 0.1
 * @date 2021-08-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <utility>
#include <type_traits>

namespace serek
{
	namespace detail
	{
		/**
		 * @brief fasade that derivates from given type and forwards all constructor to given type
		 * 
		 * @tparam T type to derive from
		 */
		template<typename T>
		struct type_holder : public T
		{
			/**
			 * @brief Construct a new type holder object by forwarding all values to parent
			 * 
			 * @tparam other_types variadic types
			 * @param argv variadic arguments
			 */
			template<typename... other_types>
			type_holder(other_types&&... argv) : T{std::forward<other_types>(argv)...}
			{
			}
		};

		/**
		 * @brief wrapper for fundamental types
		 * @note forward all possible operators and providec _implicit_ conversions
		 * 
		 * @tparam T fundamental type to wrap
		 */
		template<typename T>
		struct fundamental_type_holder
		{
			/** @brief type for internal library usage */
			using _____fundamental_type_wrapper = std::true_type;
			using value_t								= T;

			T value;

			// conversion
			operator T() { return get(); }
			operator T() const { return get(); }

			// access operator
			T operator()() { return get(); }
			T operator()() const { return get(); }

			// forward all constructors
			template<typename... other_types>
			fundamental_type_holder(other_types&&... argv) : value{std::forward<other_types>(argv)...}
			{
			}


			// comprasion operators
			template<typename other_t>
			inline friend auto operator<=>(fundamental_type_holder me, other_t other)
			{
				return me.value <=> other;
			}

			// assigment operators
			// template<typename other_t> inline friend auto operator*=(fundamental_type_holder me, other_t other){ return me.value *= other; }
			// template<typename other_t> inline friend auto operator/=(fundamental_type_holder me, other_t other){ return me.value /= other; }
			// template<typename other_t> inline friend auto operator+=(fundamental_type_holder me, other_t other){ return me.value += other; }
			// template<typename other_t> inline friend auto operator-=(fundamental_type_holder me, other_t other){ return me.value -= other; }
			// template<typename other_t> inline friend auto operator%=(fundamental_type_holder me, other_t other){ return me.value %= other; }
			// template<typename other_t> inline friend auto operator<<=(fundamental_type_holder me, other_t other){ return me.value <<= other; }
			// template<typename other_t> inline friend auto operator>>=(fundamental_type_holder me, other_t other){ return me.value >>= other; }
			// template<typename other_t> inline friend auto operator&=(fundamental_type_holder me, other_t other){ return me.value &= other; }
			// template<typename other_t> inline friend auto operator^=(fundamental_type_holder me, other_t other){ return me.value ^= other; }
			// template<typename other_t> inline friend auto operator|=(fundamental_type_holder me, other_t other){ return me.value |= other; }

			// template<typename other_t> inline friend auto operator*=(other_t other, fundamental_type_holder me){ return other *= me.value; }
			// template<typename other_t> inline friend auto operator/=(other_t other, fundamental_type_holder me){ return other /= me.value; }
			// template<typename other_t> inline friend auto operator+=(other_t other, fundamental_type_holder me){ return other += me.value; }
			// template<typename other_t> inline friend auto operator-=(other_t other, fundamental_type_holder me){ return other -= me.value; }
			// template<typename other_t> inline friend auto operator%=(other_t other, fundamental_type_holder me){ return other %= me.value; }
			// template<typename other_t> inline friend auto operator<<=(other_t other, fundamental_type_holder me){ return other <<= me.value; }
			// template<typename other_t> inline friend auto operator>>=(other_t other, fundamental_type_holder me){ return other >>= me.value; }
			// template<typename other_t> inline friend auto operator&=(other_t other, fundamental_type_holder me){ return other &= me.value; }
			// template<typename other_t> inline friend auto operator^=(other_t other, fundamental_type_holder me){ return other ^= me.value; }
			// template<typename other_t> inline friend auto operator|=(other_t other, fundamental_type_holder me){ return other |= me.value; }

			// increment/decrement operators
			inline friend auto operator++(fundamental_type_holder me) { return ++me.value; }
			inline friend auto operator--(fundamental_type_holder me) { return ++me.value; }
			inline friend auto operator++(fundamental_type_holder me, int) { return me.value++; }
			inline friend auto operator--(fundamental_type_holder me, int) { return me.value++; }

			// arithmetic operators
			// single argument
			inline friend auto operator+(fundamental_type_holder me) { return +me.value; }
			inline friend auto operator-(fundamental_type_holder me) { return -me.value; }
			inline friend auto operator~(fundamental_type_holder me) { return ~me.value; }
			// double arguments
			// template<typename other_t> inline friend auto operator+(fundamental_type_holder me, other_t other) { return me.value + other; }
			// template<typename other_t> inline friend auto operator-(fundamental_type_holder me, other_t other) { return me.value - other; }
			// template<typename other_t> inline friend auto operator*(fundamental_type_holder me, other_t other) { return me.value * other; }
			// template<typename other_t> inline friend auto operator/(fundamental_type_holder me, other_t other) { return me.value / other; }
			// template<typename other_t> inline friend auto operator%(fundamental_type_holder me, other_t other) { return me.value % other; }
			// template<typename other_t> inline friend auto operator&(fundamental_type_holder me, other_t other) { return me.value & other; }
			// template<typename other_t> inline friend auto operator|(fundamental_type_holder me, other_t other) { return me.value | other; }
			// template<typename other_t> inline friend auto operator^(fundamental_type_holder me, other_t other) { return me.value ^ other; }
			// template<typename other_t> inline friend auto operator<<(fundamental_type_holder me, other_t other) { return me.value << other; }
			// template<typename other_t> inline friend auto operator>>(fundamental_type_holder me, other_t other) { return me.value >> other; }

			// template<typename other_t> inline friend auto operator+(other_t other, fundamental_type_holder me) { return other + me.value; }
			// template<typename other_t> inline friend auto operator-(other_t other, fundamental_type_holder me) { return other - me.value; }
			// template<typename other_t> inline friend auto operator*(other_t other, fundamental_type_holder me) { return other * me.value; }
			// template<typename other_t> inline friend auto operator/(other_t other, fundamental_type_holder me) { return other / me.value; }
			// template<typename other_t> inline friend auto operator%(other_t other, fundamental_type_holder me) { return other % me.value; }
			// template<typename other_t> inline friend auto operator&(other_t other, fundamental_type_holder me) { return other & me.value; }
			// template<typename other_t> inline friend auto operator|(other_t other, fundamental_type_holder me) { return other | me.value; }
			// template<typename other_t> inline friend auto operator^(other_t other, fundamental_type_holder me) { return other ^ me.value; }
			// template<typename other_t> inline friend auto operator<<(other_t other, fundamental_type_holder me) { return other << me.value; }
			// template<typename other_t> inline friend auto operator>>(other_t other, fundamental_type_holder me) { return other >> me.value; }

		private:
			inline constexpr T& get() { return value; }
			inline constexpr const T& get() const { return value; }

			// verify is given type is fundamental one more time
			static_assert(std::is_fundamental_v<T>);
		};
	}	 // namespace detail
}	 // namespace serek