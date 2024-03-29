/**
 * @file visitors.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief
 * @version 0.1
 * @date 2022-10-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <iostream>

#include <serek/private/visitors.hpp>
#include <serek/private/member_name_extractor.hpp>

namespace serek
{
	namespace json
	{
		namespace visitors
		{
			using namespace serek::visitors;

			struct base_visitor_members_with_stack_names : public base_visitor_members
			{
				struct pop_guard
				{
					base_visitor_members_with_stack_names* vis;

					~pop_guard()
					{
						if(vis != nullptr)
						{
							vis->top();
							vis->stack_name.pop();
						}
					}
				};

				template<typename Any>
				explicit base_visitor_members_with_stack_names(Any* any) : base_visitor_members{any}
				{
					serek::member_name_extractors::extract_class_members<Any>([&](const serek::str& v) { this->stack_name.emplace(v); });
					std::stack<str> _reverse;
					while(!stack_name.empty())
					{
						_reverse.emplace(stack_name.top());
						stack_name.pop();
					}
					stack_name = std::move(_reverse);
				}

				const str& top() const { return this->stack_name.top(); }
				pop_guard pop() { return pop_guard{this}; }

				bool empty() const { return stack_name.empty(); }

			 protected:
				std::stack<str> stack_name;
			};
		}	 // namespace visitors
	}		 // namespace json
}	 // namespace serek
