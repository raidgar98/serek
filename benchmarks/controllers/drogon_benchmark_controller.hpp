/**
 * @file drogon_benchmark_controller.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains declaration of benchmark controller for drogon library
 * @version 0.1
 * @date 2023-01-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <base_benchmark_controller.hpp>

namespace drogon_model
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

struct drogon_benchmark_controller : public base_benchmark_controller<drogon_benchmark_controller, typename drogon_model::C>
{
	using base_benchmark_controller::base_benchmark_controller;

	virtual void deserialize(model_t& out, const std::string_view in) const override;
	virtual void serialize(const model_t& in, std::string& out) const override;
	static std::string controller_path();
};
