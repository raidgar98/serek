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
#include <string_view>

namespace serek
{
	using visitor_result_t = bool;
	using str				  = std::string;
	using str_v				  = std::string_view;
}	 // namespace serek