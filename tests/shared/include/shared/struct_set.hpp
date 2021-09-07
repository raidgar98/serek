#pragma once

#include <shared/tools.hpp>

namespace example_structs
{
	using serek::ffield;
	using serek::field;
	using namespace serek::tests;


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


}	 // namespace example_structs