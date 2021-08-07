
#include <iostream>
#include <serek/serek.hpp>

#include <boost/type_index.hpp>

struct DUPA
{
	int dudud;
};

struct dupa : public serek::serial_helper_t
{
	serek::field<&dupa::_, int> x;
	serek::field<&dupa::x, DUPA> y;
};

int main()
{
	dupa x{{}, 2};

	using x_x_t = decltype(x.x);
	using x_y_t = decltype(x.y);

	std::cout << boost::typeindex::type_id<x_x_t>().pretty_name() << std::endl;
	std::cout << boost::typeindex::type_id<x_y_t>().pretty_name() << std::endl;


	return 0;
}