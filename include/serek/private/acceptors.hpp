/**
 * @file acceptors.hpp
 * @author Krzysztof Mochocki (raidgar98@onet.pl)
 * @brief Contains declarations and definition of classes that handles visitors
 * @version 0.1
 * @date 2021-08-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <serek/private/requirements.hpp>

namespace serek
{
	/**
	 * @brief Contains declarations and definitions of visitor handlers
	 */
	namespace acceptors
	{

		/**
		 * @brief Predeclaration of acceptor_impl
		 * 
		 * @tparam acceptor_worker_t statically checks is given worker is valid
		 */
		template<reqs::acceptor_worker_req acceptor_worker_t>
		struct acceptor_impl;

		/**
		 * @brief Most common visitor handler, which just evaluates worker
		 * 
		 * @tparam acceptor_worker_t worker to call
		 * @tparam child_t type forwarded to worker; type from derivation tree of this class
		 */
		template<template<typename T> typename acceptor_worker_t, typename child_t>
		struct acceptor_impl<acceptor_worker_t<child_t>>
		{
			/**
			 * @brief performs actuall job (invokes worker)
			 * 
			 * @tparam visitor_t type of visitor to invoker
			 * @param visitor visitor to invoke
			 * @return visitor_result_t
			 * 
			 * @see visitor_req
			 */
			template<reqs::visitor_req visitor_t>
			visitor_result_t visit(visitor_t* visitor)
			{
				return acceptor_worker_t<child_t>{}(reinterpret_cast<child_t*>(const_cast<acceptor_impl*>(this)), visitor);
			}

			/**
			 * @copydoc acceptor_impl::visit
			 */
			template<reqs::visitor_req visitor_t>
			visitor_result_t visit(visitor_t* visitor) const
			{
				return acceptor_worker_t<child_t>{}(reinterpret_cast<const child_t*>(this), visitor);
			}
		};

		/**
		 * @brief Contains declarations and definitions of handling visitors
		 */
		namespace workers
		{
			namespace
			{
				/**
				 * @brief helper function, that invokes visitor
				 * 
				 * @param visitor visitor to invoke
				 * @param acceptor object to pass to visitor
				 * @return visitor_result_t 
				 */
				inline visitor_result_t call_visitator(auto* visitor, auto* acceptor) { return (*visitor)(acceptor); }
			}	 // namespace

			/**
			 * @brief base for further acceptor workers
			 * 
			 * @tparam child_t type that is currently handled
			 */
			template<typename child_t>
			struct acceptor_worker_base
			{
				using value_t = child_t;
			 protected:

				/**
				 * @brief helper method that validates 
				 * 
				 * @tparam visitor_t type of visitor
				 * @param visitor visitor to check
				 */
				template<reqs::visitor_req visitor_t>
				void validate_visitator(visitor_t* visitor) const
				{
					require(visitor, "visitor cannot be nullptr");
					require(visitor->that(), "`that` in visitor cannot be nullptr");
				}
			};

			/**
			 * @brief most common worker, that just invokes visitor
			 * 
			 * @tparam child_t currently processed type
			 */
			template<typename child_t>
			struct basic_acceptor_worker : public acceptor_worker_base<child_t>
			{
				/**
				 * @brief Construct a new basic acceptor worker object and actally performs action
				 * 
				 * @tparam visitor_t type of visitor
				 * @param acceptor object to handle via visitor
				 * @param visitor visitor to call
				 */
				template<reqs::visitor_req visitor_t>
				visitor_result_t operator()(child_t* acceptor, visitor_t* visitor) const
				{
					this->validate_visitator(visitor);
					return call_visitator(visitor, acceptor);
				}

				/**
				 * @copydoc basic_acceptor_worker::operator()
				 */
				template<reqs::visitor_req visitor_t>
				visitor_result_t operator()(const child_t* acceptor, visitor_t* visitor) const
				{
					this->validate_visitator(visitor);
					return call_visitator(visitor, acceptor);
				}
			};

			/**
			 * @brief alias for acceptor that is in first member in struct
			 * 
			 * @tparam child_t given type of first member
			 */
			template<typename child_t>
			using finalize_acceptor_worker = basic_acceptor_worker<child_t>;

			/**
			 * @brief predeclaration is used here to extract type further in specialisation
			 * 
			 * @tparam value static pointer to field
			 */
			template<auto value>
			struct forward_acceptor_creator;

			/**
			 * @brief type wrapper for `forward_acceptor_worker_impl`
			 * 
			 * @tparam class_t type of top level owner of field
			 * @tparam prev_field_t type of previous memeber
			 * @tparam value static pointer to previous member
			 */
			template<typename class_t, typename prev_field_t, prev_field_t class_t::*value>
			struct forward_acceptor_creator<value>
			{
				/**
				 * @brief actual implementation of acceptor worker, which forward visitor to previous member
				 * 
				 * @tparam child_t type of current field
				 */
				template<typename child_t>
				struct forward_acceptor_worker_impl : public acceptor_worker_base<child_t>
				{
					/**
					 * @brief Construct a new forward acceptor worker impl object and evaluates visitor
					 * 
					 * @tparam visitor_t type of visitor
					 * @param acceptor object to pass to visitor
					 * @param visitor visito to call
					 */
					template<reqs::visitor_req visitor_t>
					visitor_result_t operator()(child_t* acceptor, visitor_t* visitor) const
					{
						this->validate_visitator(visitor);
						visitor->last_result = visitors::visit(visitor, &(visitor->template that<class_t>()->*value));
						return call_visitator(visitor, acceptor);
					}

					/**
					 * @copydoc forward_acceptor_worker_impl::operator()
					 */
					template<reqs::visitor_req visitor_t>
					visitor_result_t operator()(const child_t* acceptor, visitor_t* visitor) const
					{
						this->validate_visitator(visitor);
						visitor->last_result = visitors::visit(visitor, &(visitor->template that<class_t>()->*value));
						return call_visitator(visitor, acceptor);
					}
				};
			};
		}	 // namespace workers
	}		 // namespace acceptors
}	 // namespace serek