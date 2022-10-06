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

			constexpr const char* get_trimmed_chars() { return " \t\n\r\v\a"; }

			size_t ltrim_pos(const serek::str_v view, const size_t start = 0);
			size_t rtrim_pos(const serek::str_v view, const size_t start = serek::str_v::npos);
			serek::str_v trim(const serek::str_v view);

			/**
			 * @brief This class is responsible for proper tokenization of json, by using callback mechanism
			 *
			 * @note returned sizes refers to string length of @ref json_walker::json stringified json
			 */
			struct json_walker
			{
				/**
				 * @brief Type of function
				 *
				 * @param 1 string view of analyzed json
				 * @param 2 start of item of interest
				 * @param 3 end of item of interest
				 */
				using on_element_found_t = std::function<void(const serek::str_v, const size_t, const size_t)>;

				/**
				 * @brief Type of function
				 *
				 * @param 1 string view of analyzed json
				 * @param 2 start of item of interest
				 * @param 3 end of item of interest
				 * @param 4 type of item of interest
				 */
				using on_complex_item_range_t = std::function<void(const serek::str_v, const size_t, const size_t, const json_element_t)>;

			 private:
				/** @brief default values for items with type: @ref on_element_found_t "on_element_found_t" */
				inline static const on_element_found_t default_call = [](const serek::str_v, const size_t, const size_t) {};

				/** @brief default values for items with type: @ref on_complex_item_range_t "on_complex_item_range_t" */
				inline static const on_complex_item_range_t default_range_call = [](const serek::str_v, const size_t, const size_t, const json_element_t) {};

			 protected:
				/**
				 * @brief This method perform decision about what type of item is given token
				 *
				 * @param start position to start token analysis
				 * @return size_t size of given token
				 */
				size_t walk_over_item(const size_t start) const;

				/**
				 * @brief This method returns size of token which starts and ends with @ref JSON_CHARS::QUOTE "quote"
				 *
				 * @param start position to start token analysis
				 * @return size_t size of given string with quotes
				 */
				size_t walk_over_string(const size_t start) const;

				/**
				 * @brief This method returns size of token which starts with number and is proper number
				 *
				 * @param start position to start token analysis
				 * @return size_t size of given number
				 */
				size_t walk_over_number(const size_t start) const;

				/**
				 * @brief This method returns size of null type
				 *
				 * @param start position to start token analysis
				 * @return size_t size of given null type (4 if correct, 0 otherwise)
				 */
				size_t walk_over_null(const size_t start) const;

				/**
				 * @brief This method returns size of given object and calls callbacks in proper places
				 *
				 * @param start position to start token analysis
				 * @return size_t size of given object
				 */
				size_t walk_over_object(const size_t start) const;

				/**
				 * @brief This method returns size of given object and calls callbacks in proper places
				 *
				 * @param start position to start token analysis
				 * @return size_t size of given array
				 */
				size_t walk_over_array(const size_t start) const;

				/**
				 * @brief This method checks is given @ref JSON_CHARS::QUOTE "quote" is final for given string token
				 *
				 * @param pos position to start token analysis
				 * @return true if given @ref JSON_CHARS::QUOTE "quote" is final
				 * @return false if not
				 */
				bool is_valid_json_string_end(const size_t pos) const;

				/** @brief Holds stringified version of json, that will be tokenized */
				serek::str_v json;

			 public:
				/** @brief Called when key in object is found */
				on_element_found_t on_key_found = default_call;

				/** @brief Called when value in object or array is found */
				on_element_found_t on_value_found = default_call;

				/** @brief Called when object or array starts */
				on_complex_item_range_t on_start = default_range_call;

				/** @brief Called when object or array ends */
				on_complex_item_range_t on_stop = default_range_call;

				/**
				 * @brief Construct a new json walker object
				 *
				 * @param json json string to analyze
				 */
				explicit json_walker(const serek::str_v input_json);

				/**
				 * @brief Starts processing of json string
				 *
				 * @return size_t size of given json string
				 */
				size_t start_processing() const;
			};

		}	 // namespace json
	}		 // namespace deserial
}	 // namespace serek
