/**
 * @file base_benchmark_controller.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains template base class for controller for benchmarking serialization
 * @version 0.1
 * @date 2023-01-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <drogon/HttpSimpleController.h>
#include <boost/type_index.hpp>

using drogon_callback_t				= std::function<void(const drogon::HttpResponsePtr&)>;
using benchmark_scores_storage_t = std::shared_ptr<std::vector<size_t>>;

constexpr size_t amount_of_calls{10'000};

/** @brief Measure time from object creation till object destruction */
struct scope_stoper
{
	/**
	 * @brief Construct a new scope stoper object
	 *
	 * @param output
	 */
	scope_stoper(benchmark_scores_storage_t::element_type::value_type& output);

	/** @brief Destroy the scope stoper object and writes measured time to m_output */
	~scope_stoper();

 private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
	benchmark_scores_storage_t::element_type::value_type& m_output;
};

/**
 * @brief Implements whole functionality of benchmarking,
 				leaving child classes classes only serialization and deserialization to implement
 *
 * @tparam T child class (CRTP)
 * @tparam t_model_t type to serialize/deserialize
 */
template<typename T, typename t_model_t>
struct base_benchmark_controller : public drogon::HttpSimpleController<base_benchmark_controller<T, t_model_t>, false>
{

	/** @brief Aliased for handy usage */
	using model_t = t_model_t;

	/**
	 * @brief Construct a new base benchmark controller object
	 *
	 * @param deserial_scores shared pointer to vector with time scores of deserialization
	 * @param serial_scores shared pointer to vector with time scores of serialization
	 */
	base_benchmark_controller(benchmark_scores_storage_t deserial_scores, benchmark_scores_storage_t serial_scores) :
		 m_scores_serialization{serial_scores}, m_scores_deserialization{deserial_scores}
	{
		// set atomic counter to 0ul
		m_request_no.store(0ul);

		// initialize one (reserve and fill with 0) and then force deep copy on second one (memcpy of 80'000 bytes is faster than filling 10'000 object)
		m_scores_serialization->reserve(amount_of_calls);
		m_scores_serialization->insert(m_scores_serialization->end(), amount_of_calls, 0ul);
		*m_scores_deserialization = *m_scores_serialization;

		// check is pointer has not been changed
		assert(m_scores_serialization.get() == serial_scores.get() && m_scores_deserialization.get() == deserial_scores.get());

		// check are both collections has same size, and memory regions are different (deep copy)
		assert(m_scores_deserialization->size() == m_scores_serialization->size() && m_scores_deserialization->data() != m_scores_serialization->data());
	}

	virtual void deserialize(model_t& out, const std::string_view in) const {}
	virtual void serialize(const model_t& in, std::string& out) const {}

	/**
	* @brief This function is called by drogon library when got connection
	*
	* @param req data from request
	* @param callback this callback sends response to sender
	*/
	virtual void asyncHandleHttpRequest(const drogon::HttpRequestPtr& req, drogon_callback_t&& callback) override
	{
		// increment and save request no. for this request
		const size_t current_request_id{m_request_no++};

		// if amount of request exceeds cancel further benchmarking
		if(current_request_id >= amount_of_calls)
		{
			auto err = drogon::HttpResponse::newHttpResponse();
			err->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
			err->setBody("Reached request limit!");

			callback(err);
			return;
		}

		// create model where deserialized output is stored and benchmark
		model_t model;
		{
			scope_stoper _{m_scores_deserialization->at(current_request_id)};
			deserialize(model, req->body());
		}

		// create string with huge pre-reserved space and benchmark
		std::string output{};
		output.resize(10'000ul);
		{
			scope_stoper _{m_scores_serialization->at(current_request_id)};
			serialize(model, output);
		}

		// send response which contains re-serialized incoming data
		auto response = drogon::HttpResponse::newHttpResponse();
		response->setStatusCode(drogon::HttpStatusCode::k200OK);
		response->setContentTypeCode(drogon::ContentType::CT_APPLICATION_JSON);
		response->setBody(std::move(output));

		callback(response);
	}

	/** @brief This function prepares routing */
	static void initPathRouting() { T::registerSelf__(T::controller_path() + "/bench", {drogon::Post}); }

	benchmark_scores_storage_t m_scores_serialization{};
	benchmark_scores_storage_t m_scores_deserialization{};

 private:
	std::atomic_uint64_t m_request_no{};
};
