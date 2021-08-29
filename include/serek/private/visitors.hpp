#pragma once

#include <functional>
#include <cassert>

#include <serek/private/requirements.hpp>

namespace serek
{
	namespace visitors
	{
		using namespace requirements::detail;

		struct base_visitor_members
		{
			visitor_result_t last_result;

			template<typename Any>
			explicit base_visitor_members(Any* i_that = nullptr)
			{
				that(i_that);
			}

			void* that() const { return const_cast<void*>(m_that); }
			void that(volatile void* input) { m_that = input; }
			void that(void* input) { that(const_cast<volatile void*>(input)); }

			template<typename Any>
			Any* that() const
			{
				return reinterpret_cast<Any*>(that());
			}
			template<typename Any>
			void that(Any* input)
			{
				that(reinterpret_cast<void*>(input));
			}

		 private:
			volatile void* m_that = nullptr;
		};

		template<visitor_req visitor_t, acceptor_req acceptor_t>
		visitor_result_t visit(visitor_t* vis, acceptor_t* object)
		{
			serek::require(object);
			return object->visit(vis);
		}
	}	 // namespace visitors
}	 // namespace serek