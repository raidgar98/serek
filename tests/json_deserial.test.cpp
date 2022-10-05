#include <shared/tools.hpp>
#include <shared/packed_struct_set.hpp>

#include <serek/json/deserial.hpp>

namespace
{
	namespace but = boost::ut;
	using namespace but::literals;
	using namespace serek::tests;
	using namespace serek::deserial::json;

	but::suite tools = [] {
		const auto comparison_fail_exception_check = [](auto func) { but::expect(but::throws<serek::exceptions::comparison_fail_exception>(func)); };

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
			const auto check_valid_end = [](const serek::str& str, const size_t pos, const bool valid = true) { but::expect(but::eq(valid, is_valid_json_string_end(str, pos))); };

			check_valid_end("\"\"", 1);
			check_valid_end("\" \"", 2);

			check_valid_end("\"\\\"", 2, false);
		};

		"length_of_string_with_quotes"_test = [&] {
			const auto check_length = [](const serek::str& str, const size_t expected_length, const size_t start = 0ul) {
				but::expect(but::eq(length_of_string_with_quotes(str, start), expected_length));
			};

			check_length("\"aaa\"", 5);
			check_length("\"a   aa\"", 8);
			check_length("\"a\t\t\aaa\"", 8);
			check_length("\"\taaa\"", 6);
			check_length("\"\\\"aaa\"", 7);
			check_length("\"aa\\\"a\"", 7);
			check_length("\"\t bla bla bla \\\"aaaa\\\"\n\t  bla bla bla  \a \"", 43);

			check_length(" \"aaa\"", 5, 1);
			check_length("\t\"aaa\"", 5, 1);
			check_length("\a\t\"aaa\"", 5, 2);

			check_length("\"aaa\"a", 5);
			check_length("\"aaa\"   a", 5);

			check_length(" \"aaa\" ", 5, 1);
			check_length(" \"aaa\" a", 5, 1);

			check_length("a\"aaa\"", 0);
			check_length(" a\"aaa\"", 0, 1);
			comparison_fail_exception_check([&] { check_length("\"aaa", 5); });
		};

		"length_of_number"_test = [&] {
			const auto check_length
				 = [](const serek::str& str, const size_t expected_length, const size_t start = 0ul) { but::expect(but::eq(length_of_number(str, start), expected_length)); };

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
				check_length("   " + str + "       ", length, 3);

				comparison_fail_exception_check([&] { check_length(str + "ee", length + 2); });
				comparison_fail_exception_check([&] { check_length(str + "EE", length + 2); });
				comparison_fail_exception_check([&] { check_length(str + "..", length + 2); });

				comparison_fail_exception_check([&] { check_length(str + "---", length + 3); });
				comparison_fail_exception_check([&] { check_length("---" + str, length + 3); });
			}
		};

		"length_of_null"_test = [&] {
			const auto check_length
				 = [](const serek::str& str, const size_t expected_length, const size_t start = 0ul) { but::expect(but::eq(length_of_null(str, start), expected_length)); };

			check_length("null", 4);
			check_length("null ", 4);

			check_length(" null", 4, 1);
			check_length(" null ", 4, 1);

			check_length("NULL", 0);
			check_length(" null", 0);
		};

		"item_length"_test = [&] {

		};
	};
}	 // namespace
