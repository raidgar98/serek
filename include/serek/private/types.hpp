/**
 * @file types.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains aliases for commonly used types
 * @version 0.1
 * @date 2021-08-30
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

#include <string>
#include <sstream>
#include <string_view>

#include <boost/type_index.hpp>

namespace serek
{
	using visitor_result_t = bool;
	using str				  = std::string;
	using str_v				  = std::string_view;
	using sstr				  = std::stringstream;

	template<typename Any>
	[[nodiscard]] inline str type_name()
	{
		return boost::typeindex::type_id<Any>().pretty_name();
	}
}	 // namespace serek
