#pragma once

#include "requirements.hpp"

namespace serek
{
	namespace acceptors
	{

		template<reqs::acceptor_worker_req acceptor_worker_t> struct acceptor_impl;

		template<template<typename T> typename acceptor_worker_t, typename child_t>
		struct acceptor_impl<acceptor_worker_t<child_t>>
		{
			template<reqs::visitor_req visitor_t> visitor_result_t visit(visitor_t* visitor)
			{
				return acceptor_worker_t<child_t>{
					 reinterpret_cast<child_t*>(const_cast<acceptor_impl*>(this)),
					 visitor}.result;
			}
		};

		namespace workers
		{
			template<typename child_t> struct acceptor_worker_base
			{
				visitor_result_t result{};

			 protected:
				template<reqs::visitor_req visitor_t> void validate_visitator(visitor_t* visitor) const
				{
					if(nullptr == visitor) throw std::invalid_argument{"visitor cannot be nullptr"};
					assert(nullptr == visitor->that);
					// throw std::invalid_argument{"that in visitor cannot be nullptr"};
				}
			};

			template<typename child_t>
			struct basic_acceptor_worker : public acceptor_worker_base<child_t>
			{
				template<reqs::visitor_req visitor_t>
				basic_acceptor_worker(child_t* acceptor, visitor_t* visitor)
				{
					this->validate_visitator(visitor);
					this->result = visitor->template operator()<child_t>(acceptor);
				}
			};

			template<typename child_t>
			struct finalize_acceptor_worker : public acceptor_worker_base<child_t>
			{
				template<reqs::visitor_req visitor_t>
				finalize_acceptor_worker(child_t* acceptor, visitor_t* visitor)
				{
					this->validate_visitator(visitor);
					visitor->template operator()<child_t>(acceptor);
					this->result = false;
				}
			};

			template<auto value> struct forward_acceptor_creator;

			template<typename class_t, typename next_field_t, next_field_t class_t::*value>
			struct forward_acceptor_creator<value>
			{
				template<typename child_t>
				struct forward_acceptor_worker_impl : public acceptor_worker_base<child_t>
				{
					template<reqs::visitor_req visitor_t>
					forward_acceptor_worker_impl(child_t* acceptor, visitor_t* visitor)
					{
						this->validate_visitator(visitor);
						visitor->last_result = visitors::template visit<visitor_t, next_field_t>(
							 visitor,
							 &(reinterpret_cast<class_t*>(visitor->that)->*value));
						this->result = visitor->template operator()<child_t>(acceptor);
					}
				};
			};
		}	 // namespace workers
	}		 // namespace acceptors
}	 // namespace serek