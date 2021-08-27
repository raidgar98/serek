#include <sstream>

#include <serek/serek.hpp>

#ifdef BOOST_STACKTRACE_USE_BACKTRACE

	#include <boost/stacktrace.hpp>

namespace
{
	void save_stacktrace(std::unique_ptr<std::string>& out)
	{
		const auto frames{boost::stacktrace::stacktrace().as_vector()};
		std::stringstream stream{};
		stream << boost::stacktrace::stacktrace();
		out = std::make_unique<std::string>(stream.str());
	}
}	 // namespace
#else
void save_stacktrace(std::string& out) {}
#endif

serek::exceptions::exception_base::exception_base(const char* msg) :
	 message{std::make_unique<std::string>(msg)}
{
	save_stacktrace(this->stacktrace);
}

std::string serek::exceptions::exception_base::pretty() const noexcept
{
	std::stringstream ss;
	ss << "exception message: " << this->what() << "\n"
		<< "\n";
	if(this->stacktrace) ss << "stacktrace:\n" << *this->stacktrace << '\n';

	return ss.str();
}

const char* serek::exceptions::exception_base::what() const noexcept
{
	return this->message->c_str();
}
