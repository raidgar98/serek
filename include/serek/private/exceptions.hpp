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

		struct comprasion_fail_exception : public exception_base
		{
			explicit comprasion_fail_exception(const str_v i_what, const str& i_comprasion_result = str{}) : exception_base{i_what}, comprasion_result{new str{i_comprasion_result}} {}

			virtual str pretty() const noexcept override;

			template<typename operator_t, typename T>
			static str format_fail_comprasion(const T& l_value, const T& r_value)
			{
				std::stringstream ss;
				ss << "comprasion via functor: " << boost::typeindex::type_id<operator_t>().pretty_name() << "{}( `" << l_value << "`, `" << r_value << "` ) returned false!";
				return ss.str();
			}

		 private:
			std::shared_ptr<str> comprasion_result;
		};
	}	 // namespace exceptions

	template<reqs::throwable_req exception_t = typename exceptions::assert_exception>
	void require(const bool result, const str_v error_message = "failed on check!")
	{
		if(!result) throw exception_t{error_message};
	}

	template<template<typename T> typename operator_t, typename T>
	void require(const T& l_value, const T& r_value, const str_v error_message = "comprasion failed!")
	{
		if(!operator_t<T>{}(l_value, r_value)) throw typename exceptions::comprasion_fail_exception{
			error_message, exceptions::comprasion_fail_exception::template format_fail_comprasion<operator_t<T>>(l_value, r_value)
		};
	}

	template<reqs::comparable_as_pointer_req pointer_t>
	void require(const pointer_t& result, const str_v error_message = "given pointer was null!")
	{
		if(result == nullptr) throw exceptions::pointer_was_null_exception{error_message};
	}
}	 // namespace serek

std::ostream& operator<<(std::ostream& os, const serek::exceptions::exception_base& ex);