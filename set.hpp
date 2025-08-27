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
#include <algorithm>

namespace dynamic_set{
    // dynamic set: a value of a type
    constexpr bool is_set(int) {
        return false;
    }
    template<size_t N>
    constexpr bool is_set(std::bitset<N>) {
        return true;
    }
    template<typename T>
    constexpr bool is_set(std::set<T>) {
        return true;
    }
    template<typename T>
    constexpr bool is_set(std::unordered_set<T>) {
        return true;
    }
    template<typename T>
    constexpr bool is_set(std::flat_set<T>) {
        return true;
    }

    template<typename F>
    class satisfied_set {
    public:
        constexpr satisfied_set(F p) : m_p{std::move(p)}
        {}
        constexpr bool contains(auto element) {
            return m_p(element);
        }
    private:
        F m_p;
    };
    template<typename F>
    constexpr bool is_set(satisfied_set<F>) {
        return true;
    }
    template<typename F>
    constexpr bool belong_to(auto element, satisfied_set<F> s) {
        return s.contains(element);
    }
    template<typename F>
    constexpr bool contains(satisfied_set<F> s, auto element) {
        return s.contains(element);
    }

    template<typename T1, typename T2>
    constexpr bool is_subset(std::set<T1> A, std::set<T2> B) {
        return std::ranges::for_each(A,
                [&B](auto a) {
                    std::ranges::contains(B, a);
                }
                );
    }

    template<typename T1, typename T2>
    constexpr bool is_equal(std::set<T1> A, std::set<T2> B) {
        return A.size() == B.size() &&
            is_subset(A, B);
    }
    template<typename T1, typename T2>
    constexpr bool is_proper_subset(std::set<T1> A, std::set<T2> B) {
        return A.size() < B.size() &&
            is_subset(A, B);
    }

    struct empty_set{};
    constexpr bool is_subset(empty_set, auto) {
        return true;
    }

    template<typename S1, typename S2>
    class union_set {
    public:
        union_set(S1 s1, S2 s2) : m_s1{s1}, m_s2{s2}
        {}
        constexpr bool contains(auto element) {
            return contains(m_s1, element) || contains(m_s2, element);
        }
    private:
        S1 m_s1;
        S2 m_s2;
    };
    template<typename S1, typename S2>
    constexpr bool is_set(union_set<S1,S2>) { return true; }
    template<typename S1, typename S2>
    constexpr bool is_subset(S1, union_set<S1,S2>) { return true; }
    template<typename S1, typename S2>
    constexpr bool is_subset(S2, union_set<S1,S2>) { return true; }

    template<typename S1, typename S2>
    class intersection_set {
    public:
        intersection_set(S1 s1, S2 s2) : m_s1{s1}, m_s2{s2}
        {}
        constexpr bool contains(auto element) {
            return contains(m_s1, element) && contains(m_s2, element);
        }
    private:
        S1 m_s1;
        S2 m_s2;
    };
    template<typename S1, typename S2>
    constexpr bool is_set(intersection_set<S1,S2>) { return true; }
    template<typename S1, typename S2>
    constexpr bool is_subset(intersection_set<S1,S2>, S1) { return true; }
    template<typename S1, typename S2>
    constexpr bool is_subset(intersection_set<S1,S2>, S2) { return true; }

    template<typename T1, typename T2>
    constexpr bool is_disjoint(std::set<T1> A, std::set<T2> B) {
        return std::ranges::for_each(A,
                [&B](auto a) {
                    return !contains(B, a);
                }
                );
    }

    template<typename S1, typename S2>
    class cartesian_product_set {
    public:
        cartesian_product_set(S1 s1, S2 s2) : m_s1{s1}, m_s2{s2}
        {}
        constexpr bool contains(auto element) {
            auto& [a,b] = element;
            return contains(m_s1, a) && contains(m_s2, b);
        }
    private:
        S1 m_s1;
        S2 m_s2;
    };

    template<typename T1, typename T2>
    constexpr auto cartesian_product(std::set<T1> A, std::set<T2> B) {
        return cartesian_product_set(A, B);
    }
}
