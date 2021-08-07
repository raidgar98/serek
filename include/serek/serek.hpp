#pragma once

#include "private/field.hpp"

namespace serek
{
	// using field

	struct ___null_t
	{
		/** @brief terminator */
		bool accept(void*) { return false; }
	};

	/**
	 * @brief serialization helper. all it does is to append null_t as member (which mean end of serialization)
	*/
	struct serial_helper_t
	{
		___null_t _{};
	};
}	 // namespace serek
