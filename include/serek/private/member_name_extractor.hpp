#pragma once

#include <serek/private/field.hpp>

namespace serek
{
	namespace member_name_extractors
	{
		namespace impl
		{
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

			template<typename T, typename... Argv>
			struct concatenator<field_extractor<T>, Argv...>
			{
				using helper_t = field_extractor<T>;
				using result	= typename concatenator<typename helper_t::next_t, typename helper_t::packed_t, Argv...>::result;
			};

			template<typename T, typename... Argv>
			struct concatenator<field_extractor<serek::ffield<T>>, Argv...>
			{
				using result = typename std::tuple<Argv...>;
			};

			template<typename packed_t>
			using members_as_types_result_t = concatenator<packed_t>::result;
		}	 // namespace impl

		template<typename packed_t>
		using members_as_types = typename impl::members_as_types_result_t<packed_t>;

		namespace detail
		{
			template<typename storage_t>
			using storage_appender_t = std::function<void(storage_t&, const serek::str&)>;
		}

		/**
		 * @brief for given serek::pack returns set of members names
		 *
		 * @tparam packed_t serek::pack type to extract member names from
		 * @tparam storage_t type of storage to use
		 * @param appender functor that appends and returns that reference to inserted value
		 * @return storage_t<serek::str>
		 */
		template<typename packed_t, template<typename T> typename storage_t = std::vector>
		storage_t<serek::str> extract_class_members(const detail::storage_appender_t<storage_t<serek::str>> appender = [](auto& storage, const serek::str& item) { storage.emplace_back(item); })
		{
			serek::str members = serek::type_name<members_as_types<packed_t>>();
			storage_t<serek::str> result_set;
			for(size_t i = members.find_first_of('&'); i < members.size() && i != members.npos;)
			{
				const size_t next_ampersand = members.find_first_of('&', i + 1);
				size_t close_shevron;
				if(next_ampersand == serek::str::npos) [[unlikely]]
					close_shevron = members.find_last_not_of(" >") + 1;
				else
					close_shevron = members.find_last_of('>', next_ampersand);
				const size_t member_namespace = members.find_last_of(':', close_shevron);
				const size_t start				= member_namespace + 1;
				const auto tag						= members.substr(start, close_shevron - start);
				appender(result_set, tag);
				i = next_ampersand;
			}
			return result_set;
		}
	}	 // namespace member_name_extractors

}	 // namespace serek
