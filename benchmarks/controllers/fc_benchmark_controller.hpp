/**
 * @file fc_benchmark_controller.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains declaration of benchmark controller for fc library
 * @version 0.1
 * @date 2023-01-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <base_benchmark_controller.hpp>
#include <fc/reflect/reflect.hpp>

namespace fc_model_t
{
	struct A
	{
		bool a1{};
		int32_t a2{};
		uint32_t a3{};
		float a4{};
		double a5{};
	};

	struct B
	{
		std::vector<int> b1{};
		std::vector<double> b2{};
		std::vector<A> b3{};
	};

	struct C : public A
	{
		A c1{};
		B c2{};
	};
}

struct fc_benchmark_controller : public base_benchmark_controller<fc_benchmark_controller, typename fc_model_t::C>
{
	using base_benchmark_controller::base_benchmark_controller;

	virtual void deserialize(model_t& out, const std::string_view in) const override;
	virtual void serialize(const model_t& in, std::string& out) const override;
	static std::string controller_path();
};

FC_REFLECT(fc_model_t::A, (a1)(a2)(a3)(a4)(a5));
FC_REFLECT(fc_model_t::B, (b1)(b2)(b3));
FC_REFLECT_DERIVED(fc_model_t::C, (fc_model_t::A), (c1)(c2));
