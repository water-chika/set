#pragma once

#include <variant>

namespace set {
    // static set: a type
    template<typename T>
    struct is_set {
        static constexpr bool value = false;
    };
    template<typename T>
    constexpr bool is_set_v = is_set<T>::value;
    template<typename T>
    concept set = is_set_v<T>;


    template<typename A, typename B>
    struct is_same_set {
        static_assert(std::is_same_v<A,B>, "not sure A and B is same set");
        static constexpr bool value = std::is_same_v<A,B>;
    };

    template<typename A, typename B>
    constexpr bool is_same_set_v = is_same_set<A,B>::value;

    template<typename A, typename B>
    concept set_same_as = is_same_set_v<A, B>;

    template<typename Set>
    struct is_countable {
        static_assert(std::is_arithmetic_v<Set> && std::numeric_limits<Set>::is_exact, "not sure Set is countable");
        static constexpr bool value = std::is_arithmetic_v<Set> && std::numeric_limits<Set>::is_exact;
    };
    template<typename Set>
    constexpr bool is_countable_v = is_countable<Set>::value;

    template<typename A, typename B>
    struct is_subset {
        static_assert(is_same_set_v<A,B>, "not sure A is subset of B");
        static constexpr bool value = is_same_set_v<A,B>;
    };
    template<typename A, typename B>
    constexpr bool is_subset_v = is_subset<A,B>::value;

    template<typename A, typename B> 
    concept subset = is_subset_v<A, B>;

    template<std::integral Int>
    struct is_set<Int> {
        static constexpr bool value = true;
    };
    template<std::integral A, std::integral B>
    struct is_subset<A, B> {
        static constexpr bool value = std::numeric_limits<A>::min() >= std::numeric_limits<B>::min()
                                    && std::numeric_limits<A>::max() <= std::numeric_limits<B>::max();
    };

    template<std::floating_point Float>
    struct is_set<Float> {
        static constexpr bool value = true;
    };
    template<std::floating_point A, std::floating_point B>
    struct is_subset<A, B> {
        static constexpr bool value =
           std::numeric_limits<A>::has_infinity == std::numeric_limits<B>::has_infinity &&
           std::numeric_limits<A>::has_quiet_NaN == std::numeric_limits<B>::has_quiet_NaN &&
           std::numeric_limits<A>::has_signaling_NaN == std::numeric_limits<B>::has_signaling_NaN &&
           std::numeric_limits<A>::has_denorm == std::numeric_limits<B>::has_denorm &&
           std::numeric_limits<A>::radix == std::numeric_limits<B>::radix &&
           std::numeric_limits<A>::digits <= std::numeric_limits<B>::digits &&
           std::numeric_limits<A>::min_exponent >= std::numeric_limits<B>::min_exponent &&
           std::numeric_limits<A>::max_exponent <= std::numeric_limits<B>::max_exponent;
    };

    template<std::integral A, std::floating_point B>
    struct is_subset<A, B> {
        static_assert(std::numeric_limits<A>::radix == std::numeric_limits<B>::radix, "not sure A is subset of B");
        static constexpr bool value =
            std::numeric_limits<A>::radix == std::numeric_limits<B>::radix &&
            std::numeric_limits<A>::digits <= std::numeric_limits<B>::digits;
    };

    template<template<typename> typename Temp, typename A0, typename... An>
    constexpr bool all_substitute = Temp<A0>::value && all_substitute<Temp, An...>;
    template<template<typename> typename Temp, typename A0>
    constexpr bool all_substitute<Temp, A0> = Temp<A0>::value;

    template<typename... An>
    struct is_set<typename std::variant<An...>> {
        static constexpr bool value = all_substitute<is_set, An...>;
    };
    template<typename... An>
    struct is_countable<typename std::variant<An...>> {
        static constexpr bool value = all_substitute<is_countable, An...>;
    };

    template<template<typename> typename Temp, typename A0, typename... An>
    constexpr bool exist_substitute = Temp<A0>::value || exist_substitute<Temp, An...>;
    template<template<typename> typename Temp, typename A0>
    constexpr bool exist_substitute<Temp, A0> = Temp<A0>::value;

    template<typename A, typename... Bn>
    struct is_subset<A, std::variant<Bn...>> {
        template<typename B>
        struct A_is_subset {
            static constexpr bool value = is_subset_v<A, B>;
        };
        static constexpr bool value = exist_substitute<A_is_subset, Bn...>;
    };
}

#include <bitset>
#include <set>
#include <unordered_set>
#include <flat_set>

namespace dynamic_set{
    // dynamic set: a value of a type
    bool is_set(int) {
        return false;
    }
    template<size_t N>
    bool is_set(std::bitset<N>) {
        return true;
    }
    template<typename T>
    bool is_set(std::set<T>) {
        return true;
    }
    template<typename T>
    bool is_set(std::unordered_set<T>) {
        return true;
    }
    template<typename T>
    bool is_set(std::flat_set<T>) {
        return true;
    }

    template<typename F>
    class satisfied_set {
    public:
        satisfied_set(F p) : m_p{std::move(p)}
        {}
        bool contains(auto element) {
            return m_p(element);
        }
    private:
        F m_p;
    };
    template<typename F>
    bool is_set(satisfied_set<F>) {
        return true;
    }
    template<typename F>
    bool belong_to(auto element, satisfied_set<F> s) {
        return s.contains(element);
    }
    template<typename F>
    bool contains(satisfied_set<F> s, auto element) {
        return s.contains(element);
    }
}
