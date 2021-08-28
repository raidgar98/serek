#include <iostream>
#include <serek/serek.hpp>

#include <boost/type_index.hpp>

struct Sample
{
	int dudud;
};

struct sample_class
{
	serek::ffield<int> gg;
	serek::field<&sample_class::gg, Sample> y{};
	serek::field<&sample_class::y, float> z{2.67f};
};

struct my_vis : public serek::visitors::base_visitor_members
{
	using res_t = serek::visitor_result_t;

	template<serek::reqs::fundamental_wrapper_req T> res_t operator()(T* v)
	{
		std::cout << boost::typeindex::type_id<typename T::value_t>().pretty_name() << ": " << *v
					 << std::endl;
		return true;
	}

	template<typename T> res_t operator()(T*)
	{
		std::cout << boost::typeindex::type_id<T>().pretty_name() << std::endl;
		return true;
	}
};


template<int N> void foo() { foo<N - 1>(); }

template<> void foo<0>() { serek::require(nullptr); }

int main()
{
	sample_class x{.gg{2}, .z{21.37f}};

	try
	{
		foo<10>();
		// serek::require( nullptr );
	}
	catch(const serek::exceptions::exception_base& ex)
	{
		std::cout << ex.pretty() << std::endl;
	}

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
	serek::visitors::visit(&vv, &x.z);

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