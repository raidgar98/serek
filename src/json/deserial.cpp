#include <serek/json/deserial.hpp>

namespace serek
{
	namespace deserial
	{
		namespace json
		{
			using serek::json::JSON_CHARS;

			size_t ltrim_pos(const serek::str_v view, const size_t start) { return view.find_first_not_of(get_trimmed_chars(), start); }
			size_t rtrim_pos(const serek::str_v view, const size_t start) { return view.find_last_not_of(get_trimmed_chars(), start); }

			serek::str_v trim(const serek::str_v view)
			{
				const size_t ltrim_position = ltrim_pos(view);
				const size_t rtrim_position = rtrim_pos(view) + 1;
				return serek::str_v{view.begin() + ltrim_position, view.begin() + rtrim_position};
			}

			json_walker::json_walker(const serek::str_v input_json) : json{trim(input_json)} {}

			size_t json_walker::start_processing() { return std::get<0>(walk_over_item(0)); }

			bool json_walker::is_valid_json_string_end(const size_t pos)
			{
				serek::require<std::greater>(pos, 0ul, "position to check has to be greater than 0");
				serek::require<std::less>(pos, json.size(), "position out of range");

				return json[pos] == to_char(JSON_CHARS::QUOTE) && json[pos - 1] != '\\';
			}

			std::tuple<size_t, json_element_t> json_walker::walk_over_item(const size_t start)
			{
				size_t length;

				if(length = walk_over_null(start); length > 0ul) return {length, json_element_t::FUNDAMENTAL_TYPE};
				else if(length = walk_over_number(start); length > 0ul)
					return {length, json_element_t::FUNDAMENTAL_TYPE};
				else if(length = walk_over_string(start); length > 0ul)
					return {length, json_element_t::FUNDAMENTAL_TYPE};
				else if(length = walk_over_object(start); length > 0ul)
					return {length, json_element_t::OBJECT_TYPE};
				else if(length = walk_over_array(start); length > 0ul)
					return {length, json_element_t::ARRAY_TYPE};
				else
				{
					serek::require(false, "unknown object, invalid json");
					return {serek::str_v::npos, json_element_t::NOT_SET};
				}
			}

			size_t json_walker::walk_over_string(const size_t start)
			{
				if(json[start] != to_char(JSON_CHARS::QUOTE)) return 0;

				size_t pos{start + 1ul};
				while(pos < json.size() && !is_valid_json_string_end(pos)) pos = ltrim_pos(json, pos + 1);

				serek::require<std::not_equal_to>(serek::str_v::npos, pos, "end of json string not found, invalid json");
				return pos - start + 1ul;
			}

			size_t json_walker::walk_over_number(const size_t start)
			{
				if(!(std::isdigit(json[start]) || json[start] == '-')) return 0;

				size_t number_end_pos{json.find_first_not_of("-0123456789.eE", start)};
				number_end_pos = (number_end_pos == serek::str_v::npos ? json.size() : number_end_pos);

				// validation
				const serek::str_v num{json.begin() + start, json.begin() + number_end_pos};
				serek::require<std::less_equal>(std::count(num.begin(), num.end(), '-'), 2l);
				for(char c: ".Ee") serek::require<std::less_equal>(std::count(num.begin(), num.end(), c), 1l);

				return number_end_pos - start;
			}

			size_t json_walker::walk_over_null(const size_t start)
			{
				const static serek::str_v null_string{"null"};
				for(size_t i = 0; i < null_string.size(); ++i)
					if(json[start + i] != null_string[i]) return 0ul;
				return null_string.size();
			}

			size_t json_walker::walk_over_object(const size_t start)
			{
				if(json[start] != JSON_CHARS::OBJECT_START) return 0;

				on_start(json, start, serek::str_v::npos, json_element_t::OBJECT_TYPE);
				detail::length_object_helper looking_for{};
				looking_for.key = true;
				looking_for.end = true;

				for(size_t pos = ltrim_pos(json, start + 1ul); pos < json.size(); pos = ltrim_pos(json, pos))
				{
					if(json[pos] == JSON_CHARS::ITEMS_SEPARATOR)
					{
						serek::require(looking_for.coma, "unexpected coma, invalid json");

						looking_for.nothing();
						looking_for.key = true;
						++pos;
					}
					else if(json[pos] == JSON_CHARS::KEY_VALUE_SEPARATOR)
					{
						serek::require(looking_for.key_value_separator, "unexpected semicolon, invalid json");

						looking_for.nothing();
						looking_for.value = true;
						++pos;
					}
					else if(json[pos] == JSON_CHARS::QUOTE)
					{
						serek::require(looking_for.key || looking_for.value, "unexpected quote, invalid json");
						const size_t length_of_item{walk_over_string(pos)};
						if(looking_for.key)
						{
							on_key_found(json, pos, length_of_item);
							looking_for.nothing();
							looking_for.key_value_separator = true;
						}
						else
						{
							on_value_found(json, pos, length_of_item);
							looking_for.nothing();
							looking_for.coma = true;
							looking_for.end  = true;
						}
						pos += length_of_item;
					}
					else if(json[pos] == JSON_CHARS::OBJECT_STOP)
					{
						serek::require(looking_for.end, "unexpected object end, invalid json");
						const size_t length{pos - start + 1ul};
						on_stop(json, start, length, json_element_t::OBJECT_TYPE);
						return length;
					}
					else
					{
						serek::require(pos != serek::str_v::npos, "unexpected input end, invalid json");
						serek::require(looking_for.value, "unexpected token, or item start, invalid json");

						const auto [length_of_item, type_of_item] = walk_over_item(pos);
						on_value_found(json, pos, length_of_item);
						pos += length_of_item;

						looking_for.nothing();
						looking_for.coma = true;
						looking_for.end  = true;
					}
				}

				serek::require(false, "unexpected finish of processing of object, invalid json");
				return serek::str_v::npos;
			}

