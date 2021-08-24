#pragma once

#include <concepts>
#include "types.hpp"

namespace serek
{
	namespace requirements
	{
		namespace detail
		{
			template<typename T>
			concept field_impl_field_t_req = requires
			{
				requires !std::is_fundamental_v<T>;
				requires !std::is_final_v<T>;
			};

			template<typename T>
			concept fundamental_req = requires
			{
				requires std::is_fundamental_v<T>;
			};

			template<typename T>
			concept acceptor_worker_req = requires(T x)
			{
				std::same_as<decltype(x.result), visitor_result_t>;
			};

			template<typename acceptor_t>
			concept acceptor_req = requires(acceptor_t* x)
			{
				std::same_as<decltype(x->accept(nullptr)), visitor_result_t>;
			};

			template<typename visitor_t>
			concept visitor_req = requires(visitor_t* x)
			{
				std::same_as<decltype(x->visit(nullptr)), visitor_result_t>;
				std::same_as<decltype(x->last_result), visitor_result_t>;
				std::same_as<decltype(x->that), void*>;
			};

		}	 // namespace detail
	}		 // namespace requirements

	namespace reqs = requirements::detail;
}	 // namespace serek