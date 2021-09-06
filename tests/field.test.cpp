#include <shared/tools.hpp>

namespace
{
	namespace but = boost::ut;
	using namespace but::literals;
	using namespace serek::tests;

	using serek::ffield;
	using serek::field;

	struct test_struct_0
	{
		ffield<int> field_0;
	};

	struct test_struct_1
	{
		ffield<int> field_0;
		field<&test_struct_1::field_0, float> field_1;
	};

	struct test_struct_2 : public test_struct_1
	{
		field<&test_struct_2::field_1, char> field_2;
	};

	struct test_struct_3
	{
		ffield<serek::str> field_0;
		inline friend std::ostream& operator<<(std::ostream& os, const test_struct_3& str) { return os << str.field_0; }
	};

	struct test_struct_4
	{
		ffield<test_struct_3> field_0;
	};

	struct test_struct_5
	{
		ffield<int> field_0;
		int empty_field_1;
		field<&test_struct_5::field_0, float> field_2;
	};

	struct test_struct_6
	{
		ffield<number_t<0>> field_0;
		field<&test_struct_6::field_0, number_t<1>> field_1;
		field<&test_struct_6::field_1, number_t<2>> field_2;
		field<&test_struct_6::field_2, number_t<3>> field_3;
		field<&test_struct_6::field_3, number_t<4>> field_4;
	};

	struct test_struct_7
	{
		ffield<int> field_0;
		field<&test_struct_7::field_0, int> field_1;
		field<&test_struct_7::field_0, int> field_2;
	};

	struct trivial_serializer : public serek::visitors::base_visitor_members
	{
		using base_visitor_members::base_visitor_members;
		using res_t = serek::visitor_result_t;

		template<typename T>
		res_t operator()(T* v)
		{
			(*stream) << *v;
			return true;
		}

		serek::str result() const { return stream->str(); }

	 private:
		std::shared_ptr<std::stringstream> stream{new std::stringstream{}};
	};

	template<auto start_from>
	struct validate_order;

	template<typename test_struct, typename member_t, member_t test_struct::*value>
	struct validate_order<value>
	{
		template<typename... Argv>
		explicit validate_order(const serek::str expected, Argv&&... args)
		{
			test_struct obj{std::forward<Argv>(args)...};
			trivial_serializer visitor{&obj};
			but::expect(serek::visitors::visit(&visitor, &((&obj)->*value)));
			but::expect(expected == visitor.result());
		}
	};

	but::suite proper_order = [] {
		"order_0"_test = [] { validate_order<&test_struct_0::field_0>{"0", 0}; };
		"order_1"_test = [] { validate_order<&test_struct_1::field_1>{"12", 1, 2.0f}; };
		"order_2"_test = [] { validate_order<&test_struct_2::field_2>{"12c", 1, 2.0f, 'c'}; };
		"order_3"_test = [] { validate_order<&test_struct_3::field_0>{"aaa", "aaa"}; };
		"order_4"_test = [] { validate_order<&test_struct_4::field_0>{"aaa", "aaa" /* implicit conversion to test_struct_3 */}; };
		"order_5"_test = [] { validate_order<&test_struct_5::field_2>{"13", 1, 2, 3.0f}; };
		"order_6"_test = [] { validate_order<&test_struct_6::field_4>{"01234"}; };
		"order_7"_test = [] { validate_order<&test_struct_7::field_2>{"02", 0, 1, 2}; };
	};


	template<typename T1, typename T2>
	inline constexpr void compare_size()
	{
		but::expect(but::eq(sizeof(T1), sizeof(T2)));
	}

	but::suite proper_size = [] {
		struct test_struct_6_helper
		{
			number_t<0> n0;
			number_t<1> n1;
			number_t<2> n2;
			number_t<3> n3;
			number_t<4> n4;
		};

		"sizeof_0"_test = [] { compare_size<test_struct_0, std::tuple<int>>(); };
		"sizeof_1"_test = [] { compare_size<test_struct_1, std::tuple<int, float>>(); };
		"sizeof_2"_test = [] { compare_size<test_struct_2, std::tuple<int, float, char>>(); };
		"sizeof_3"_test = [] { compare_size<test_struct_3, std::tuple<serek::str>>(); };
		"sizeof_4"_test = [] { compare_size<test_struct_4, std::tuple<test_struct_3>>(); };
		"sizeof_5"_test = [] { compare_size<test_struct_5, std::tuple<int, float, int>>(); };
		"sizeof_6"_test = [] { compare_size<test_struct_6, test_struct_6_helper>(); };
		"sizeof_7"_test = [] { compare_size<test_struct_7, std::tuple<int, int, int>>(); };
	};

}	 // namespace