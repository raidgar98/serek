#pragma once

#include <concepts>
#include <functional>
#include <cassert>

namespace serek
{
	namespace visitors
	{
		using result_t = bool;
		template<typename acceptor_t>
		concept acceptor_req = requires(acceptor_t* x)
		{
			{ x->accept(nullptr) } -> std::same_as<result_t>;
		};

		template<typename visitor_t>
		concept visitor_req = requires(visitor_t* x)
		{
			{ x->visit(nullptr) } -> std::same_as<result_t>;
			{ x->last_result } -> std::same_as<result_t>;
			{ x->that } -> std::same_as<void*>;
		};

		namespace helpers
		{
			struct base_visitor_members
			{
				void* that;
				result_t last_result;
			};

			struct inline_base_visitor : public base_visitor_members
			{
				using visitor_fun_t = std::function<result_t(void*)>;
				visitor_fun_t foo;

				template<typename acceptor_t> result_t visit(acceptor_t* object) { return foo(object); }
			};
		}	 // namespace helpers

		template<visitor_req visitor_t, acceptor_req acceptor_t>
		result_t visit(visitor_t* vis, acceptor_t* object)
		{
			assert(object != nullptr);
			// assert( vis != nullptr );
			return object->accept(vis);
		}
	}	 // namespace visitors
}	 // namespace serek