/**
 * @file common.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief
 * @version 0.1
 * @date 2022-10-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

namespace serek
{
	namespace json
	{
		enum JSON_CHARS
		{
			OBJECT_START = '{',
			OBJECT_STOP	 = '}',

			ARRAY_START = '[',
			ARRAY_STOP	= ']',

			KEY_VALUE_SEPARATOR = ':',
			ITEMS_SEPARATOR	  = ',',

			QUOTE = '"'
		};

		constexpr inline char to_char(const JSON_CHARS c) { return static_cast<char>(c); }
	}	 // namespace json
}	 // namespace serek
