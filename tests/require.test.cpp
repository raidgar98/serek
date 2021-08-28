#include <shared/tools.hpp>

namespace
{
	namespace but = boost::ut;
	using namespace but::literals;
	namespace except = serek::exceptions;

	but::suite no_fail_basic = [] {
		"true_given_0"_test = [] { serek::require(true); };
		"true_given_1"_test = [] { serek::require(1 == 1); };
		"true_given_2"_test = [] { serek::require(1 != 2); };
	};


	but::suite fail_basic = [] {
		"false_given_0"_test = [] { but::expect(but::throws<except::assert_exception>([] { serek::require(false); })); };
		"false_given_1"_test = [] { but::expect(but::throws<except::assert_exception>([] { serek::require(1 == 2); })); };
		"false_given_2"_test = [] { but::expect(but::throws<except::assert_exception>([] { serek::require(1 != 1); })); };
	};

	but::suite pointers = [] {
		std::unique_ptr<int> uptr{new int{}};
		std::shared_ptr<int> sptr{new int{}};
		int* raw = uptr.get();

		"no_raise_raw"_test			= [&] { serek::require(raw); };
		"no_raise_unique_ptr"_test = [&] { serek::require(uptr); };
		"no_raise_shared_ptr"_test = [&] { serek::require(sptr); };

		uptr.reset();
		sptr.reset();
		raw = nullptr;

		"raise_raw"_test	= [&] { but::expect(but::throws<except::pointer_was_null_exception>([&] { serek::require(raw); })); };
		"raise_uptr"_test = [&] { but::expect(but::throws<except::pointer_was_null_exception>([&] { serek::require(uptr); })); };
		"raise_sptr"_test = [&] { but::expect(but::throws<except::pointer_was_null_exception>([&] { serek::require(sptr); })); };
	};

	but::suite messages = [] {
		constexpr serek::str_v message{"my custom message"};
		const auto get_exception = []() -> except::assert_exception {
			try
			{
				serek::require(false, message);
			}
			catch(const except::assert_exception& ex)
			{
				return ex;
			}

			return except::assert_exception{serek::str_v{}};
		};

		"proper_message"_test	= [&] { but::expect(but::eq(message, get_exception().what())); };
		"message_contains"_test = [&] { but::expect(but::neq(get_exception().pretty().find(message), serek::str::npos)); };
	};

	template<typename exception_t>
	auto perform_throw()
	{
		return but::throws<exception_t>([] { serek::template require<exception_t>(false); });
	}

	but::suite custom_exception = [] {
		struct example_exception_child : public except::exception_base
		{
			using except::exception_base::exception_base;
		};

		struct example_exception_native
		{
			explicit example_exception_native(serek::str_v) {}
		};

		"custom_exception"_test		  = [] { but::expect(perform_throw<example_exception_child>()); };
		"full_custom_exception"_test = [] { but::expect(perform_throw<example_exception_native>()); };
	};
}	 // namespace