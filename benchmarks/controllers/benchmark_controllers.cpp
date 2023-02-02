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
	const Json::Value null = Json::Value::null;
	const auto is_not_null = [&](const Json::Value& val) { serek::require<std::not_equal_to>(val, null); };

	Json::Value parsing_output;
	Json::Reader reader;

	if(!reader.parse(in.data(), parsing_output))
	{
		const auto err = reader.getFormattedErrorMessages();
		std::cout << err << std::endl;
		serek::require(false);
	}

	const auto deserial_A = [&is_not_null, &null](const Json::Value& json, drogon_model::A& o)
	{
		auto x = json.get("a1", null);
		is_not_null(x);
		o.a1 = x.asBool();

		x = json.get("a2", null);
		is_not_null(x);
		o.a2 = x.asInt();

		x = json.get("a3", null);
		is_not_null(x);
		o.a3 = x.asUInt();

		x = json.get("a4", null);
		is_not_null(x);
		o.a4 = x.asFloat();

		x = json.get("a5", null);
		is_not_null(x);
		o.a5 = x.asDouble();
	};

	const auto deserial_B = [&is_not_null, &null, &deserial_A](const Json::Value& json, drogon_model::B& o)
	{
		auto x = json.get("b1", null);
		is_not_null(x);
		o.b1.reserve(x.size());
		for(const auto& it : x)
			o.b1.emplace_back(it.asInt());

		x = json.get("b2", null);
		is_not_null(x);
		o.b2.reserve(x.size());
		for(const auto& it : x)
			o.b2.emplace_back(it.asDouble());

		x = json.get("b3", null);
		is_not_null(x);
		o.b3.reserve(x.size());
		for(const auto& it : x)
		{
			auto& a = o.b3.emplace_back();
			deserial_A(it, a);
		}
	};

	const auto c1 = parsing_output.get("c1", null);
	is_not_null(c1);

	const auto c2 = parsing_output.get("c2", null);
	is_not_null(c2);

	deserial_A(parsing_output, out);

	deserial_A(parsing_output, out.c1);
	deserial_B(parsing_output, out.c2);
}

void drogon_benchmark_controller::serialize(const model_t& in, std::string& out) const
{
	Json::Value output;

	const auto serial_A = [](Json::Value& json, const drogon_model::A& in)
	{
		json["a1"] = in.a1;
		json["a2"] = in.a2;
		json["a3"] = in.a3;
		json["a4"] = in.a4;
		json["a5"] = in.a5;
	};

	const auto serial_B = [&serial_A](Json::Value& json, const drogon_model::B& in)
	{
		json["b1"] = Json::Value(Json::ValueType::arrayValue);
		json["b1"].resize(in.b1.size());
		for(const auto& it : in.b1)
			json["b1"].append(it);

		json["b2"] = Json::Value(Json::ValueType::arrayValue);
		json["b2"].resize(in.b2.size());
		for(const auto& it : in.b2)
			json["b2"].append(it);

		json["b3"] = Json::Value(Json::ValueType::arrayValue);
		json["b3"].resize(in.b3.size());
		for(const auto& it : in.b3)
		{
			Json::Value it_result;
			serial_A(it_result, it);
			json["b3"].append(it_result);
		}
	};

	Json::Value result{Json::ValueType::objectValue};
	serial_A(result, in);

	Json::Value c1{Json::ValueType::objectValue};
	serial_A(c1, in.c1);

	Json::Value c2{Json::ValueType::objectValue};
	serial_B(c2, in.c2);

	result["c1"] = c1;
	result["c2"] = c2;

	Json::FastWriter writer{};
	out = writer.write(result);
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
