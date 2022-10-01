#include <shared/tools.hpp>
#include <shared/struct_set.hpp>

namespace
{
	namespace but = boost::ut;
	using namespace but::literals;
	using namespace serek::tests;
	using namespace example_structs;

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
		"order_4"_test = [] { validate_order<&test_struct_4<>::field_0>{"aaa", "aaa" /* implicit conversion to test_struct_3 */}; };
		"order_5"_test = [] { validate_order<&test_struct_5::field_2>{"13", 1, 2, 3.0f}; };
		"order_6"_test = [] { validate_order<&test_struct_6::field_4>{"01234"}; };
		"order_7"_test = [] { validate_order<&test_struct_7::field_2>{"02", 0, 1, 2}; };
	};


	but::suite proper_size = [] {
		"sizeof_0"_test = [] { compare_size<test_struct_0, std::tuple<int>>(); };
		"sizeof_1"_test = [] { compare_size<test_struct_1, std::tuple<int, float>>(); };
		"sizeof_2"_test = [] { compare_size<test_struct_2, std::tuple<int, float, char>>(); };
		"sizeof_3"_test = [] { compare_size<test_struct_3, std::tuple<serek::str>>(); };
		"sizeof_4"_test = [] { compare_size<test_struct_4<>, std::tuple<test_struct_3>>(); };
		"sizeof_5"_test = [] { compare_size<test_struct_5, std::tuple<int, float, int>>(); };
		"sizeof_6"_test = [] { compare_size<test_struct_6, test_struct_6_raw>(); };
		"sizeof_7"_test = [] { compare_size<test_struct_7, std::tuple<int, int, int>>(); };
	};

	but::suite polimorphism_fluctuation = [] {
		using namespace polimorphic;

		"is_virtual_first"_test = [] {
			but::expect(std::is_polymorphic_v<some_virtual_base_class>);
			but::expect(not std::is_polymorphic_v<decltype(some_virtual_base_class::field_0)>);
			but::expect(std::is_polymorphic_v<some_virtual_child_class>);
			but::expect(not std::is_polymorphic_v<decltype(some_virtual_child_class::field_0)>);
			but::expect(std::is_polymorphic_v<decltype(some_virtual_child_class::field_1)>);

			some_virtual_base_class vb1;
			but::expect(but::eq(vb1.do_something(), 0));

			some_virtual_child_class vb2;
			but::expect(but::eq(vb2.do_something(), 1));

			std::unique_ptr<some_virtual_base_class> ptr_v{static_cast<some_virtual_base_class*>(new some_virtual_child_class())};
			but::expect(but::eq(ptr_v->do_something(), 1));
		};

		"is_virtual_second"_test = [] {
			but::expect(not std::is_polymorphic_v<some_non_virtual_base_class>);
			but::expect(std::is_polymorphic_v<decltype(some_non_virtual_base_class::field_0)>);
			but::expect(not std::is_polymorphic_v<some_non_virtual_child_class>);
			but::expect(std::is_polymorphic_v<decltype(some_non_virtual_child_class::field_0)>);
			but::expect(not std::is_polymorphic_v<decltype(some_non_virtual_child_class::field_1)>);

			some_non_virtual_base_class nvb1;
			but::expect(but::eq(nvb1.do_something(), 0));

			some_non_virtual_child_class nvb2;
			but::expect(but::eq(nvb2.do_something(), 1));

			std::unique_ptr<some_non_virtual_base_class> ptr_nv{reinterpret_cast<some_non_virtual_base_class*>(new some_non_virtual_child_class())};
			but::expect(but::eq(ptr_nv->do_something(), 0));
		};
	};

	but::suite proper_cv = [] {
		// theese lambdas are to fool -Wunused-varriabble-warning
		const auto c_dummy  = [](const auto&) { return; };
		const auto v_dummy  = [](volatile auto&) { return; };
		const auto cv_dummy = [](const volatile auto&) { return; };

		"const_initialization"_test = [&] {
			{
				const test_struct_0 s{};
				c_dummy(s);
			}
			{
				const test_struct_1 s{};
				c_dummy(s);
			}
			{
				const test_struct_2 s{};
				c_dummy(s);
			}
			{
				const test_struct_3 s{};
				c_dummy(s);
			}
			{
				const test_struct_4 s{};
				c_dummy(s);
			}
			{
				const test_struct_5 s{};
				c_dummy(s);
			}
			{
				const test_struct_6 s{};
				c_dummy(s);
			}
			{
				const test_struct_7 s{};
				c_dummy(s);
			}
		};

		"volatile_initialization"_test = [&] {
			{
				volatile test_struct_0 s{};
				v_dummy(s);
			}
			{
				volatile test_struct_1 s{};
				v_dummy(s);
			}
			{
				volatile test_struct_2 s{};
				v_dummy(s);
			}
			{
				volatile test_struct_3 s{};
				v_dummy(s);
			}
			{
				volatile test_struct_4 s{};
				v_dummy(s);
			}
			{
				volatile test_struct_5 s{};
				v_dummy(s);
			}
			{
				volatile test_struct_6 s{};
				v_dummy(s);
			}
			{
				volatile test_struct_7 s{};
				v_dummy(s);
			}
		};

		"const_volatile_initialization"_test = [&] {
			{
				const volatile test_struct_0 s{};
				cv_dummy(s);
			}
			{
				const volatile test_struct_1 s{};
				cv_dummy(s);
			}
			{
				const volatile test_struct_2 s{};
				cv_dummy(s);
			}
			{
				const volatile test_struct_3 s{};
				cv_dummy(s);
			}
			{
				const volatile test_struct_4 s{};
				cv_dummy(s);
			}
			{
				const volatile test_struct_5 s{};
				cv_dummy(s);
			}
			{
				const volatile test_struct_6 s{};
				cv_dummy(s);
			}
			{
				const volatile test_struct_7 s{};
				cv_dummy(s);
			}
		};
	};
}	 // namespace
