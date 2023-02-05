#pragma once

#include <shared/struct_set.hpp>

namespace example_structs
{
	using serek::ffield;
	using serek::field;
	using namespace serek::tests;

	namespace packed
	{
		namespace es  = example_structs;
		namespace pes = es::polimorphic;

		using test_struct_0_packed = serek::pack<&es::test_struct_0::field_0>;
		using test_struct_1_packed = serek::pack<&es::test_struct_1::field_1>;
		using test_struct_2_packed = serek::pack<&es::test_struct_2::field_2>;
		using test_struct_3_packed = serek::pack<&es::test_struct_3::field_0>;
		using test_struct_4_packed = serek::pack<&es::test_struct_4<test_struct_3_packed>::field_0>;
		using test_struct_5_packed = serek::pack<&es::test_struct_5::field_2>;
		using test_struct_6_packed = serek::pack<&es::test_struct_6::field_4>;
		using test_struct_7_packed = serek::pack<&es::test_struct_7::field_2>;

		using some_virtual_base_class_packed		= serek::pack<&pes::some_virtual_base_class::field_0>;
		using some_virtual_child_class_packed		= serek::pack<&pes::some_virtual_child_class::field_1>;
		using some_non_virtual_base_class_packed	= serek::pack<&pes::some_non_virtual_base_class::field_0>;
		using some_non_virtual_child_class_packed = serek::pack<&pes::some_non_virtual_child_class::field_1>;

		struct some_virtual_base_class_fixed
		{
			serek::ffield<test_struct_0_packed> field_0;
			virtual int do_something() const { return 0; };
			virtual ~some_virtual_base_class_fixed() {}
		};
		using some_virtual_base_class_fixed_packed = serek::pack<&some_virtual_base_class_fixed::field_0>;

		struct some_virtual_child_class_fixed : public some_virtual_base_class_fixed
		{
			serek::field<&some_virtual_child_class_fixed::field_0, some_virtual_base_class_fixed_packed> field_1;
			virtual int do_something() const override { return 1; };
		};
		using some_virtual_child_class_fixed_packed = serek::pack<&some_virtual_child_class_fixed::field_1>;

		namespace array
		{
			struct test_array_01
			{
				serek::ffield<std::vector<int>> field_0;
			};
			using test_array_01_packed = serek::pack<&test_array_01::field_0>;

			struct test_array_02
			{
				serek::ffield<int> field_0;
				serek::field<&test_array_02::field_0, std::vector<int>> field_1;
			};
			using test_array_02_packed = serek::pack<&test_array_02::field_1>;

			struct test_array_03
			{
				serek::ffield<std::vector<test_struct_0_packed>> field_0;
			};
			using test_array_03_packed = serek::pack<&test_array_03::field_0>;

			struct test_array_04
			{
				serek::ffield<int> field_0;
				serek::field<&test_array_04::field_0, std::vector<test_struct_0_packed>> field_1;
			};
			using test_array_04_packed = serek::pack<&test_array_04::field_1>;

		}	 // namespace array
	}		 // namespace packed
}	 // namespace example_structs
