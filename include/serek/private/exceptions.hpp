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
			std::shared_ptr<str> message;
			std::shared_ptr<str> stacktrace;
		};

		struct assert_exception : public exception_base
		{
			using exception_base::exception_base;
		};

		struct pointer_was_null_exception : public exception_base
		{
			using exception_base::exception_base;
		};
	}	 // namespace exceptions

	template<reqs::throwable_req exception_t = typename exceptions::assert_exception>
	void require(const bool result, const str_v error_message = "failed on check!")
	{
		if(!result) throw exception_t{error_message};
	}

	template<reqs::comparable_as_pointer_req pointer_t>
	void require(const pointer_t& result, const str_v error_message = "given pointer was null!")
	{
		if(result == nullptr) throw exceptions::pointer_was_null_exception{error_message};
	}
}	 // namespace serek

std::ostream& operator<<(std::ostream& os, const serek::exceptions::exception_base& ex);