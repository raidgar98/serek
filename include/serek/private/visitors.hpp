#pragma once

#include <functional>
#include <cassert>

#include "requirements.hpp"

namespace serek
{
	namespace visitors
	{
		using namespace requirements::detail;

		struct base_visitor_members
		{
			void* that;
			visitor_result_t last_result;
		};

		template<visitor_req visitor_t, acceptor_req acceptor_t>
		visitor_result_t visit(visitor_t* vis, acceptor_t* object)
		{
			assert(object != nullptr);
			// assert( vis != nullptr );
			return object->accept(vis);
		}
	}	 // namespace visitors
}	 // namespace serek