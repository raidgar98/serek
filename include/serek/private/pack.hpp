/**
 * @file pack.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains implementation of wrapper for class
 * @version 0.1
 * @date 2021-09-07
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

#include <iostream>

#include <serek/private/field.hpp>

namespace serek
{
	/**
	 * @brief contains implementation details of `pack`
	 */
	namespace detail
	{
		/**
		 * @copydoc pack_impl
		 *
		 * @tparam owner_t type of class that is wrapped
		 * @tparam previous_field_t type of last field
		 * @tparam owner_t::*value static pointer to last field
		 */
		template<typename owner_t, typename previous_field_t, previous_field_t owner_t::*value>
		struct pack_impl<value> : public owner_t
		{
			/**
			 * @brief Construct a new pack impl object by forwarding to base
			 */
			template<typename... Argv>
			pack_impl(Argv&&... argv) : owner_t{std::forward<Argv>(argv)...}
			{
			}

			/**
			 * @brief forwards visitor to last field
			 *
			 * @tparam visitor_t type of visitor to forward
			 * @param v visitor to forward
			 * @return visitor_result_t
			 */
			template<reqs::visitor_req visitor_t>
			visitor_result_t visit(visitor_t* v)
			{
				void* i_prev = v->that();
				v->that(this);
				const visitor_result_t result = visitors::visit(v, &(dynamic_cast<owner_t*>(this)->*value));
				v->that(i_prev);
				return result;
			}

			/**
			 * @copydoc visit
			 */
			template<reqs::visitor_req visitor_t>
			visitor_result_t visit(visitor_t* v) const
			{
				const void* i_prev = v->that();
				v->that(this);
				const visitor_result_t result = visitors::visit(v, &(dynamic_cast<const owner_t*>(this)->*value));
				v->that(i_prev);
				return result;
			}
		};
	}	 // namespace detail

	/**
	 * @copydoc detail::pack_impl
	 */
	template<auto last_field>
	using pack = typename detail::pack_impl<last_field>;
}	 // namespace serek
