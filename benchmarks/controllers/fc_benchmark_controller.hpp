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

struct fc_benchmark_controller : public base_benchmark_controller<fc_benchmark_controller>
{
	using base_benchmark_controller::base_benchmark_controller;

	virtual void deserialize(model_t& out, const std::string_view in) const override;
	virtual void serialize(const model_t& in, std::string& out) const override;
	static std::string controller_path();
};
