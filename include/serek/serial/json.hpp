#pragma once

#include <iomanip>
#include <map>
#include <serek/serek.hpp>

#include <serek/serial/visitors.hpp>

namespace serek
{
	namespace serial
	{
		namespace json
		{
			using namespace serek::json::visitors;

			namespace detail
			{
				enum class JSON_CHARS : char
				{
					OBJECT_START = '{',
					OBJECT_STOP	 = '}',

					ARRAY_START = '[',
					ARRAY_STOP	= ']',

					KEY_VALUE_SEPARATOR = ':',
					ITEMS_SEPARATOR	  = ',',
				};

				struct stream_holder
				{
					template<typename Any>
					inline friend stream_holder& operator<<(stream_holder& out, const Any& any)
					{
						out.put_to_stream(any);
						return out;
					}

					void put_to_stream(const stream_holder& sh) { this->output << sh.output.str(); }
					void put_to_stream(const JSON_CHARS jc) { this->output << static_cast<char>(jc); }

					template<typename Any>
					void put_to_stream(const Any& any)
					{
						this->output << any;
					}

					void put_to_stream(const char c)
					{
						static const std::map<char, typename serek::str> trivial_escape{{std::pair<char, serek::str>{'\0', "\0"}, {'\1', "\1"}, {'\2', "\2"}, {'\3', "\3"}, {'\4', "\4"}, {'\5', "\5"}, {'\6', "\6"}, {'\7', "\7"}, {'\n', "\n"}, {'\r', "\r"}, {'\7', "\7"}, {'\'', "\'"}, {'\"', "\""}, {'\a', "\a"}, {'\b', "\b"}, {'\t', "\t"}, {'\v', "\v"}, {'\?', "\?"}, {'\\', "\\"}, {'\f', "\f"}}};
						serek::str x{"a"};

						const auto it = trivial_escape.find(c);
						if(it != trivial_escape.end()) x = it->second;
						else
							x[0] = c;

						this->output << std::quoted(x);
					}

					template<reqs::string_type_req T>
					void put_to_stream(const T& str)
					{
						this->output << std::quoted(str);
					}

				 protected:
					str get() const { return output.str(); }

				 protected:
					sstr output;
				};

				void add_key(std::stack<str>& stack, stream_holder& out)
				{
					out << std::quoted(stack.top()) << JSON_CHARS::KEY_VALUE_SEPARATOR;
					stack.pop();
				}

				template<reqs::visitor_req vis_t, typename Any>
				// void serial(visitors::base_visitor_members_with_stacknames& fwd, stream_holder& out, const Any& obj)
				void serial(vis_t& fwd, stream_holder& out, const Any& obj)
				{
					add_key(fwd.stack_name, out);
					out << JSON_CHARS::OBJECT_START;
					serek::visitors::visit(&fwd, &obj);
					out << JSON_CHARS::OBJECT_STOP;
				}

				template<reqs::visitor_req vis_t, reqs::fundamental_wrapper_req Any>
				void serial(vis_t& vis, stream_holder& out, const Any& obj)
				{
					add_key(vis.stack_name, out);
					out << obj;
				}

				template<reqs::visitor_req vis_t, reqs::iterable_req Any>
				void serial(vis_t& vis, stream_holder& out, const Any& any)
				{
					static const char separator_decision[2] = {static_cast<char>(detail::JSON_CHARS::ITEMS_SEPARATOR), '\0'};

					add_key(vis.stack_name, out);
					out << JSON_CHARS::ARRAY_START;
					for(auto it = any.begin(); it != any.end(); it++) out << separator_decision[it == any.begin()] << *it;
					out << JSON_CHARS::ARRAY_STOP;
				}
			}	 // namespace detail

			struct json_visitor : public visitors::base_visitor_members_with_stacknames, public detail::stream_holder
			{
				template<typename T>
				json_visitor(T* any) : base_visitor_members_with_stacknames{any}
				{
					put_to_stream(detail::JSON_CHARS::OBJECT_START);
				}

				template<typename Any>
				visitor_result_t operator()(const Any* any)
				{
					this->last_result = true;
					if(!stack_name.empty()) detail::serial(*this, static_cast<detail::stream_holder&>(*this), *any);

					if(!stack_name.empty()) put_to_stream(detail::JSON_CHARS::ITEMS_SEPARATOR);
					else
						put_to_stream(detail::JSON_CHARS::OBJECT_STOP);

			template<typename Any>
			serek::str serialize(const Any& any)
			{
				json_visitor vis{&any};
				serek::visitors::visit(&vis, &any);
				return vis.json();
			}

		}	 // namespace json
	}		 // namespace serial
}	 // namespace serek
