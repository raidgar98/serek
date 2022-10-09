#pragma once

#include <serek/private/field.hpp>

namespace serek
{
	namespace member_name_extractors
	{
		namespace impl
		{
			struct __terminator
			{
			};

			template<typename T>
			struct pack_extractor;

			template<typename T>
			struct field_extractor;

			template<auto X>
			struct auto_holder
			{
			};

			template<typename T, typename... Argv>
			struct concatenator;

			template<typename in_class_t, typename in_member_t, in_member_t in_class_t::*V>
			struct pack_extractor<serek::detail::pack_impl<V>>
			{
				using member_t = in_member_t;
				using packed_t = auto_holder<V>;
			};

			template<typename in_class_t, typename in_member_t, in_member_t in_class_t::*V, typename T>
			struct field_extractor<serek::detail::field_impl<V, T>>
			{
				using member_t = in_member_t;
				using next_t	= field_extractor<member_t>;
				using packed_t = auto_holder<V>;
			};

			template<auto V>
			struct concatenator<serek::detail::pack_impl<V>>
			{
				using helper_t = typename impl::pack_extractor<serek::detail::pack_impl<V>>;
				using result	= typename concatenator<field_extractor<typename helper_t::member_t>, typename helper_t::packed_t>::result;
			};

			template<auto V>
			struct concatenator<const serek::detail::pack_impl<V>>
			{
				using helper_t = typename impl::pack_extractor<serek::detail::pack_impl<V>>;
				using result	= typename concatenator<field_extractor<typename helper_t::member_t>, typename helper_t::packed_t>::result;
			};

			template<auto V>
			struct concatenator<serek::detail::type_holder<serek::detail::pack_impl<V>>> : public concatenator<serek::detail::pack_impl<V>>
			{
				using result = typename concatenator<serek::detail::pack_impl<V>>::result;
			};

			template<auto V>
			struct concatenator<const serek::detail::type_holder<serek::detail::pack_impl<V>>> : public concatenator<const serek::detail::pack_impl<V>>
			{
				using result = typename concatenator<const serek::detail::pack_impl<V>>::result;
			};

			template<typename T, typename... Argv>
			struct concatenator<field_extractor<T>, Argv...>
			{
				using helper_t = field_extractor<T>;
				using result	= typename concatenator<typename helper_t::next_t, typename helper_t::packed_t, Argv...>::result;
			};

			template<typename T, typename... Argv>
			struct concatenator<field_extractor<serek::ffield<T>>, Argv...>
			{
				using result = typename std::tuple<Argv..., __terminator>;
			};

			using tuple_name_extractor_appender_t = std::function<void(const serek::str&)>;

			template<typename tuple_t>
			struct tuple_name_collector;

			template<>
			struct tuple_name_collector<std::tuple<__terminator>>
			{
				tuple_name_collector(const tuple_name_extractor_appender_t) {}
			};

			template<typename type_1, typename... Args>
			struct tuple_name_collector<std::tuple<type_1, Args...>>
			{
				tuple_name_collector(const tuple_name_extractor_appender_t appender)
				{
					appender(serek::type_name<type_1>());
					tuple_name_collector<std::tuple<Args...>>{appender};
				}
			};

			template<typename packed_t>
			using members_as_types_result_t = concatenator<packed_t>::result;
		}	 // namespace impl

		template<typename packed_t>
		using members_as_types = typename impl::members_as_types_result_t<packed_t>;

		template<typename tuple_t>
		using tuple_name_collector = typename impl::tuple_name_collector<tuple_t>;

		namespace detail
		{
			template<typename storage_t>
			using storage_appender_t = std::function<void(const serek::str&)>;
		}

		/**
		 * @brief for given serek::pack returns set of members names
		 *
		 * @tparam packed_t serek::pack type to extract member names from
		 * @tparam storage_t type of storage to use
		 * @param appender functor that appends and returns that reference to inserted value
		 * @return storage_t<serek::str>
		 */
		template<typename packed_t>
		constexpr void extract_class_members(const detail::storage_appender_t<serek::str> appender)
		{
			tuple_name_collector<members_as_types<packed_t>>{[&](const serek::str& v) {
				const size_t namespace_pos = v.find_last_of(':') + 1ul;
				const size_t last_shevron	= v.find_last_of('>');
				appender(v.substr(namespace_pos, last_shevron - namespace_pos));
			}};
		}
	}	 // namespace member_name_extractors

}	 // namespace serek
