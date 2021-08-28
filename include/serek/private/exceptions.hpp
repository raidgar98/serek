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

		struct assert_exception : public exception_base
		{
			using exception_base::exception_base;
		};
		};
	}	 // namespace exceptions

	template<typename exception_t = typename exceptions::assert_exception>
	void require(const bool result, const str_v error_message = "failed on check!")
	{
		if(!result) throw exception_t{error_message};
	}
}	 // namespace serek