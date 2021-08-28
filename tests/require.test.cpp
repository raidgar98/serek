#include <shared/tools.hpp>

namespace
{
	namespace but = boost::ut;
	using namespace but::literals;

	but::suite no_fail = [] { "true_given"_test = [] { serek::require(true); }; };
}	 // namespace