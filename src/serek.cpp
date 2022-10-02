/**
 * @file serek.cpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains implementations of few methods and functions
 * @version 0.1
 * @date 2021-08-30
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <sstream>

#include <serek/serek.hpp>

#ifdef BOOST_STACKTRACE_USE_BACKTRACE

	#include <iomanip>
	#include <boost/stacktrace.hpp>

namespace
{

	inline constexpr size_t STACKTRACE_OFFSET{4};

	/**
	 * @brief generates stacktrace. requires backtrace to work
	 *
	 * @param out result will be saved, into given shared pointer
	 *
	 * @warning std::make_shared<str> will be executed on given reference
	 * @note stacktrace format: #0000	at ${function_name} in file ${source_file}:${source_line_no}\n
	 */
	void save_stacktrace(std::shared_ptr<serek::str>& out)
	{
		std::stringstream stream{};
		const auto frames{boost::stacktrace::stacktrace().as_vector()};
		for(size_t i = STACKTRACE_OFFSET; i < frames.size(); ++i)
		{
			const auto& frame		  = frames[i];
			const int64_t frame_id = static_cast<int64_t>(i - STACKTRACE_OFFSET);
			stream << "\t#" << std::setw(4) << std::setfill('0') << frame_id;
			if(frame.empty()) stream << "\tunresolved\n";
			else
			{
				stream << "\tat '" << frame.name() << '\'';

				if(frame.source_line()) stream << " in file " << frame.source_file() << ':' << frame.source_line() << '\n';
				else	 // filename and line number unresolved
					stream << '\n';
			}
		}
		out = std::make_shared<serek::str>(stream.str());
	}
#else
void save_stacktrace(std::shared_ptr<serek::str>&) {}
#endif
}	 // namespace

namespace serek
{

	exceptions::exception_base::exception_base(str_v msg) : message{std::make_shared<str>(msg)} { save_stacktrace(this->stacktrace); }

	str exceptions::exception_base::pretty() const noexcept
	{
		std::stringstream ss;
		ss << "exception message: " << this->what() << "\n\n";
		if(this->stacktrace) ss << "stacktrace:\n\n" << *this->stacktrace << '\n';
		return ss.str();
	}

	const char* exceptions::exception_base::what() const noexcept { return this->message->c_str(); }

	str exceptions::comprasion_fail_exception::pretty() const noexcept
	{
		using namespace std::literals::string_literals;
		return (*this->comprasion_result) + "\n"s + exception_base::pretty();
	}

}	 // namespace serek

std::ostream& operator<<(std::ostream& os, const serek::exceptions::exception_base& ex) { return os << ex.pretty(); }