			size_t json_walker::walk_over_array(const size_t start)
			{
				if(json[start] != JSON_CHARS::ARRAY_START) return 0;

				on_start(json, start, serek::str_v::npos, json_element_t::ARRAY_TYPE);
				detail::length_array_helper looking_for{};
				looking_for.value = true;
				looking_for.end	= true;

				for(size_t pos = ltrim_pos(json, start + 1ul); pos < json.size(); pos = ltrim_pos(json, pos))
				{
					if(json[pos] == JSON_CHARS::ITEMS_SEPARATOR)
					{
						serek::require(looking_for.coma, "unexpected coma, invalid json");

						looking_for.nothing();
						looking_for.value = true;
						++pos;
					}
					else if(json[pos] == JSON_CHARS::ARRAY_STOP)
					{
						serek::require(looking_for.end, "unexpected array end, invalid json");
						const size_t length{pos - start + 1ul};
						on_stop(json, start, length, json_element_t::ARRAY_TYPE);
						return length;
					}
					else
					{
						serek::require(pos != serek::str_v::npos, "unexpected input end, invalid json");
						serek::require(looking_for.value, "unexpected token, or item start, invalid json");

						const auto [length_of_item, type_of_item] = walk_over_item(pos);
						on_value_found(json, pos, length_of_item);
						pos += length_of_item;


						looking_for.nothing();
						looking_for.coma = true;
						looking_for.end  = true;
					}
				}

				serek::require(false, "unexpected finish of processing of array, invalid json");
				return serek::str_v::npos;
			}

			void json_walker::on_key_found(const serek::str_v, const size_t, const size_t) {}
			void json_walker::on_value_found(const serek::str_v, const size_t, const size_t) {}
			void json_walker::on_start(const serek::str_v, const size_t, const size_t, const json_element_t) {}
			void json_walker::on_stop(const serek::str_v, const size_t, const size_t, const json_element_t) {}

			json_tokenizer::json_tokenizer(const serek::str_v input_json) : json_walker{input_json} { json_walker::start_processing(); }

			void json_tokenizer::on_key_found(const serek::str_v view, const size_t start, const size_t length)
			{
				auto& repr		= json_depth.top().repr;
				auto& last_key = json_depth.top().last_key;

				const static size_t quote_width = sizeof(serek::str_v::value_type);
				serek::require<std::equal_to>(repr->element_type, json_element_t::OBJECT_TYPE);
				serek::require(!last_key.has_value(), "last_key must be not set! double key?");

				const auto result = json_depth.top().repr->object.emplace(serek::str{view.begin() + start + quote_width, view.begin() + start + length - quote_width},
																							 make_json<json_element_t::NOT_SET>());
				serek::require(result.second, "key was not inserted!");
				last_key = result.first->first;
			}

			void json_tokenizer::on_value_found(const serek::str_v view, const size_t start, const size_t length)
			{
				auto& repr		= json_depth.top().repr;
				auto& last_key = json_depth.top().last_key;

				const serek::str_v item{view.begin() + start, view.begin() + start + length};

				if(repr->element_type == json_element_t::OBJECT_TYPE)
				{
					if(last_key.has_value())
					{
						auto ptr = make_json<json_element_t::FUNDAMENTAL_TYPE>();
						auto itr = repr->object.emplace(*last_key, ptr);
						if(itr.second) { itr.first->second->item = item; }
						else
							serek::require<std::not_equal_to>(itr.first->second->element_type, json_element_t::NOT_SET, "object was not set!");
					}
				}
				else if(repr->element_type == json_element_t::ARRAY_TYPE)
				{
					auto ptr	 = make_json<json_element_t::FUNDAMENTAL_TYPE>();
					ptr->item = item;
					repr->array.emplace_back(ptr);
				}
				else if(repr->element_type == json_element_t::FUNDAMENTAL_TYPE)
				{
					repr->item = item;
					last_key.reset();
				}
			}

			void json_tokenizer::on_start(const serek::str_v, const size_t, const size_t, const json_element_t json_element_type)
			{
				json_depth.emplace(json_depth_frame{json_element_type});
				json_depth.top().repr->element_type = json_element_type;
			}

			void json_tokenizer::on_stop(const serek::str_v view, const size_t start, const size_t length, const json_element_t json_element_type)
			{
				if(json_depth.size() == 1) return;

				json_depth_frame frame{json_depth.top()};
				json_depth.pop();

				auto& repr		= json_depth.top().repr;
				auto& last_key = json_depth.top().last_key;

				serek::require(repr, "repr cannot be nullptr");

				if(repr->element_type == json_element_t::OBJECT_TYPE)
				{
					serek::require(last_key);
					auto it = repr->object.find(*last_key);
					serek::require<std::not_equal_to>(it, repr->object.end(), "object was not inserted!");
					it->second = frame.repr;
					last_key.reset();
				}
				else if(repr->element_type == json_element_t::ARRAY_TYPE)
				{
					repr->array.pop_back();
					repr->array.emplace_back(frame.repr);
				}
				else
					serek::require(false, "only acceptable types are arrays and objects");
			}
		}	 // namespace json
	}		 // namespace deserial
}	 // namespace serek
