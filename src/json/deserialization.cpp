#include <serek/json/deserial.hpp>

namespace serek
{
	namespace deserial
	{
		namespace json
		{
			namespace detail
			{
				json_tokenizer::result_t visitor_with_stacked_names_and_tokenized_json::pop_for_key(const serek::str key)
				{
					// checks
					serek::require<std::equal_to>(this->top(), key);
					serek::require<std::equal_to>(tokenized_json->element_type, json_element_t::OBJECT_TYPE);
					auto iterator = tokenized_json->object.find(key);
					serek::require<std::not_equal_to>(iterator, tokenized_json->object.end());

					// acquiring
					auto result = iterator->second;

					// popping
					tokenized_json->object.erase(iterator);
					this->pop();

					//returning
					return result;
				}
			}	 // namespace detail
		}		 // namespace json
	}			 // namespace deserial
}	 // namespace serek
