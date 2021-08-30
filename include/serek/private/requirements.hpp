/**
 * @file requirements.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contatins declarations and definitions of concepts
 * @version 0.1
 * @date 2021-08-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <concepts>

#include <serek/private/types.hpp>
#include <serek/private/type_holders.hpp>

namespace serek
{
	/**
	 * @brief Contaitns concepts declarations
	 */
	namespace requirements
	{
		namespace detail
		{
			/**
			 * @brief checks is given type `T` can be used as type for serialization
			 */
			template<typename T>
			concept field_impl_field_t_req = requires
			{
				requires !std::is_fundamental_v<T>;
				requires !std::is_final_v<T>;
			};

			/**
			 * @brief checks is given type `T` is fundamental
			 * @see https://en.cppreference.com/w/cpp/types/is_fundamental
			 */
			template<typename T>
			concept fundamental_req = requires
			{
				requires std::is_fundamental_v<T>;
			};

			template<typename T>
			concept fundamental_wrapper_req = requires
			{
				typename T::_____fundamental_type_wrapper;
				typename T::value_t;
			};

			template<typename visitor_t>
			concept visitor_req = requires(visitor_t* x)
			{
				std::same_as<decltype(x->template operator()(new serek::detail::fundamental_type_holder<int>)), visitor_result_t>;
				std::same_as<decltype(x->last_result), visitor_result_t>;
				std::same_as<decltype(x->that()), void*>;
				{x->that(static_cast<void*>(NULL))};
			};

			namespace acceptor_req_details
			{
				struct ex_vis
				{
					visitor_result_t last_result;

					void* that() const { return const_cast<void*>(m_that); }
					void that(void* input) { m_that = input; }

					template<typename T>
					visitor_result_t operator()(T*)
					{
						return visitor_result_t{};
					}

				 private:
					volatile void* m_that = nullptr;
				};
				static_assert(visitor_req<ex_vis>);

			}	 // namespace acceptor_req_details

			template<typename acceptor_t>
			concept acceptor_req = requires(acceptor_t* x)
			{
				std::same_as<decltype(x->template visit(new acceptor_req_details::ex_vis)), visitor_result_t>;
			};

			template<typename T>
			concept acceptor_worker_req = requires(T x)
			{
				{T{nullptr, new acceptor_req_details::ex_vis}};
				std::same_as<decltype(x.result), visitor_result_t>;
			};

			template<typename T>
			concept comparable_as_pointer_req = requires(T x)
			{
				std::same_as<decltype(x == nullptr), bool>;
			};

			template<typename T>
			concept throwable_req = requires
			{
				{T{serek::str_v{}}};
			};

		}	 // namespace detail
	}		 // namespace requirements

	namespace reqs = requirements::detail;
}	 // namespace serek