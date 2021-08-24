#pragma once

#include "requirements.hpp"

namespace serek
{
	namespace acceptors
	{
		template<reqs::acceptor_worker_req acceptor_worker_t> struct acceptor_impl
		{
			template<visitor_req visitor_t> visitor_result_t accept(visitor_t* visitor)
			{
				return acceptor_worker_t{visitor}.result;
			}

			template<visitor_req visitor_t> visitor_result_t accept(visitor_t* visitor) const
			{
				return acceptor_worker_t{visitor}.result;
			}
		};

		namespace workers
		{
			struct acceptor_worker_base
			{
				visitor_result_t result{};

			 protected:
				template<reqs::visitor_req visitor_t> void validate_visitator(visitor_t* visitor) const
				{
					if(visitor == nullptr) throw std::invalid_argument{"visitor cannot be nullptr"};
					if(visitor->that == nullptr)
						throw std::invalid_argument{"that in visitor cannot be nullptr"};
				}
			};

			struct basic_acceptor_worker : public acceptor_worker_base
			{
				template<reqs::visitor_req visitor_t> explicit basic_acceptor_worker(visitor_t* visitor)
				{
					validate_visitator(visitor);
					result = visitor->visit(this);
				}
			};

			template<typename class_t, auto value>
			struct forward_acceptor_worker : public acceptor_worker_base
			{
				template<reqs::visitor_req visitor_t> explicit forward_acceptor_worker(visitor_t* visitor)
				{
					validate_visitator(visitor);
					visitor->last_result
						 = visitors::visit(visitor, reinterpret_cast<class_t*>(visitor->that)->*value);
					result = visitor->visit(this);
				}
			};
		}	 // namespace workers
	}		 // namespace acceptors
}	 // namespace serek