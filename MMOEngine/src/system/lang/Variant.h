/**
 * @author      : theanswer (theanswer@hyperv)
 * @file        : Variant
 * @created     : Sunday Nov 24, 2019 15:15:36 UTC
 */

#ifndef E3VARIANT_H
#define E3VARIANT_H

#include "variant.hpp"

namespace sys {
	namespace lang {
		template<class ... K>
			using Variant = mpark::variant<K...>;

		template <typename T, typename... Ts>
			inline constexpr T &get(Variant<Ts...> &v) {
				return mpark::get<T, Ts...>(v);
			}

		template <typename T, typename... Ts>
			inline constexpr T &&get(Variant<Ts...> &&v) {
				return mpark::get<T, Ts...>(std::move(v));
			}

		template <typename T, typename... Ts>
			inline constexpr const T &get(const Variant<Ts...> &v) {
				return mpark::get<T, Ts...>(v);
			}

		template <typename T, typename... Ts>
			inline constexpr const T &&get(const Variant<Ts...> &&v) {
				return mpark::get<T, Ts...>(std::move(v));
			}

		template <typename Visitor, typename... Vs>
			inline constexpr decltype(auto) visit(Visitor &&visitor, Vs &&... vs) {
				return mpark::visit<Visitor, Vs...>(std::move(visitor), std::forward<Vs>(vs)...);
			}

	}
}

#endif /* end of include guard E3VARIANT_H */

