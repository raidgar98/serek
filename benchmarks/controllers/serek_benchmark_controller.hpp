/**
 * @file serek_benchmark_controller.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains declaration of benchmark controller for serek library
 * @version 0.1
 * @date 2023-01-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <base_benchmark_controller.hpp>

#include <serek/serek.hpp>
namespace serek_model_t
{
	struct A_impl
	{
		serek::ffield<bool> a1{};
		serek::field<&A_impl::a1, int32_t> a2{};
		serek::field<&A_impl::a2, uint32_t> a3{};
		serek::field<&A_impl::a3, float> a4{};
		serek::field<&A_impl::a4, double> a5{};
	};
	using A = serek::pack<&A_impl::a5>;

	struct B_impl
	{
		serek::ffield<std::vector<int>> b1{};
		serek::field<&B_impl::b1, std::vector<double>> b2{};
		serek::field<&B_impl::b2, std::vector<A>> b3{};
	};
	using B = serek::pack<&B_impl::b3>;

	struct C_impl : public A_impl
	{
		serek::field<&C_impl::a5, A> c1{};
		serek::field<&C_impl::c1, B> c2{};
	};
	using C = serek::pack<&C_impl::c2>;
}	 // namespace serek_model_t

struct serek_benchmark_controller : public base_benchmark_controller<serek_benchmark_controller, typename serek_model_t::C>
{
	using base_benchmark_controller::base_benchmark_controller;

	virtual void deserialize(model_t& out, const std::string_view in) const override;
	virtual void serialize(const model_t& in, std::string& out) const override;
	static std::string controller_path();
};
