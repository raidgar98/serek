#pragma once

#include <iomanip>
#include <map>
#include <serek/serek.hpp>

#include <serek/json/common.hpp>
#include <serek/json/visitors.hpp>

namespace serek
{
	namespace serial
	{
		namespace json
		{
			using namespace serek::json;
			using namespace serek::json::visitors;

			namespace detail
			{
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
						static const std::map<char, typename serek::str> trivial_escape{{std::pair<char, serek::str>{'\0', "\0"},
																											  {'\1', "\1"},
																											  {'\2', "\2"},
																											  {'\3', "\3"},
																											  {'\4', "\4"},
																											  {'\5', "\5"},
																											  {'\6', "\6"},
																											  {'\7', "\7"},
																											  {'\n', "\n"},
																											  {'\r', "\r"},
																											  {'\7', "\7"},
																											  {'\'', "\'"},
																											  {'\"', "\""},
																											  {'\a', "\a"},
																											  {'\b', "\b"},
																											  {'\t', "\t"},
																											  {'\v', "\v"},
																											  {'\?', "\?"},
																											  {'\\', "\\"},
																											  {'\f', "\f"}}};
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

				auto add_key(base_visitor_members_with_stack_names& vis, stream_holder& out)
				{
					out << std::quoted(vis.top()) << JSON_CHARS::KEY_VALUE_SEPARATOR;
					return vis.pop();
				}

				template<reqs::visitor_req vis_t, auto Any>
				void serial(vis_t& fwd, stream_holder& out, const serek::pack<Any>* obj)
				{
					serek::require(obj);

					add_key(fwd, out);

					out << JSON_CHARS::OBJECT_START;

					vis_t new_obj_vis{obj};
					serek::visitors::visit(&new_obj_vis, obj);
					out.put_to_stream(static_cast<const stream_holder&>(new_obj_vis));

					out << JSON_CHARS::OBJECT_STOP;
				}

				template<reqs::visitor_req vis_t, typename Any>
				requires serek::requirements::fundamental_req<Any> || reqs::string_type_req<Any>
				void serial(vis_t& vis, stream_holder& out, const Any* obj)
				{
					serek::require(obj);
					add_key(vis, out);
					out.put_to_stream(*obj);
				}

				template<reqs::visitor_req vis_t, reqs::iterable_req Any>
				void serial(vis_t& vis, stream_holder& out, const Any* any)
				{
					serek::require(any);
					static const char separator_decision[2] = {static_cast<char>(JSON_CHARS::ITEMS_SEPARATOR), '\0'};

					add_key(vis, out);
					out << JSON_CHARS::ARRAY_START;
					for(auto it = any->begin(); it != any->end(); it++) out << separator_decision[it == any->begin()] << *it;
					out << JSON_CHARS::ARRAY_STOP;
				}
			}	 // namespace detail

			template<bool root_object = true>
			struct json_visitor : public base_visitor_members_with_stack_names, public detail::stream_holder
			{
				template<typename T>
				json_visitor(T* any) : base_visitor_members_with_stack_names{any}
				{
					if(root_object) put_to_stream(JSON_CHARS::OBJECT_START);
				}

				template<typename Any>
				visitor_result_t operator()(const serek::detail::type_holder<Any>* any)
				{
					this->process(any);
					return true;
				}

				str json() const { return get(); }

			 private:
				template<typename Any>
				void process(const serek::detail::type_holder<Any>* any)
				{
					this->last_result = true;
					if(!this->empty()) detail::serial(reinterpret_cast<json_visitor<false>&>(*this), static_cast<detail::stream_holder&>(*this), reinterpret_cast<const Any*>(any));

					if(!this->empty()) put_to_stream(JSON_CHARS::ITEMS_SEPARATOR);
					else if(root_object)
						put_to_stream(JSON_CHARS::OBJECT_STOP);
				}
			};

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
