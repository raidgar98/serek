#pragma once

#include <stdexcept>
#include <memory>

#include <serek/private/requirements.hpp>

namespace serek
{
	namespace exceptions
	{
		struct exception_base : public std::exception
		{
			exception_base(str_v msg = "unknown");

			virtual const char* what() const noexcept;
			virtual str pretty() const noexcept;

		 private:
			std::unique_ptr<str> message;
			std::unique_ptr<str> stacktrace;
		};
		};
	}	 // namespace exceptions
}	 // namespace serek