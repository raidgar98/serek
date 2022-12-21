#include <shared/tools.hpp>
#include <shared/packed_struct_set.hpp>

#include <serek/json/tokenization.hpp>

namespace
{
	namespace but = boost::ut;
	using namespace but::literals;
	using namespace serek::tests;
	using namespace serek::deserial::json;

	struct test_json_walker : public json_walker
	{
		using json_walker::is_valid_json_string_end;
		using json_walker::json_walker;
		using json_walker::walk_over_array;
		using json_walker::walk_over_item;
		using json_walker::walk_over_null;
		using json_walker::walk_over_number;
		using json_walker::walk_over_object;
		using json_walker::walk_over_string;
	};

	template<auto function>
	auto check_length_template()
	{
		return [](const serek::str& str, const size_t expected_length, const size_t start = 0ul) { but::expect(but::eq((test_json_walker{str}.*function)(start), expected_length)); };
	}

	but::suite tools = [] {
		const auto comparison_fail_exception_check = [](auto func) { but::expect(but::throws<serek::exceptions::comparison_fail_exception>(func)); };
		const auto assert_exception_check			 = [](auto func) { but::expect(but::throws<serek::exceptions::assert_exception>(func)); };

		"ltrim_pos"_test = [&] {
			const auto check_valid_pos = [](const serek::str& str, const size_t expected_pos, const size_t start = 0) { but::expect(but::eq(expected_pos, ltrim_pos(str, start))); };

			check_valid_pos("a", 0);
			check_valid_pos(" a", 1);
			check_valid_pos(" \a\ta", 3);
			check_valid_pos(" \a\ta ", 3);
			check_valid_pos(" \a\ta \a\t", 3);

			// check all trimmed chars
			const serek::str trimmed_chars{get_trimmed_chars()};
			for(const char c: trimmed_chars)
			{
				const char tab[6] = {' ', c, c, 'a', ' ', c};
				check_valid_pos(serek::str{tab, 6}, 3);
			}

			check_valid_pos("\a\t\a\a\aa", 5, 2);
			check_valid_pos("aaa", 1, 1);
			check_valid_pos("\aa\aa\a", 3, 2);
		};

		"rtrim_pos"_test = [&] {
			const auto check_valid_pos
				 = [](const serek::str& str, const size_t expected_pos, const size_t start = serek::str::npos) { but::expect(but::eq(expected_pos, rtrim_pos(str, start))); };

			check_valid_pos("a", 0);
			check_valid_pos("a ", 0);
			check_valid_pos("a\a", 0);
			check_valid_pos("a\a\t", 0);
			check_valid_pos("a\a   \t", 0);
			check_valid_pos("a\a   \t", 0);

			check_valid_pos(" a\a   \t", 1);
			check_valid_pos("\a\a a\a   \t", 3);

			// check all trimmed chars
			const serek::str trimmed_chars{get_trimmed_chars()};
			for(const char c: trimmed_chars)
			{
				const char tab[6] = {' ', 'a', c, c, ' ', c};
				check_valid_pos(serek::str{tab, 6}, 1);
			}

			check_valid_pos("a\a\a\t", 0, 2);
			check_valid_pos("aaaaa", 1, 1);
			check_valid_pos("a\aa\aa", 0, 1);
		};

		"trim"_test = [&] {
			const auto verify_with_pattern = serek::tests::verify_with_pattern<serek::str_v>;

			verify_with_pattern(trim("abc"), "abc");
			verify_with_pattern(trim(" abc "), "abc");
			verify_with_pattern(trim("\tabc\a"), "abc");
			verify_with_pattern(trim("\aabc\a"), "abc");
			verify_with_pattern(trim("\aa bc\a"), "a bc");
			verify_with_pattern(trim("\aab c\a"), "ab c");
			verify_with_pattern(trim("\aa b c\a"), "a b c");
			verify_with_pattern(trim("                      abc      \t\t\t\t"), "abc");
			verify_with_pattern(trim("                  a    abc  c    \t\t\t\t"), "a    abc  c");
		};

		"is_valid_json_string_end"_test = [&] {
			const auto check_valid_end
				 = [](const serek::str& str, const size_t pos, const bool valid = true) { but::expect(but::eq(valid, test_json_walker{str}.is_valid_json_string_end(pos))); };

			check_valid_end("\"\"", 1);
			check_valid_end("\" \"", 2);

			check_valid_end("\"\\\"", 2, false);
		};

		"length_of_string_with_quotes"_test = [&] {
			const auto check_length = check_length_template<&test_json_walker::walk_over_string>();

			check_length("\"aaa\"", 5);
			check_length("\"a   aa\"", 8);
			check_length("\"a\t\t\aaa\"", 8);
			check_length("\"\taaa\"", 6);
			check_length("\"\\\"aaa\"", 7);
			check_length("\"aa\\\"a\"", 7);
			check_length("\"\t bla bla bla \\\"aaaa\\\"\n\t  bla bla bla  \a \"", 43);

			check_length(" \"aaa\"", 5);
			check_length("\t\"aaa\"", 5);
			check_length("\a\t\"aaa\"", 5);

			check_length("\"aaa\"a", 5);
			check_length("\"aaa\"   a", 5);

			check_length(" \"aaa\" ", 5);
			check_length(" \"aaa\" a", 5);

			check_length("a\"aaa\"", 0);
			check_length(" a\"aaa\"", 0);
			comparison_fail_exception_check([&] { check_length("\"aaa", 5); });
		};

		"length_of_number"_test = [&] {
			const auto check_length = check_length_template<&test_json_walker::walk_over_number>();

			check_length("aaa", 0);
			check_length(" aaa", 0, 1);

			std::vector<serek::str> parametrize{{
				 "7",
				 "2137",

				 "7.2",
				 "21.3",
				 "21.37",

				 "2e1",
				 "2e21",
				 "2e-1",
				 "2e-21",

				 "2.0e1",
				 "2.0e-1",
				 "2.0e21",
				 "2.0e-21",

				 "2.2222222e1",
				 "2.2222222e-1",
				 "2.2222222e21",
				 "2.2222222e-21",
			}};

			for(const auto& str: parametrize)
			{
				if(const size_t pos = str.find('e'); pos != serek::str::npos)
				{
					serek::str item{str};
					item[pos] = 'E';
					parametrize.emplace_back(item);
				}
			}

			for(const auto& str: parametrize)
			{
				const size_t length{str.size()};
				check_length(str, length);
				check_length('-' + str, length + 1);

				check_length(str + "       ", length);
				check_length("   " + str + "       ", length);

				comparison_fail_exception_check([&] { check_length(str + "ee", length + 2); });
				comparison_fail_exception_check([&] { check_length(str + "EE", length + 2); });
				comparison_fail_exception_check([&] { check_length(str + "..", length + 2); });

				comparison_fail_exception_check([&] { check_length(str + "---", length + 3); });
				comparison_fail_exception_check([&] { check_length("---" + str, length + 3); });
			}
		};

		"length_of_null"_test = [&] {
			const auto check_length = check_length_template<&test_json_walker::walk_over_null>();

			check_length("null", 4);
			check_length("null ", 4);

			check_length(" null", 4);
			check_length(" null ", 4);

			check_length("NULL", 0);
			check_length(" null", 4);
		};

		"length_of_object"_test = [&] {
			const auto check_length		 = check_length_template<&test_json_walker::walk_over_object>();
			const auto length_of_object = [&](const serek::str_v v, const size_t pos) { return test_json_walker{v}.walk_over_object(pos); };

			but::expect(but::nothrow([&] { length_of_object("}", 0); }));

			check_length("{}", 2);
			check_length("{} ", 2);
			check_length("{\"a\":1}", 7);
			check_length("{\"a\": 1}", 8);
			check_length("{\"a\":    \t\a\t1}", 14);
			check_length("{\"a\": \"aaa{}}}}{}{}}}\"}", 23);

			check_length("{\"a\": {\"a\":   []}}", 11, 6);
			check_length("\a\a\a\a{}\t\t\t\t\t", 2);
			check_length("{\"null\":null}", 13);
			check_length("{\"null\":\a\a\"'\"}", 14);


			assert_exception_check([&] { length_of_object("{", 0); });
			assert_exception_check([&] { length_of_object("{,}", 0); });
			assert_exception_check([&] { length_of_object("{:}", 0); });
			assert_exception_check([&] { length_of_object("{\"a\":,}", 0); });
			assert_exception_check([&] { length_of_object("{\"a\":}", 0); });
			assert_exception_check([&] { length_of_object("{\"a\"::}", 0); });
			assert_exception_check([&] { length_of_object("{\"a\",}", 0); });
			assert_exception_check([&] { length_of_object("{\"a\",:}", 0); });
			assert_exception_check([&] { length_of_object("{:\"a\"}", 0); });
			assert_exception_check([&] { length_of_object("{,\"a\"}", 0); });
			assert_exception_check([&] { length_of_object("{12}", 0); });
			assert_exception_check([&] { length_of_object("{12,}", 0); });
			assert_exception_check([&] { length_of_object("{12:}", 0); });
			assert_exception_check([&] { length_of_object("{null}", 0); });
			assert_exception_check([&] { length_of_object("{null:}", 0); });
			assert_exception_check([&] { length_of_object("{null,}", 0); });
		};

		"length_of_array"_test = [&] {
			const auto check_length		= check_length_template<&test_json_walker::walk_over_array>();
			const auto length_of_array = [&](const serek::str_v v, const size_t pos) { return test_json_walker{v}.walk_over_array(pos); };

			but::expect(but::nothrow([&] { length_of_array("]", 0); }));

			check_length("[]", 2);
			check_length("[] ", 2);
			check_length("[]      ", 2);
			check_length("     []      ", 2);
			check_length("[ ]", 3);
			check_length("[null]", 6);
			check_length("[{}]", 4);
			check_length("[1]", 3);
			check_length("[12]", 4);
			check_length("[1.2]", 5);
			check_length("[[],[],[]]", 10);
			check_length("[[],{},[]]", 10);
			check_length("[[],      {}]", 13);
			check_length("[[],   \a\a[]]", 12);

			assert_exception_check([&] { length_of_array("[", 0); });
			assert_exception_check([&] { length_of_array("[,]", 0); });
			assert_exception_check([&] { length_of_array("[,,,]", 0); });
			assert_exception_check([&] { length_of_array("[null null]", 0); });
		};
	};

