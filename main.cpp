
#include <iostream>
#include <serek/serek.hpp>

#include <boost/type_index.hpp>

struct print_visitor
{

};

struct DUPA
{
	int dudud;
};

struct dupa 
{
	serek::ffield<int> gg;
	serek::field<&dupa::gg, DUPA> y{};
	serek::field<&dupa::y, float> z{2.67f};
};

int main()
{
	dupa x{ .gg{2} };
	std::cout << x.gg << std::endl;

	using x_x_t = decltype(x.gg);
	using x_y_t = decltype(x.y);

	std::cout << boost::typeindex::type_id<x_x_t>().pretty_name() << std::endl;
	std::cout << boost::typeindex::type_id<x_y_t>().pretty_name() << std::endl;
	std::cout << boost::typeindex::type_id<decltype(x.z)>().pretty_name() << std::endl;

	std::cout << x.gg + 20 << std::endl;
	std::cout << (x.gg < 1) << std::endl;
	std::cout << (20 - x.gg) << std::endl;
	std::cout << x.gg << std::endl;
	
	std::cout << x.z << std::endl;


	return 0;
}