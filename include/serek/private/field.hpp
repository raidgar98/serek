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
#include <stdexcept>
#include "fundamental_type_holder.hpp"
#include "visitors.hpp"

namespace serek
{
	namespace detail
	{
		template<typename current_field_t, reqs::acceptor_worker_req acceptor_worker_t>

		 protected:
			template<visitor_req visitor_t>
			void validate_visitator(visitor_t* visitor) const
			{
				if(visitor == nullptr) throw std::invalid_argument{"visitor cannot be nullptr"};
				if(visitor->that == nullptr)
					throw std::invalid_argument{"that in visitor cannot be nullptr"};
			}
		};

		template<reqs::field_impl_field_t_req current_field_t,
					reqs::acceptor_worker_req acceptor_worker_t>
			{
				validate_visitator(visitor);
				result = visitor->visit(this);
			}
		};
		template<typename class_t, auto value>
		struct forward_acceptor_worker : public acceptor_base
		{
			template<visitor_req visitor_t>
			explicit forward_acceptor_worker(visitor_t* visitor)
			{
				validate_visitator(visitor);
				visitor->last_result = visitors::visit(
					visitor, 
					reinterpret_cast<class_t*>(visitor->that)->*value
				);
				result = visitor->visit(this);
			}
		};

		template<reqs::fundamental_req current_field_t, reqs::acceptor_worker_req acceptor_worker_t>
		{
			template<visitor_req visitor_t> visitor_result_t accept(visitor_t* visitor)
			{
				return acceptor_worker_t{visitor}.result;
			}

			template<visitor_req visitor_t>
			visitor_result_t accept(visitor_t* visitor) const
			{
				return acceptor_worker_t{visitor}.result;
			}
		};

		template<typename current_field_t, acceptor_worker_req acceptor_worker_t>
		struct field_impl_value_handler;

		template<field_impl_field_t_req current_field_t, acceptor_worker_req acceptor_worker_t>
		struct field_impl_value_handler<current_field_t, acceptor_worker_t> :
			 public current_field_t,
			 public acceptor_core_impl<acceptor_worker_t>
		{};

		template<fundamental_req current_field_t, acceptor_worker_req acceptor_worker_t>
		struct field_impl_value_handler<current_field_t, acceptor_worker_t> :
			 public detail::fundamental_type_holder<current_field_t>,
			 public acceptor_core_impl<acceptor_worker_t>
		{};

		template<auto ref_to_prev_member, typename T> struct field_impl;
		template<typename owner_t, typename previous_field_t, previous_field_t owner_t::*value, typename current_field_t>
		struct field_impl<value, current_field_t> :
			 public field_impl_value_handler<current_field_t, forward_acceptor_worker<owner_t, value>>
		{};

	}	 // namespace detail

	template<auto value, typename current_field_t>
	using field = detail::field_impl<value, current_field_t>;

	template<typename first_field_t>
	using ffield = detail::field_impl_value_handler<first_field_t, typename detail::basic_acceptor_worker>;

}	 // namespace serek