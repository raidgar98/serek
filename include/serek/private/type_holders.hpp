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
		 * @brief can be used to wrap class to be iterable over fields
		 *
		 * @tparam last_field last field
		 */
		template<auto last_field>
		struct pack_impl;
	}

	namespace requirements
	{
		/**
		 * @brief checks is given type `T` is fundamental
		 * @see https://en.cppreference.com/w/cpp/types/is_fundamental
		 */
		template<typename T>
		concept fundamental_req = requires
		{
			requires std::is_fundamental_v<T>;
		};

		/**
		 * @brief checks is given type `T` can be used as type for serialization
		 */
		template<typename T>
		concept field_impl_field_t_req = requires
		{
			requires !std::is_fundamental_v<T>;
			requires !std::is_final_v<T>;
		};
	}

	namespace detail
	{
		template<typename T>
		struct type_holder;

		/**
		 * @brief fasade that derivates from given type and forwards all constructor to given type
		 *
		 * @tparam T type to derive from
		 */
		template<requirements::field_impl_field_t_req T>
		struct type_holder<T> : public T
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

		template<typename owner_t, typename previous_field_t, previous_field_t owner_t::*value>
		struct type_holder<detail::pack_impl<value>> : public owner_t
		{
			template<typename... other_types>
			type_holder(other_types&& ... argv) : owner_t{std::forward<other_types>(argv)...}
			{
				
			}
		};

		/**
		 * @brief wrapper for fundamental types
		 * @note forward all possible operators and providec _implicit_ conversions
		 *
		 * @tparam T fundamental type to wrap
		 */
		template<requirements::fundamental_req T>
		struct type_holder<T>
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
			type_holder() : value{} {}
			template<typename other_t> type_holder(const other_t& arg) : value{arg} {}
			template<typename other_t> type_holder(other_t&& arg) : value{arg} {}


			// comprasion operators
			template<typename other_t>
			inline friend auto operator<=>(type_holder me, other_t other)
			{
				return me.value <=> other;
			}

			// assigment operators
			// template<typename other_t> inline friend auto operator*=(type_holder me, other_t other){ return me.value *= other; }
			// template<typename other_t> inline friend auto operator/=(type_holder me, other_t other){ return me.value /= other; }
			// template<typename other_t> inline friend auto operator+=(type_holder me, other_t other){ return me.value += other; }
			// template<typename other_t> inline friend auto operator-=(type_holder me, other_t other){ return me.value -= other; }
			// template<typename other_t> inline friend auto operator%=(type_holder me, other_t other){ return me.value %= other; }
			// template<typename other_t> inline friend auto operator<<=(type_holder me, other_t other){ return me.value <<= other; }
			// template<typename other_t> inline friend auto operator>>=(type_holder me, other_t other){ return me.value >>= other; }
			// template<typename other_t> inline friend auto operator&=(type_holder me, other_t other){ return me.value &= other; }
			// template<typename other_t> inline friend auto operator^=(type_holder me, other_t other){ return me.value ^= other; }
			// template<typename other_t> inline friend auto operator|=(type_holder me, other_t other){ return me.value |= other; }

			// template<typename other_t> inline friend auto operator*=(other_t other, type_holder me){ return other *= me.value; }
			// template<typename other_t> inline friend auto operator/=(other_t other, type_holder me){ return other /= me.value; }
			// template<typename other_t> inline friend auto operator+=(other_t other, type_holder me){ return other += me.value; }
			// template<typename other_t> inline friend auto operator-=(other_t other, type_holder me){ return other -= me.value; }
			// template<typename other_t> inline friend auto operator%=(other_t other, type_holder me){ return other %= me.value; }
			// template<typename other_t> inline friend auto operator<<=(other_t other, type_holder me){ return other <<= me.value; }
			// template<typename other_t> inline friend auto operator>>=(other_t other, type_holder me){ return other >>= me.value; }
			// template<typename other_t> inline friend auto operator&=(other_t other, type_holder me){ return other &= me.value; }
			// template<typename other_t> inline friend auto operator^=(other_t other, type_holder me){ return other ^= me.value; }
			// template<typename other_t> inline friend auto operator|=(other_t other, type_holder me){ return other |= me.value; }

			// increment/decrement operators
			inline friend auto operator++(type_holder me) { return ++me.value; }
			inline friend auto operator--(type_holder me) { return ++me.value; }
			inline friend auto operator++(type_holder me, int) { return me.value++; }
			inline friend auto operator--(type_holder me, int) { return me.value++; }

			// arithmetic operators
			// single argument
			inline friend auto operator+(type_holder me) { return +me.value; }
			inline friend auto operator-(type_holder me) { return -me.value; }
			inline friend auto operator~(type_holder me) { return ~me.value; }
			// double arguments
			// template<typename other_t> inline friend auto operator+(type_holder me, other_t other) { return me.value + other; }
			// template<typename other_t> inline friend auto operator-(type_holder me, other_t other) { return me.value - other; }
			// template<typename other_t> inline friend auto operator*(type_holder me, other_t other) { return me.value * other; }
			// template<typename other_t> inline friend auto operator/(type_holder me, other_t other) { return me.value / other; }
			// template<typename other_t> inline friend auto operator%(type_holder me, other_t other) { return me.value % other; }
			// template<typename other_t> inline friend auto operator&(type_holder me, other_t other) { return me.value & other; }
			// template<typename other_t> inline friend auto operator|(type_holder me, other_t other) { return me.value | other; }
			// template<typename other_t> inline friend auto operator^(type_holder me, other_t other) { return me.value ^ other; }
			// template<typename other_t> inline friend auto operator<<(type_holder me, other_t other) { return me.value << other; }
			// template<typename other_t> inline friend auto operator>>(type_holder me, other_t other) { return me.value >> other; }

			// template<typename other_t> inline friend auto operator+(other_t other, type_holder me) { return other + me.value; }
			// template<typename other_t> inline friend auto operator-(other_t other, type_holder me) { return other - me.value; }
			// template<typename other_t> inline friend auto operator*(other_t other, type_holder me) { return other * me.value; }
			// template<typename other_t> inline friend auto operator/(other_t other, type_holder me) { return other / me.value; }
			// template<typename other_t> inline friend auto operator%(other_t other, type_holder me) { return other % me.value; }
			// template<typename other_t> inline friend auto operator&(other_t other, type_holder me) { return other & me.value; }
			// template<typename other_t> inline friend auto operator|(other_t other, type_holder me) { return other | me.value; }
			// template<typename other_t> inline friend auto operator^(other_t other, type_holder me) { return other ^ me.value; }
			// template<typename other_t> inline friend auto operator<<(other_t other, type_holder me) { return other << me.value; }
			// template<typename other_t> inline friend auto operator>>(other_t other, type_holder me) { return other >> me.value; }

		 private:
			inline constexpr T& get() { return value; }
			inline constexpr const T& get() const { return value; }

			// verify is given type is fundamental one more time
			static_assert(std::is_fundamental_v<T>);
		};
	}	 // namespace detail
}	 // namespace serek
