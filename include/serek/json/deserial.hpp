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
#include <tuple>

#include <serek/serek.hpp>
#include <serek/json/common.hpp>

namespace serek
{
	namespace deserial
	{
		namespace json
		{
			struct pre_opened_json_representation_t;

			using pre_opened_key_t					= serek::str;
			using pre_opened_value_t				= serek::str;
			using pre_opened_item_t					= std::shared_ptr<pre_opened_json_representation_t>;
			using pre_opened_container_object_t = std::map<pre_opened_key_t, pre_opened_item_t>;
			using pre_opened_container_array_t	= std::vector<pre_opened_item_t>;

			namespace detail
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
			}	 // namespace detail

			enum class json_element_t : uint16_t
			{
				NOT_SET = 0,

				// int, float, string
				FUNDAMENTAL_TYPE = 2,
				OBJECT_TYPE		  = 3,
				ARRAY_TYPE		  = 4
			};

			struct pre_opened_json_representation_t
			{
				explicit pre_opened_json_representation_t(const json_element_t i_element_type = json_element_t::NOT_SET) : element_type{i_element_type} {}

				json_element_t element_type{json_element_t::NOT_SET};

				pre_opened_value_t item{};
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
			 protected:
				/**
				 * @brief This method perform decision about what type of item is given token
				 *
				 * @param start position to start token analysis
				 * @return std::tuple<size_t, json_element_t> size of given token and type of walked element
				 */
				std::tuple<size_t, json_element_t> walk_over_item(const size_t start);

				/**
				 * @brief This method returns size of token which starts and ends with @ref JSON_CHARS::QUOTE "quote"
				 *
				 * @param start position to start token analysis
				 * @return size_t size of given string with quotes
				 */
				size_t walk_over_string(const size_t start);

				/**
				 * @brief This method returns size of token which starts with number and is proper number
				 *
				 * @param start position to start token analysis
				 * @return size_t size of given number
				 */
				size_t walk_over_number(const size_t start);

				/**
				 * @brief This method returns size of null type
				 *
				 * @param start position to start token analysis
				 * @return size_t size of given null type (4 if correct, 0 otherwise)
				 */
				size_t walk_over_null(const size_t start);

				/**
				 * @brief This method returns size of given object and calls callbacks in proper places
				 *
				 * @param start position to start token analysis
				 * @return size_t size of given object
				 */
				size_t walk_over_object(const size_t start);

				/**
				 * @brief This method returns size of given object and calls callbacks in proper places
				 *
				 * @param start position to start token analysis
				 * @return size_t size of given array
				 */
				size_t walk_over_array(const size_t start);

				/**
				 * @brief This method checks is given @ref JSON_CHARS::QUOTE "quote" is final for given string token
				 *
				 * @param pos position to start token analysis
				 * @return true if given @ref JSON_CHARS::QUOTE "quote" is final
				 * @return false if not
				 */
				bool is_valid_json_string_end(const size_t pos);

				/** @brief Holds stringified version of json, that will be tokenized */
				serek::str_v json;

			 public:
				/** @brief Called when key in object is found */
				virtual void on_key_found(const serek::str_v json, const size_t start, const size_t length);

				/** @brief Called when value in object or array is found */
				virtual void on_value_found(const serek::str_v json, const size_t start, const size_t length);

				/** @brief Called when object or array starts */
				virtual void on_start(const serek::str_v json, const size_t start, const size_t length, const json_element_t json_element_type);

				/** @brief Called when object or array ends */
				virtual void on_stop(const serek::str_v json, const size_t start, const size_t length, const json_element_t json_element_type);

				virtual ~json_walker() = default;

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
				size_t start_processing();
			};

			struct json_tokenizer : public json_walker
			{
				json_tokenizer(const serek::str_v input_json);

			 public:
				/** @brief Called when key in object is found */
				virtual void on_key_found(const serek::str_v json, const size_t start, const size_t length) override;

				/** @brief Called when value in object or array is found */
				virtual void on_value_found(const serek::str_v json, const size_t start, const size_t length) override;

				/** @brief Called when object or array starts */
				virtual void on_start(const serek::str_v json, const size_t start, const size_t length, const json_element_t json_element_type) override;

				/** @brief Called when object or array ends */
				virtual void on_stop(const serek::str_v json, const size_t start, const size_t length, const json_element_t json_element_type) override;

				std::shared_ptr<pre_opened_json_representation_t> get_result() const
				{
					serek::require<std::equal_to>(1ul, json_depth.size(), "there should be exactly one result");
					return json_depth.top().repr;
				}

			 private:
				struct json_depth_frame
				{
					json_depth_frame(const json_element_t type) : repr{std::make_shared<pre_opened_json_representation_t>(type)} {}

					std::shared_ptr<pre_opened_json_representation_t> repr;
					std::optional<serek::str> last_key = std::optional<serek::str>{};
				};

				std::stack<json_tokenizer::json_depth_frame> json_depth{};

				template<json_element_t type>
				auto make_json()
				{
					auto ptr = std::make_shared<pre_opened_json_representation_t>(type);
					return ptr;
				}
			};

		}	 // namespace json
	}		 // namespace deserial
}	 // namespace serek
