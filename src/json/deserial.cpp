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

			bool is_valid_json_string_end(const serek::str_v json_item, const size_t pos)
			{
				serek::require<std::greater>(pos, 0ul, "position to check has to be greater than 0");
				serek::require<std::less>(pos, json_item.size(), "position out of range");

				return json_item[pos] == to_char(JSON_CHARS::QUOTE) && json_item[pos - 1] != '\\';
			}

			size_t length_of_item(const serek::str_v json, const size_t start)
			{
				size_t length;
				if(length = length_of_null(json, start); length > 0ul) return length;
				else if(length = length_of_number(json, start); length > 0ul)
					return length;
				else if(length = length_of_string_with_quotes(json, start); length > 0ul)
					return length;
				else if(length = length_of_object(json, start); length > 0ul)
					return length;
				else if(length = length_of_array(json, start); length > 0ul)
					return length;
				else
				{
					serek::require(false, "unknown object, invalid json");
					return serek::str_v::npos;
				}
			}

			size_t length_of_string_with_quotes(const serek::str_v json_item, const size_t start)
			{
				if(json_item[start] != to_char(JSON_CHARS::QUOTE)) return 0;

				size_t pos{start + 1ul};
				while(pos < json_item.size() && !is_valid_json_string_end(json_item, pos)) pos = ltrim_pos(json_item, pos + 1);

				serek::require<std::not_equal_to>(serek::str_v::npos, pos, "end of json string not found, invalid json");
				return pos - start + 1ul;
			}

			size_t length_of_number(const serek::str_v json, const size_t start)
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

			size_t length_of_null(const serek::str_v json, const size_t start)
			{
				const static serek::str_v null_string{"null"};
				for(size_t i = 0; i < null_string.size(); ++i)
					if(json[start + i] != null_string[i]) return 0ul;
				return null_string.size();
			}


			size_t length_of_object(const serek::str_v json, const size_t start)
			{
				return serek::str_v::npos;
			}

			size_t length_of_array(const serek::str_v json, const size_t start)
			{
				return serek::str_v::npos;
			}
		}	 // namespace json
	}		 // namespace deserial
}	 // namespace serek
