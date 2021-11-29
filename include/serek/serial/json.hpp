#pragma once

#include <iomanip>
#include <serek/serek.hpp>

namespace serek
{
	namespace serial
	{
		namespace json
		{
			namespace detail
			{

				struct stream_holder
				{
					template<typename Any>
					inline friend stream_holder& operator<<(stream_holder& out, const Any& any)
					{
						out.output << any;
						return out;
					}

				protected:
					str get() const { return output.str(); }
				private:
					sstr output;
				};

				void add_key(std::stack<str>& stack, stream_holder& out)
				{
					out << std::quoted(stack.top()) << ':';
					stack.pop();
				}

				template<typename Any>
				void serial(visitors::base_visitor_members_with_stacknames& fwd, stream_holder& out, const Any& obj)
				{
					add_key(fwd.stack_name, out);
					out << '{';
					serek::visitors::visit(&fwd, &obj);
					out << '}';
				}

				template<reqs::fundamental_wrapper_req Any>
				void serial(visitors::base_visitor_members_with_stacknames& vis, stream_holder& out, const Any& obj)
				{
					add_key(vis.stack_name, out);
					out << obj;
				}

				template<reqs::iterable_req Any>
				void serial(visitors::base_visitor_members_with_stacknames& vis, stream_holder& out, const Any& any)
				{
					add_key(vis.stack_name, out);
					out << '[';
					for(auto it = any.begin(); it != any.end(); it++) out << ","[it == any.begin()] << *it;
					out << ']';
				}
			}

			struct json_visitor : public visitors::base_visitor_members_with_stacknames, public detail::stream_holder
			{
				template<typename T>
				json_visitor(T* any) : base_visitor_members_with_stacknames{any}
				{
					(*this) << '{';
				}

				template<typename Any>
				visitor_result_t operator()(const Any* any)
				{
					this->last_result = true;
					if(!stack_name.empty())
						detail::serial(*this, static_cast<detail::stream_holder&>(*this), *any);
					if(!stack_name.empty()) (*this) << ',';
					else (*this) << '}';

					return true;
				}

				str json() const
				{
					return get();
				}
			};
		}
	}
}



