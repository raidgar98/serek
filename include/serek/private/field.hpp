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

namespace serek
{
	namespace detail
	{

		template<auto ref_to_prev_member, typename T> 
		struct field_impl {};

		template<
			typename owner_t, 
			typename previous_field_t, 
			previous_field_t owner_t::* value, 
			typename current_field_t
		>
		struct field_impl<value, current_field_t>
		{
			using value_t = current_field_t;
			using class_t = owner_t;

			
		};
	}
}	 // namespace serek