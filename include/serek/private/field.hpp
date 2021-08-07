/**
 * @file serializer.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains implementation of serializtion for single field
 * @version 0.1
 * @date 2021-08-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <concepts>
#include "fundamental_type_holder.hpp"
namespace serek
{
	namespace detail
	{
		template<typename T>
		concept field_impl_field_t_req = requires
		{
			requires !std::is_fundamental_v<T>;
			requires !std::is_final_v<T>;
		};

		template<auto ref_to_prev_member, typename T> struct field_impl;

		template<typename owner_t, typename previous_field_t, previous_field_t owner_t::*value,
					field_impl_field_t_req current_field_t>
		struct field_impl<value, current_field_t> : public current_field_t
		{
		};
	}	 // namespace detail
	template<auto value, typename current_field_t>
	using field = typename std::conditional<
		 std::is_fundamental_v<current_field_t>,
		 detail::field_impl<value, detail::fundamental_type_holder<current_field_t>>,
		 detail::field_impl<value, current_field_t>>::type;

}	 // namespace serek