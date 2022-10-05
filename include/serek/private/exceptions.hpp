/**
 * @file exceptions.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains declarations of exception mechanism
 * @version 0.1
 * @date 2021-08-29
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

#include <memory>
#include <stdexcept>
#include <sstream>
#include <functional>

#include <boost/type_index.hpp>

#include <serek/private/requirements.hpp>

namespace serek
{
	/**
	 * @brief Contains declarations and definitions of exception structs
	 */
	namespace exceptions
	{
		/**
		 * @brief base class for exceptions
		 */
		struct exception_base : public std::exception
		{
			/**
			 * @brief Construct a new exception base object
			 *
			 * @param msg message to print on what()
			 */
			explicit exception_base(str_v msg = "unknown");

			/**
			 * @brief accessor to `message`
			 *
			 * @return const char*
			 */
			virtual const char* what() const noexcept;

			/**
			 * @brief returns `exception_base::what()` with stacktrace
			 *
			 * @return str
			 */
			virtual str pretty() const noexcept;

		 private:
			std::shared_ptr<str> message;
			std::shared_ptr<str> stacktrace;
		};

		/**
		 * @brief most common exception throw
		 */
		struct assert_exception : public exception_base
		{
			using exception_base::exception_base;
		};

		/**
		 * @brief should be thrown when given pointer is set to null, when non-null pointer is required
		 */
		struct pointer_was_null_exception : public exception_base
		{
			using exception_base::exception_base;
		};

		/**
		 * @brief same as assert exception, but additionally prints additional info about compared values (values and comparison functor)
		 */
		struct comparison_fail_exception : public assert_exception
		{
			/**
			 * @brief Construct a new comparison fail exception object
			 *
			 * @param i_what forwarded to parent (what())
			 * @param i_comparison_result string to set as comparison result
			 */
			explicit comparison_fail_exception(const str_v i_what, const str& i_comparison_result = str{}) : assert_exception{i_what}, comparison_result{new str{i_comparison_result}}
			{
			}

			/**
			 * @brief overrides default pretty, by preinserting `i_comparison_result`
			 *
			 * @return str
			 */
			virtual str pretty() const noexcept override;

		 private:
			std::shared_ptr<str> comparison_result;
		};

		namespace
		{
			/**
			 * @brief generates message of failed comparison
			 *
			 * @tparam operator_t functor type
			 * @tparam T type of element that was compared
			 * @param l_value left operand
			 * @param r_value right operand
			 * @return str
			 */
			template<typename operator_t, typename T>
			str format_fail_comparison(const T& l_value, const T& r_value)
			{
				std::stringstream ss;
				ss << "comparison via functor: " << boost::typeindex::template type_id<operator_t>().pretty_name() << "{}( `" << l_value << "`, `" << r_value << "` ) returned false!";
				return ss.str();
			}
		}	 // namespace
	}		 // namespace exceptions

	/**
	 * @brief alternative to assert
	 *
	 * @tparam exceptions::assert_exception exception type to throw
	 * @param result result from some action
	 * @param error_message message to forwrd to exception
	 */
	template<reqs::throwable_req exception_t = typename exceptions::assert_exception>
	void require(const bool result, const str_v error_message = "failed on check!")
	{
		if(!result) throw exception_t{error_message};
	}

	/**
	 * @brief overload of above, allows additionally print given values
	 *
	 * @tparam operator_t functor that will be used for comparison ( @see std::less )
	 * @tparam T type of compared elements
	 * @param l_value left operand
	 * @param r_value right operand
	 * @param error_message message to forwrd to exception
	 */
	template<template<typename T> typename operator_t, typename T>
	void require(const T& l_value, const T& r_value, const str_v error_message = "comparison failed!")
	{
		using operator_with_type = operator_t<T>;
		if(!operator_with_type{}(l_value, r_value))
			throw typename exceptions::comparison_fail_exception{error_message, exceptions::template format_fail_comparison<operator_with_type>(l_value, r_value)};
	}

	/**
	 * @brief overload of above, specialized for checking pointers
	 *
	 * @tparam pointer_t type of pointer to validate
	 * @param ptr pointer to check
	 * @param error_message message to forwrd to exception
	 */
	template<reqs::comparable_as_pointer_req pointer_t>
	void require(const pointer_t& ptr, const str_v error_message = "given pointer was null!")
	{
		if(ptr == nullptr) throw exceptions::pointer_was_null_exception{error_message};
	}
}	 // namespace serek

/**
 * @brief operator to easly print exceptions
 *
 * @param os stream
 * @param ex exception
 * @return std::ostream&
 */
std::ostream& operator<<(std::ostream& os, const serek::exceptions::exception_base& ex);
