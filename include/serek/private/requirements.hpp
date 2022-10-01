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
#include <stack>

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
			template<typename T>
			concept string_type_req = requires
			{
				requires(std::same_as<serek::str, T> || std::same_as<serek::str_v, T>);
			};

			/**
			 * @brief checks is given type is iterable
			 *
			 * @tparam T type to check
			 */
			template<typename T>
			concept iterable_req = requires(T x)
			{
				typename T::value_type;
				requires std::same_as < std::remove_cvref_t<decltype(*x.begin())>,
				typename T::value_type > ;
				{x.end()};
				requires !string_type_req<T>;
			};

			/**
			 * @brief checks is given type is fundamental wrapper
			 *
			 * @tparam T type to check
			 */
			template<typename T>
			concept fundamental_wrapper_req = requires
			{
				typename T::_____fundamental_type_wrapper;
				typename T::value_t;
			};

			/**
			 * @brief checks is given type fullfils visitor requirements
			 *
			 * @tparam visitor_t type to check
			 */
			template<typename visitor_t>
			concept visitor_req = requires(visitor_t* x)
			{
				requires std::same_as<decltype(x->template operator()(new serek::detail::type_holder<int>)), visitor_result_t>;
				requires std::same_as<decltype(x->last_result), visitor_result_t>;
				requires std::same_as<decltype(x->that()), void*>;
				requires std::same_as<decltype(x->template that<int>()), int*>;
				{x->that(static_cast<void*>(NULL))};
			};

			template<typename visitor_t>
			concept visitor_with_member_name_stack = visitor_req<visitor_t> && requires(visitor_t* x)
			{
				requires std::same_as<decltype(x->stack_name), std::stack<str>>;
			};

			/**
			 * @brief encapsulates types required to define `acceptor_req`
			 */
			namespace acceptor_req_details
			{
				/**
				 * @brief fullfills visitor_req
				 */
				struct ex_vis
				{
					visitor_result_t last_result;

					void* that() const { return const_cast<void*>(m_that); }
					void that(void* input) { m_that = input; }
					template<typename T>
					T* that()
					{
						return reinterpret_cast<T*>(this->that());
					}
					template<typename T>
					T* that() const
					{
						return reinterpret_cast<T*>(this->that());
					}

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

			/**
			 * @brief checks is given type can be used as accptor
			 *
			 * @tparam acceptor_t
			 */
			template<typename acceptor_t>
			concept acceptor_req = requires(acceptor_t* x)
			{
				requires std::same_as<decltype(x->template visit(new acceptor_req_details::ex_vis)), visitor_result_t>;
			};

			/**
			 * @brief encapsulates helper types to define `acceptor_worker_req`
			 */
			namespace acceptor_worker_req_detail
			{
				/**
				 * @brief own imlementation of std::is_invocable
				 *
				 * @tparam T type to check is invocable
				 * @tparam Argv invocable argument types
				 */
				template<typename T, typename... Argv>
				concept acceptor_worker_req_helper = requires(T x, Argv&&... args)
				{
					{x(std::forward<Argv>(args)...)};
				};
			}	 // namespace acceptor_worker_req_detail

			/**
			 * @brief checks is given type can be used as acceptor worker
			 *
			 * @tparam T type to check
			 */
			template<typename T>
			concept acceptor_worker_req = requires
			{
				typename T::value_t;
			}
			&&acceptor_worker_req_detail::acceptor_worker_req_helper<T, typename T::value_t*, acceptor_req_details::ex_vis*> || acceptor_worker_req_detail::acceptor_worker_req_helper<T, const typename T::value_t*, acceptor_req_details::ex_vis*>;

			/**
			 * @brief checks is given type can be compared like pointerr
			 *
			 * @tparam T type to check
			 */
			template<typename T>
			concept comparable_as_pointer_req = requires(T x)
			{
				requires std::same_as<decltype(x == nullptr), bool>;
			};

			/**
			 * @brief checks is given type can be used as exception
			 * @note it just need to be constructible from std::string_view
			 *
			 * @tparam T type to check
			 */
			template<typename T>
			concept throwable_req = requires
			{
				{T{serek::str_v{}}};
			};

		}	 // namespace detail
	}		 // namespace requirements

	namespace reqs = requirements::detail;
}	 // namespace serek
