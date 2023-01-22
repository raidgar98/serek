/**
 * @file benchmark_controllers.cpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains implementations of serializations and deserializations for different libraries
 * @version 0.1
 * @date 2023-01-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <drogon_benchmark_controller.hpp>
#include <fc_benchmark_controller.hpp>
#include <serek_benchmark_controller.hpp>

scope_stoper::scope_stoper(benchmark_scores_storage_t::element_type::value_type& output) : m_output{output}, m_start{std::chrono::high_resolution_clock::now()} {}

scope_stoper::~scope_stoper() { m_output = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_start).count(); }

void drogon_benchmark_controller::deserialize(model_t& out, const std::string_view in) const
{

}

void drogon_benchmark_controller::serialize(const model_t& in, std::string& out) const
{

}

void fc_benchmark_controller::deserialize(model_t& out, const std::string_view in) const
{

}

void fc_benchmark_controller::serialize(const model_t& in, std::string& out) const
{

}

void serek_benchmark_controller::deserialize(model_t& out, const std::string_view in) const
{

}

void serek_benchmark_controller::serialize(const model_t& in, std::string& out) const
{

}

std::string drogon_benchmark_controller::controller_path() { return "/drogon"; }
std::string fc_benchmark_controller::controller_path() { return "/fc"; }
std::string serek_benchmark_controller::controller_path() { return "/serek"; }
