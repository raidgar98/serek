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

