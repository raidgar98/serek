/**
 * @file visitors.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains declarations of visitors
 * @version 0.1
 * @date 2021-08-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <functional>

#include <serek/private/exceptions.hpp>
#include <serek/private/requirements.hpp>

namespace serek
{
	/**
	 * @brief Contatins declaration of visitor base class (interface)
	 * 
	 */
	namespace visitors
	{
		using namespace requirements::detail;

		/**
		 * @brief base class for further visitors
		 * 
		 * @note derive from this class to fullfill most of `reqs::visitor_req` concept
		 */
		struct base_visitor_members
		{
			visitor_result_t last_result;

			/**
			 * @brief Construct a new base visitor members object
			 * 
			 * @tparam Any type of opinter
			 * @param i_that pointer to object that will be visited
			 * @todo rethink is `i_that` required in base class, maybe it should be in something like `base_field_iterable_visitor_t`?
			 */
			template<typename Any>
			explicit base_visitor_members(Any* i_that = nullptr)
			{
				that(i_that);
			}

			/**
			 * @brief returns pointer to that
			 * 
			 * @return void* 
			 */
			void* that() const { return const_cast<void*>(m_that); }

			/**
			 * @brief sets that pointer
			 * 
			 * @param input pointer to set as that
			 */
			void that(volatile void* input) { m_that = input; }

			/**
			 * @copydoc base_visitor_members::that(volatile void* input)
			 */
			void that(void* input) { that(const_cast<volatile void*>(input)); }

			/**
			 * @copydoc base_visitor_members::that() const
			 * 
			 * @tparam Any type to cast `that` to
			 * @return Any* 
			 */
			template<typename Any>
			Any* that() const
			{
				return reinterpret_cast<Any*>(that());
			}

			/**
			 * @copydoc base_visitor_members::that(volatile void* input)
			 * 
			 * @tparam Any type of input pointer
			 */
			template<typename Any>
			void that(Any* input)
			{
				that(reinterpret_cast<void*>(input));
			}

			/**
			 * @copydoc base_visitor_members::that(Any*)
			 */
			template<typename Any>
			void that(const Any* input)
			{
				that(reinterpret_cast<void*>(const_cast<Any*>(input)));
			}

		private:
			volatile void* m_that = nullptr;
		};

		namespace{
			/**
			 * @brief dummy struct just to validate is base_visitor_members is properly implemented
			 */
			struct dummy_visitor : public base_visitor_members
			{
				template<typename Any>
				visitor_result_t operator()(Any*) { return visitor_result_t{}; }
			};

			// verifies that `base_visitor_members` fullfils `visitor_req` concept
			static_assert( reqs::visitor_req<dummy_visitor> );
		}

		/**
		 * @brief This function should be called to visit objects
		 * 
		 * @tparam visitor_t type of visitor
		 * @tparam acceptor_t type of object to be visitred
		 * @param vis visitor that will be passed to `object`
		 * @param object acceptor that will get `vis` via `::visit()` method
		 * @return visitor_result_t 
		 */
		template<visitor_req visitor_t, acceptor_req acceptor_t>
		visitor_result_t visit(visitor_t* vis, acceptor_t* object)
		{
			serek::require(object);
			return object->visit(vis);
		}
		/**
		 * @copydoc visit
		 */
		template<visitor_req visitor_t, acceptor_req acceptor_t>
		visitor_result_t visit(visitor_t* vis, const acceptor_t* object)
		{
			serek::require(object);
			return object->visit(vis);
		}
	}	 // namespace visitors
}	 // namespace serek