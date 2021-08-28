#include <shared/tools.hpp>

namespace
{
	namespace but = boost::ut;
	using namespace but::literals;

	but::suite proper_order = [] { "field_base"_test = [] { serek::require(true); }; };
}	 // namespace