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
	}	 // namespace packed
}	 // namespace example_structs
