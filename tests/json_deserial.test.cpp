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

	};
}	 // namespace
