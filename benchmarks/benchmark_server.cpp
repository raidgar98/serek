/**
 * @file benchmark_server.cpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains main() function for benchmark server
 * @version 0.1
 * @date 2023-01-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <fstream>

#include <drogon/HttpAppFramework.h>

#include <drogon_benchmark_controller.hpp>
#include <fc_benchmark_controller.hpp>
#include <serek_benchmark_controller.hpp>

using ll = trantor::Logger::LogLevel;
using score_set_t		= std::pair<benchmark_scores_storage_t, benchmark_scores_storage_t>;
using score_map_type = std::pair<std::string, score_set_t>;

/**
 * @brief Returns logger stream object
 *
 * @tparam log_level level on which log should be displayed
 * @param line_no line number where log happen (use __LINE__ macro)
 * @param function function name which emitted this log
 * @return trantor::LogStream& stream object, works same as std::cout
 */
template<ll log_level>
trantor::LogStream& log(const size_t line_no, const char* function = "main")
{
	static trantor::Logger log(__FILE__, line_no, log_level, function);
	return log.stream();
}

/**
 * @brief Create a pair object
 *
 * @return score_set_t initialized pair of two shared pointers pointing to initialized vectors
 */
score_set_t create_pair()
{
	return std::make_pair(
		std::make_shared<benchmark_scores_storage_t::element_type>(),
		std::make_shared<benchmark_scores_storage_t::element_type>()
	);
}

int main()
{
	std::map<score_map_type::first_type, score_map_type::second_type> scores;
	const auto& dg = scores.insert(score_map_type{"drogon_benchmark_controller", create_pair()});
	const auto& fc = scores.insert(score_map_type{"fc_benchmark_controller", create_pair()});
	const auto& sk = scores.insert(score_map_type{"serek_benchmark_controller", create_pair()});

	drogon::app()
		 .setLogLevel(ll::kTrace)
		 .registerController(std::make_shared<drogon_benchmark_controller>(dg.first->second.first, dg.first->second.second))
		 .registerController(std::make_shared<fc_benchmark_controller>(fc.first->second.first, fc.first->second.second))
		 .registerController(std::make_shared<serek_benchmark_controller>(sk.first->second.first, sk.first->second.second))
		 .setLogPath("./")
		 .addListener("0.0.0.0", 9999)
		 .setThreadNum(1)
		 .run();

	for(const auto& kv : scores)
	{
		// verify is amount of serializations and deserializations were same
		if(kv.second.first->size() != kv.second.second->size())
		{
			log<ll::kFatal>(__LINE__) << "Different amount of calls performed aborting!";
			return -1;
		}

		// warn if number of performed calls were lower than expected
		if(const size_t calls = kv.second.first->size(); calls < amount_of_calls)
			log<ll::kWarn>(__LINE__) << "Amount of performed calls is lower than expected number of calls: " << calls << " < " << amount_of_calls;

		// open file
		const std::string filename{ kv.first + "_scores.csv" };
		std::ofstream out_file{  filename };
		if(!out_file.good())
		{
			// handle errors
			log<ll::kError>(__LINE__) << "Failed to open: " << filename;
			out_file.close();
			continue;
		}

		// write header
		out_file << "probe no.,deserialization time in ms,serialization time in ms\n";

		//  write data
		for(size_t i{0ul}; i < kv.second.first->size(); ++i)
			out_file << i << ',' << kv.second.first->at(i) << ',' << kv.second.second->at(i) << '\n';

		// flush data that has been written
		out_file << std::flush;

		// close file
		out_file.close();
	}

	return 0;
}
