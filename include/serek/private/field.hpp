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
#include <serek/private/fundamental_type_holder.hpp>
#include <serek/private/visitors.hpp>
#include <serek/private/acceptors.hpp>

namespace serek
{
	namespace detail
	{
		using namespace acceptors;
		using namespace acceptors::workers;

		template<typename current_field_t, template<typename T> typename acceptor_worker_t>
		struct field_impl_value_handler;

		template<auto ref_to_prev_member, typename T>
		struct field_impl;

		template<reqs::field_impl_field_t_req current_field_t, template<typename T> typename acceptor_worker_t>
		struct field_impl_value_handler<current_field_t, acceptor_worker_t> : public current_field_t, public acceptor_impl<acceptor_worker_t<current_field_t>>
		{
		};

		template<reqs::fundamental_req current_field_t, template<typename T> typename acceptor_worker_t>
		struct field_impl_value_handler<current_field_t, acceptor_worker_t> : public detail::fundamental_type_holder<current_field_t>, public acceptor_impl<acceptor_worker_t<detail::fundamental_type_holder<current_field_t>>>
		{
		};

		template<typename owner_t, typename previous_field_t, previous_field_t owner_t::*value, typename current_field_t>
		struct field_impl<value, current_field_t> : public field_impl_value_handler<current_field_t, detail::forward_acceptor_creator<value>::template forward_acceptor_worker_impl>
		{
		};

	}	 // namespace detail

	template<auto value, typename current_field_t>
	using field = detail::field_impl<value, current_field_t>;

	template<typename first_field_t>
	using ffield = detail::field_impl_value_handler<first_field_t, detail::finalize_acceptor_worker>;

}	 // namespace serek