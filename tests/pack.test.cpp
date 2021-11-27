#include <shared/tools.hpp>
#include <shared/struct_set.hpp>

namespace
{
	namespace but = boost::ut;
	using namespace but::literals;

	namespace detail
	{
		namespace es  = example_structs;
		namespace pes = es::polimorphic;

		using test_struct_0_packed = serek::pack<&es::test_struct_0::field_0>;
		using test_struct_1_packed = serek::pack<&es::test_struct_1::field_1>;
		using test_struct_2_packed = serek::pack<&es::test_struct_2::field_2>;
		using test_struct_3_packed = serek::pack<&es::test_struct_3::field_0>;
		using test_struct_4_packed = serek::pack<&es::test_struct_4::field_0>;
		using test_struct_5_packed = serek::pack<&es::test_struct_5::field_2>;
		using test_struct_6_packed = serek::pack<&es::test_struct_6::field_4>;
		using test_struct_7_packed = serek::pack<&es::test_struct_7::field_2>;

		using some_virtual_base_class_packed		= serek::pack<&pes::some_virtual_base_class::field_0>;
		using some_virtual_child_class_packed		= serek::pack<&pes::some_virtual_child_class::field_1>;
		using some_non_virtual_base_class_packed	= serek::pack<&pes::some_non_virtual_base_class::field_0>;
		using some_non_virtual_child_class_packed = serek::pack<&pes::some_non_virtual_child_class::field_1>;
	}	 // namespace detail

	but::suite proper_size = [] {
		using namespace detail;
		using namespace serek::tests;

		"sizeof_0"_test = [] { compare_size<test_struct_0_packed, std::tuple<int>>(); };
		"sizeof_1"_test = [] { compare_size<test_struct_1_packed, std::tuple<int, float>>(); };
		"sizeof_2"_test = [] { compare_size<test_struct_2_packed, std::tuple<int, float, char>>(); };
		"sizeof_3"_test = [] { compare_size<test_struct_3_packed, std::tuple<serek::str>>(); };
		"sizeof_4"_test = [] { compare_size<test_struct_4_packed, std::tuple<test_struct_3_packed>>(); };
		"sizeof_5"_test = [] { compare_size<test_struct_5_packed, std::tuple<int, float, int>>(); };
		"sizeof_6"_test = [] { compare_size<test_struct_6_packed, detail::es::test_struct_6_raw>(); };
		"sizeof_7"_test = [] { compare_size<test_struct_7_packed, std::tuple<int, int, int>>(); };
	};

	but::suite polimorphism = [] {
		using namespace detail;

		"virtual"_test = [] {
			but::expect(std::is_polymorphic_v<some_virtual_base_class_packed>);
			but::expect(not std::is_polymorphic_v<decltype(some_virtual_base_class_packed::field_0)>);
			but::expect(std::is_polymorphic_v<some_virtual_child_class_packed>);
			but::expect(not std::is_polymorphic_v<decltype(some_virtual_child_class_packed::field_0)>);
			but::expect(std::is_polymorphic_v<decltype(some_virtual_child_class_packed::field_1)>);

			some_virtual_base_class_packed vb1;
			but::expect(but::eq(vb1.do_something(), 0));

			some_virtual_child_class_packed vb2;
			but::expect(but::eq(vb2.do_something(), 1));

			using some_virtual_base_class = example_structs::polimorphic::some_virtual_base_class;
			std::unique_ptr<some_virtual_base_class> ptr_v{static_cast<some_virtual_base_class*>(new some_virtual_child_class_packed())};
			but::expect(but::eq(ptr_v->do_something(), 1));
		};

		"non_virtual"_test = [] {
			but::expect(not std::is_polymorphic_v<some_non_virtual_base_class_packed>);
			but::expect(std::is_polymorphic_v<decltype(some_non_virtual_base_class_packed::field_0)>);
			but::expect(not std::is_polymorphic_v<some_non_virtual_child_class_packed>);
			but::expect(std::is_polymorphic_v<decltype(some_non_virtual_child_class_packed::field_0)>);
			but::expect(not std::is_polymorphic_v<decltype(some_non_virtual_child_class_packed::field_1)>);

			some_non_virtual_base_class_packed nvb1;
			but::expect(but::eq(nvb1.do_something(), 0));

			some_non_virtual_child_class_packed nvb2;
			but::expect(but::eq(nvb2.do_something(), 1));

			using some_non_virtual_base_class = example_structs::polimorphic::some_non_virtual_base_class;
			std::unique_ptr<some_non_virtual_base_class> ptr_nv{reinterpret_cast<some_non_virtual_base_class*>(new some_non_virtual_child_class_packed())};
			but::expect(but::eq(ptr_nv->do_something(), 0));
		};
	};
}	 // namespace