	but::suite tokenization = [] {
		"empty"_test = [] {
			for(auto param : {
				"{}",
				"[]",
				"null",
				"\"aaaa\"",
				"7",
				"21.37"
			}) but::expect(but::nothrow([&]{tokenize_json(param);}));
		};

		"invalid_empty"_test = [] {
			for(auto param : {
				"",
				"  ",
			})
			but::expect(but::throws<serek::exceptions::assert_exception>([&]{tokenize_json(param);}));
		};

		"object"_test = [] {
			auto result = tokenize_json(R"({"a": 1, "b": "aaa", "c": null, "d": {}, "e": [], "f": 21.37})");

			but::expect(result->element_type == json_element_t::OBJECT_TYPE);
			but::expect(but::eq(result->object.size(), 6ul));

			for(auto kv : {std::pair<serek::str, std::pair<json_element_t, serek::str>>
				{"a", {json_element_t::FUNDAMENTAL_TYPE, "1"}},
				{"b", {json_element_t::FUNDAMENTAL_TYPE, "\"aaa\""}},
				{"c", {json_element_t::FUNDAMENTAL_TYPE, "null"}},
				{"d", {json_element_t::OBJECT_TYPE, ""}},
				{"e", {json_element_t::ARRAY_TYPE, ""}},
				{"f", {json_element_t::FUNDAMENTAL_TYPE, "21.37"}}
			})
			{
				auto it = result->object.find(kv.first);
				but::expect(result->object.end() != it);
				but::expect(it->second->element_type == kv.second.first);
				but::expect(it->second->item == kv.second.second);
				but::expect(but::eq(it->second->object.size(), 0ul));
				but::expect(but::eq(it->second->array.size(), 0ul));
			}
		};

		"array"_test = [] {
			const std::vector expected_type_values{std::pair<json_element_t, serek::str>
				{json_element_t::FUNDAMENTAL_TYPE, "1"},
				{json_element_t::FUNDAMENTAL_TYPE, "\"aaa\""},
				{json_element_t::FUNDAMENTAL_TYPE, "null"},
				{json_element_t::OBJECT_TYPE, ""},
				{json_element_t::ARRAY_TYPE, ""},
				{json_element_t::FUNDAMENTAL_TYPE, "21.37"}
			};

			auto result = tokenize_json(R"([1, "aaa", null, {}, [], 21.37])");

			but::expect(result->element_type == json_element_t::ARRAY_TYPE);
			but::expect(but::eq(result->array.size(), expected_type_values.size()));

			for(size_t i = 0; i < expected_type_values.size(); ++i)
			{
				const auto& it = result->array[i];
				const auto& kv = expected_type_values[i];

				but::expect(it->element_type == kv.first);
				but::expect(it->item == kv.second);
				but::expect(but::eq(it->object.size(), 0ul));
				but::expect(but::eq(it->array.size(), 0ul));
			}
		};
	};
}	 // namespace
