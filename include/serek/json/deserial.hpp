/**
 * @file json.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief
 * @version 0.1
 * @date 2022-10-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <map>
#include <optional>

#include <serek/serek.hpp>
#include <serek/json/common.hpp>

namespace serek
{
	namespace deserial
	{
		namespace json
		{
			using serek::json::JSON_CHARS;
			using serek::json::to_char;

			using pre_opened_key_t					= serek::str;
			using pre_opened_item_t					= serek::str;
			using pre_opened_container_object_t = std::map<pre_opened_key_t, pre_opened_item_t>;
			using pre_opened_container_array_t	= std::vector<pre_opened_item_t>;

			enum class json_element_t : uint16_t
			{
				NOT_SET	  = 0,
				NULL_VALUE = 1,

				// int, float, string
				FUNDAMENTAL_TYPE = 2,
				OBJECT_TYPE		  = 3,
				ARRAY_TYPE		  = 4
			};

			struct pre_opened_json_representation_t
			{
				explicit pre_opened_json_representation_t(const json_element_t i_element_type = json_element_t::NOT_SET) : element_type{i_element_type} {}

				json_element_t element_type{json_element_t::NOT_SET};

				pre_opened_item_t item{};
				pre_opened_container_object_t object{};
				pre_opened_container_array_t array{};
			};


		}	 // namespace json
	}		 // namespace deserial
}	 // namespace serek
