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
					void fill(Any& output, const serek::str_v value)
					{
						output = static_cast<Any>(std::atoll(value.data()));
					}

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

					template<reqs::visitor_req vis_t, typename element_t>
						requires serek::requirements::fundamental_req<element_t> || reqs::string_type_req<element_t>
					void deserial_array_element(json_tokenizer::result_t in, element_t& out)
					{
						serek::require<std::equal_to>(in->element_type, json_element_t::FUNDAMENTAL_TYPE);
						fill(out, in->item);
					}

					template<reqs::visitor_req vis_t, auto X>
					void deserial_array_element(json_tokenizer::result_t in, serek::detail::pack_impl<X>& out)
					{
						serek::require<std::equal_to>(in->element_type, json_element_t::OBJECT_TYPE);

						vis_t visitor{in, out};
						serek::visitors::visit(&visitor, &out);
					}

					// this is array deserialization
					template<reqs::visitor_req vis_t, template<typename T, typename... Argv> typename collection_t, typename element_t, typename... other_container_args>
						requires reqs::iterable_req<collection_t<element_t, other_container_args...>>
					void deserial(visitor_with_stacked_names_and_tokenized_json& json, collection_t<element_t, other_container_args...>* output)
					{
						serek::require(output);

						const auto json_iterable = json.pop_for_key(json.top());
						serek::require<std::equal_to>(json_iterable->element_type, json_element_t::ARRAY_TYPE);

						const size_t pre_reserve_size{json_iterable->array.size()};
						output->reserve(pre_reserve_size);
						for(auto it: json_iterable->array)
						{
							// TODO: insert default, and operate on inserted element
							element_t item_output;
							deserial_array_element<vis_t>(it, item_output);
							output->emplace_back(item_output);
						}
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

			template<typename T>
			void deserialize(const serek::str_v json, T& output)
			{
				serek::deserial::json::json_visitor_deserializer deserial{json, output};
				serek::visitors::visit(&deserial, &output);
			}

			template<typename T>
			T deserialize(const serek::str_v json)
			{
				T output;
				deserialize(json, output);
				return output;
			}

		}	 // namespace json
	}		 // namespace deserial
}	 // namespace serek
