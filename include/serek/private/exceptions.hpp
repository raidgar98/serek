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
			exception_base(const char* msg = "unknown");

			virtual const char* what() const noexcept;
			virtual std::string pretty() const noexcept;
			inline friend std::ostream& operator<<(std::ostream& os, const exception_base& ex)
			{
				return os << ex.pretty();
			}

		 private:
			std::unique_ptr<std::string> message;
			std::unique_ptr<std::string> stacktrace;
		};
	}	 // namespace exceptions
}	 // namespace serek