#pragma once

#include <iostream>

#include <serek/private/field.hpp>

namespace serek
{
	namespace detail
	{
		template<auto last_field>
		struct pack_impl;

		template<typename owner_t, typename previous_field_t, previous_field_t owner_t::*value>
		struct pack_impl<value> : public owner_t
		{
			template<typename... Argv>
			explicit pack_impl(Argv&&... argv) : owner_t{{std::forward<Argv>(argv)}...}
			{
			}

			template<reqs::visitor_req visitor_t>
			visitor_result_t visit(visitor_t* v)
			{
				void* i_prev = v->that();
				v->that(this);
				const visitor_result_t result = visitors::visit(v, &(dynamic_cast<owner_t*>(this)->*value));
				v->that(i_prev);
				return result;
			}
		};
	}	 // namespace detail

	template<auto last_field>
	using pack = typename detail::pack_impl<last_field>;
}	 // namespace serek