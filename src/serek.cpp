#include <sstream>

#include <serek/serek.hpp>

#ifdef BOOST_STACKTRACE_USE_BACKTRACE

	#include <iomanip>
	#include <boost/stacktrace.hpp>

namespace
{

	inline constexpr size_t STACKTRACE_OFFSET{4};

	void save_stacktrace(std::shared_ptr<serek::str>& out)
	{
		std::stringstream stream{};
		const auto& frames{boost::stacktrace::stacktrace().as_vector()};
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
}	 // namespace
#else
void save_stacktrace(str& out) {}
#endif

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
}	 // namespace serek

std::ostream& operator<<(std::ostream& os, const serek::exceptions::exception_base& ex) { return os << ex.pretty(); }