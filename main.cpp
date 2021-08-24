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

struct my_vis : public serek::visitors::base_visitor_members
{
	using res_t = serek::visitor_result_t;

	template<serek::reqs::fundamental_wrapper_req T>
	res_t visit(T* v)
	{
		std::cout << boost::typeindex::type_id<typename T::value_t>().pretty_name() << ": " << *v << std::endl;
		return true;
	}

	template<typename T>
	res_t visit(T*)
	{
		std::cout << boost::typeindex::type_id<T>().pretty_name() << std::endl;
		return true;
	}
};


int main()
{
	dupa x{ .gg{2}, .z{21.37f}};

	// using x_x_t = decltype(x.gg);
	// using x_y_t = decltype(x.y);

	// std::cout << boost::typeindex::type_id<x_x_t>().pretty_name() << std::endl;
	// std::cout << boost::typeindex::type_id<x_y_t>().pretty_name() << std::endl;
	// std::cout << boost::typeindex::type_id<decltype(x.z)>().pretty_name() << std::endl;

	// std::cout << x.gg + 20 << std::endl;
	// std::cout << (x.gg < 1) << std::endl;
	// std::cout << (20 - x.gg) << std::endl;
	// std::cout << x.gg << std::endl;
	
	// std::cout << x.z << std::endl;

	my_vis vv{&x};
	// vv.template visit<int>(nullptr);
	serek::visitors::visit( &vv, &x.z);

	return 0;
}
/*
struct container
{
	template<typename x>
	struct my_vector_impl
	{

	};
	
	template<typename x>
	using my_vector = typename container::my_vector_impl<x>;
};

template<template<typename T> typename any_container>
struct my_struct
{
	any_container<int> x;
};

int main()
{
	my_struct< container::my_vector > x; 
}
*/