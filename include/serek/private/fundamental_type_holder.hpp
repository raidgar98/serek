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

			// conversion
			operator T() { return get(); }
			operator T() const { return get(); }

			// access operator
			T operator()(){ return get(); }
			T operator()() const { return get(); }

			// forward all constructors
			template<typename ... other_types>
			fundamental_type_holder(other_types ... argv) : value{ std::forward<other_types>(argv)... } {}

			// comprasion operators
			template<typename other_t>
			inline friend auto operator<=>(fundamental_type_holder me, other_t other) { return me.value <=> other; }

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
				inline friend auto operator++(fundamental_type_holder me){ return ++me.value; }
				inline friend auto operator--(fundamental_type_holder me){ return ++me.value; }
				inline friend auto operator++(fundamental_type_holder me, int){ return me.value++; }
				inline friend auto operator--(fundamental_type_holder me, int){ return me.value++; }

			// arithmetic operators
				// single argument
					inline friend auto operator+(fundamental_type_holder me){ return +me.value; }
					inline friend auto operator-(fundamental_type_holder me){ return -me.value; }
					inline friend auto operator~(fundamental_type_holder me){ return ~me.value; }
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

			static_assert( std::is_fundamental_v<T> );
		};
	}
}