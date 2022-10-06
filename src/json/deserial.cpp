#include <serek/json/deserial.hpp>

namespace serek
{
	namespace deserial
	{
		namespace json
		{
			size_t ltrim_pos(const serek::str_v view, const size_t start) { return view.find_first_not_of(get_trimmed_chars(), start); }
			size_t rtrim_pos(const serek::str_v view, const size_t start) { return view.find_last_not_of(get_trimmed_chars(), start); }

			serek::str_v trim(const serek::str_v view)
			{
				const size_t ltrim_position = ltrim_pos(view);
				const size_t rtrim_position = rtrim_pos(view) + 1;
				return serek::str_v{view.begin() + ltrim_position, view.begin() + rtrim_position};
			}

			json_walker::json_walker(const serek::str_v input_json) : json{trim(input_json)} {}

			size_t json_walker::start_processing() const { return walk_over_item(0); }

			bool json_walker::is_valid_json_string_end(const size_t pos) const
			{
				serek::require<std::greater>(pos, 0ul, "position to check has to be greater than 0");
				serek::require<std::less>(pos, json.size(), "position out of range");

				return json[pos] == to_char(JSON_CHARS::QUOTE) && json[pos - 1] != '\\';
			}

			size_t json_walker::walk_over_item(const size_t start) const
			{
				size_t length;
				if(length = walk_over_null(start); length > 0ul) return length;
				else if(length = walk_over_number(start); length > 0ul)
					return length;
				else if(length = walk_over_string(start); length > 0ul)
					return length;
				else if(length = walk_over_object(start); length > 0ul)
					return length;
				else if(length = walk_over_array(start); length > 0ul)
					return length;
				else
				{
					serek::require(false, "unknown object, invalid json");
					return serek::str_v::npos;
				}
			}

			size_t json_walker::walk_over_string(const size_t start) const
			{
				if(json[start] != to_char(JSON_CHARS::QUOTE)) return 0;

				size_t pos{start + 1ul};
				while(pos < json.size() && !is_valid_json_string_end(pos)) pos = ltrim_pos(json, pos + 1);

				serek::require<std::not_equal_to>(serek::str_v::npos, pos, "end of json string not found, invalid json");
				return pos - start + 1ul;
			}

			size_t json_walker::walk_over_number(const size_t start) const
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

			size_t json_walker::walk_over_null(const size_t start) const
			{
				const static serek::str_v null_string{"null"};
				for(size_t i = 0; i < null_string.size(); ++i)
					if(json[start + i] != null_string[i]) return 0ul;
				return null_string.size();
			}

			namespace
			{
				struct length_array_helper
				{
					bool end;
					bool value;
					bool coma;

					length_array_helper() { nothing(); }
					virtual ~length_array_helper() = default;

					virtual void nothing()
					{
						end	= false;
						value = false;
						coma	= false;
					}
				};

				struct length_object_helper : public length_array_helper
				{
					bool key;
					bool key_value_separator;

					length_object_helper() { nothing(); }
					virtual ~length_object_helper() = default;

					virtual void nothing() override
					{
						length_array_helper::nothing();
						key					  = false;
						key_value_separator = false;
					}
				};

				struct scope_logger
				{
					explicit scope_logger(const str i_message, const char c) : message{i_message} { std::cout << "started: " << message << " with letter: `" << c << "`" << std::endl; }

					~scope_logger() { std::cout << "finished: " << message << std::endl; }

				 private:
					serek::str message;
				};
			}	 // namespace

			size_t json_walker::walk_over_object(const size_t start) const
			{
				if(json[start] != JSON_CHARS::OBJECT_START) return 0;

				on_start(json, start, serek::str_v::npos, json_element_t::OBJECT_TYPE);
				length_object_helper looking_for{};
				looking_for.key = true;
				looking_for.end = true;

				for(size_t pos = ltrim_pos(json, start + 1ul); pos < json.size(); pos = ltrim_pos(json, pos))
				{
					if(json[pos] == JSON_CHARS::ITEMS_SEPARATOR)
					{
						const scope_logger _{"processing coma", json[pos]};
						serek::require(looking_for.coma, "unexpected coma, invalid json");

						looking_for.nothing();
						looking_for.key = true;
						++pos;
					}
					else if(json[pos] == JSON_CHARS::KEY_VALUE_SEPARATOR)
					{
						const scope_logger _{"processing semicolon", json[pos]};
						serek::require(looking_for.key_value_separator, "unexpected semicolon, invalid json");

						looking_for.nothing();
						looking_for.value = true;
						++pos;
					}
					else if(json[pos] == JSON_CHARS::QUOTE)
					{
						const scope_logger _{"processing quote", json[pos]};
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
						const scope_logger _{"processing end of object", json[pos]};
						serek::require(looking_for.end, "unexpected object end, invalid json");
						const size_t length{pos - start + 1ul};
						on_stop(json, start, length, json_element_t::OBJECT_TYPE);
						return length;
					}
					else
					{
						const scope_logger _{"processing value", json[pos]};
						serek::require(pos != serek::str_v::npos, "unexpected input end, invalid json");
						serek::require(looking_for.value, "unexpected token, or item start, invalid json");

						const size_t length_of_item{walk_over_item(pos)};
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

			size_t json_walker::walk_over_array(const size_t start) const
			{
				if(json[start] != JSON_CHARS::ARRAY_START) return 0;

				on_start(json, start, serek::str_v::npos, json_element_t::ARRAY_TYPE);
				length_array_helper looking_for{};
				looking_for.value = true;
				looking_for.end	= true;

				for(size_t pos = ltrim_pos(json, start + 1ul); pos < json.size(); pos = ltrim_pos(json, pos))
				{
					if(json[pos] == JSON_CHARS::ITEMS_SEPARATOR)
					{
						const scope_logger _{"processing coma", json[pos]};
						serek::require(looking_for.coma, "unexpected coma, invalid json");

						looking_for.nothing();
						looking_for.value = true;
						++pos;
					}
					else if(json[pos] == JSON_CHARS::ARRAY_STOP)
					{
						const scope_logger _{"processing end of array", json[pos]};
						serek::require(looking_for.end, "unexpected array end, invalid json");
						const size_t length{pos - start + 1ul};
						on_stop(json, start, length, json_element_t::ARRAY_TYPE);
						return length;
					}
					else
					{
						const scope_logger _{"processing value", json[pos]};
						serek::require(pos != serek::str_v::npos, "unexpected input end, invalid json");
						serek::require(looking_for.value, "unexpected token, or item start, invalid json");

						const size_t length_of_item{walk_over_item(pos)};
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
		}	 // namespace json
	}		 // namespace deserial
}	 // namespace serek
