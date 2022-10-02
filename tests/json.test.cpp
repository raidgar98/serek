#include <shared/tools.hpp>
#include <shared/packed_struct_set.hpp>

#include <serek/serial/json.hpp>

namespace
{
	namespace but = boost::ut;
	using namespace but::literals;
	using namespace serek::tests;
	using namespace example_structs;
	using namespace example_structs::packed;

	template<typename T, typename... Args>
	void verify_pattern_for_type(const serek::str& pattern, Args&&... argv)
	{
		const serek::str value = serek::serial::json::serialize(T{std::forward<Args>(argv)...});
		verify_with_pattern(value, pattern);
	}

	but::suite patterns = [] {
		"flat_structs"_test = [] {
			verify_pattern_for_type<test_struct_0_packed>("{\"field_0\":0}");
			verify_pattern_for_type<test_struct_0_packed>("{\"field_0\":2137}", 2137);

			verify_pattern_for_type<test_struct_1_packed>("{\"field_0\":0,\"field_1\":0}");
			verify_pattern_for_type<test_struct_1_packed>("{\"field_0\":12,\"field_1\":3.14}", 12, 3.14f);

			verify_pattern_for_type<test_struct_2_packed>("{\"field_0\":0,\"field_1\":0,\"field_2\":\"\"}");
			verify_pattern_for_type<test_struct_2_packed>("{\"field_0\":4,\"field_1\":12.6,\"field_2\":\"\1\"}", 4, 12.6f, '\1');
			verify_pattern_for_type<test_struct_2_packed>("{\"field_0\":4,\"field_1\":12.6,\"field_2\":\"\\\\\"}", 4, 12.6f, '\\');
			verify_pattern_for_type<test_struct_2_packed>("{\"field_0\":4,\"field_1\":12.6,\"field_2\":\"\a\"}", 4, 12.6f, '\a');
			verify_pattern_for_type<test_struct_2_packed>("{\"field_0\":4,\"field_1\":12.6,\"field_2\":\"\a\"}", 4, 12.6f, '\a');

			verify_pattern_for_type<test_struct_3_packed>("{\"field_0\":\"\"}");
			verify_pattern_for_type<test_struct_3_packed>("{\"field_0\":\"bbb\"}", "bbb");

			verify_pattern_for_type<test_struct_5_packed>("{\"field_0\":0,\"field_2\":0}", 0, 0, 0.0f);
			verify_pattern_for_type<test_struct_5_packed>("{\"field_0\":1,\"field_2\":3}", 1, 2, 3.0f);

			verify_pattern_for_type<test_struct_7_packed>("{\"field_0\":0,\"field_2\":0}");
			verify_pattern_for_type<test_struct_7_packed>("{\"field_0\":1,\"field_2\":3}", 1, 2, 3);
		};

		"nested_structs"_test = [] {
			verify_pattern_for_type<test_struct_4_packed>("{\"field_0\":{\"field_0\":\"\"}}");
			verify_pattern_for_type<test_struct_4_packed>("{\"field_0\":{\"field_0\":\"aaa\"}}", "aaa");
		};
	};

}	 // namespace
