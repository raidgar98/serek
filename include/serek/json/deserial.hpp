/**
 * @file deserial.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief
 * @version 0.1
 * @date 2022-12-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <iomanip>

#include <serek/json/common.hpp>
#include <serek/json/tokenization.hpp>
#include <serek/json/visitors.hpp>

namespace serek
{
	namespace deserial
	{
		namespace json
		{
			using namespace serek::json;
			using namespace serek::json::visitors;

			namespace detail
			{
				struct visitor_with_stacked_names_and_tokenized_json : public base_visitor_members_with_stack_names
				{
					template<typename Any>
					visitor_with_stacked_names_and_tokenized_json(const serek::str_v json_input, Any& output) :
						 base_visitor_members_with_stack_names{&output}, tokenized_json{tokenize_json(json_input)}
					{
					}

					template<typename Any>
					visitor_with_stacked_names_and_tokenized_json(json_tokenizer::result_t tokenized_json_input, Any& output) :
						 base_visitor_members_with_stack_names{&output}, tokenized_json{tokenized_json_input}
					{
					}

					/**
					 * @brief Pops and returns both from tokenized json and from stack
					 *
					 * @param key name to pop
					 * @return json_tokenizer::result_t data from tokenized json
					 */
					json_tokenizer::result_t pop_for_key(const serek::str key);

				 protected:
					json_tokenizer::result_t tokenized_json;
				};

				namespace
				{
					template<reqs::integer_type_req Any>
					void fill(Any& output, const serek::str_v value) { output = static_cast<Any>(std::atoll(value.data())); }

					template<reqs::floating_type_req Any>
					void fill(Any& output, const serek::str_v value)
					{
						output = static_cast<Any>(std::atof(value.data()));
					}

					template<reqs::string_type_req Any>
					void fill(Any& output, const serek::str_v value)
					{
						serek::require<std::greater>(value.length(), 1ul);
						serek::require<std::equal_to>(value.front(), '"');
						serek::require<std::equal_to>(value.back(), '"');

						/*
						0	1	2	3	4	5	6		length	start_pos	count
						"	a	a	a	a	a	"		7			1				5
						"	a	a	"					4			1				2
						"	"							2			1				0
					 */
						const auto unquoted_view = value.substr(1, value.length() - 2);
						output						 = Any{unquoted_view.begin(), unquoted_view.end()};
					}

					template<reqs::char_type_req char_t>
					void fill(char_t& output, const serek::str_v value)
					{
						serek::str proxy;
						fill<serek::str>(proxy, value);
						serek::require<std::greater_equal>(proxy.length(), 1ul);
						output = proxy[0];
					}

					// this is fundamental types deserialization
					template<reqs::visitor_req vis_t, typename Any>
					requires serek::requirements::fundamental_req<Any> || reqs::string_type_req<Any>
					void deserial(visitor_with_stacked_names_and_tokenized_json& json, Any* output)
					{
						serek::require(output);

						const auto json_value = json.pop_for_key(json.top());
						serek::require<std::equal_to>(json_value->element_type, json_element_t::FUNDAMENTAL_TYPE);
						fill(*output, json_value->item);
					}

					// this is object deserialization
					template<reqs::visitor_req vis_t, auto Any>
					void deserial(visitor_with_stacked_names_and_tokenized_json& json, serek::detail::pack_impl<Any>* output)
					{
						serek::require(output);

						vis_t new_obj_vis{json.pop_for_key(json.top()), *output};
						serek::visitors::visit(&new_obj_vis, output);
					}

					// TODO: Add support for array and check is serialization works for array!
					// this is array deserialization
					template<reqs::visitor_req vis_t, reqs::iterable_req Any>
					void deserial(visitor_with_stacked_names_and_tokenized_json& json, serek::detail::pack_impl<Any>* output)
					// void deserial(vis_t& vis, const Any* any)
					{
						serek::require(any);
						static const char separator_decision[2] = {static_cast<char>(JSON_CHARS::ITEMS_SEPARATOR), '\0'};

						add_key(vis, out);
						out << JSON_CHARS::ARRAY_START;
						for(auto it = any->begin(); it != any->end(); it++) out << separator_decision[it == any->begin()] << *it;
						out << JSON_CHARS::ARRAY_STOP;
					}
				}	 // namespace
			}		 // namespace detail

			struct json_visitor_deserializer : public detail::visitor_with_stacked_names_and_tokenized_json
			{
				template<typename Any>
				json_visitor_deserializer(const serek::str_v json, Any& output) : visitor_with_stacked_names_and_tokenized_json{json, output}
				{
				}

				template<typename Any>
				json_visitor_deserializer(json_tokenizer::result_t json, Any& output) : visitor_with_stacked_names_and_tokenized_json{json, output}
				{
				}

				template<typename Any>
				visitor_result_t operator()(serek::detail::type_holder<Any>* any)
				{
					this->process(any);
					return true;
				}

			 private:
				template<typename Any>
				void process(serek::detail::type_holder<Any>* any)
				{
					serek::require(!this->empty(), "missing fields!");
					if(!this->empty()) detail::deserial<json_visitor_deserializer>(static_cast<visitor_with_stacked_names_and_tokenized_json&>(*this), reinterpret_cast<Any*>(any));
				}
			};

			static_assert(serek::reqs::visitor_req<json_visitor_deserializer>);

		}	 // namespace json
	}		 // namespace deserial
}	 // namespace serek
