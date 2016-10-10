#ifndef SIPLASPLAS_UTILITY_META_H
#define SIPLASPLAS_UTILITY_META_H

#include "array_view.hpp"
#include <type_traits>
#include <cstdint>

namespace cpp
{

namespace meta
{
    template<typename T>
    using decay_t = typename std::decay<T>::type;

    template<typename... Bs>
    struct assert;

    template<typename B, typename... Bs>
    struct assert<B, Bs...> : assert<Bs...>
    {
        static_assert(B::value, "Assertion failed");
    };

    template<>
    struct assert<> {};

    template<typename T>
    using void_t = typename std::conditional<sizeof(T) >= 0, void, T>::type;

    template<typename T>
    struct identity
    {
        using type = T;
    };

    template<typename Metafunction>
    using type_t = typename Metafunction::type;

    template<typename MetafunctionClass, typename... Args>
    using apply_t = type_t<typename MetafunctionClass::template apply<Args...>>;

    template<typename MetafunctionClass, typename Seq>
    struct sequence_apply;

    template<typename MetafunctionClass,
             template<typename...> class Seq, typename... Ts>
    struct sequence_apply<MetafunctionClass, Seq<Ts...>>
    {
        using type = apply_t<MetafunctionClass, Ts...>;
    };

    template<typename MetafunctionClass, typename Seq>
    using sequence_apply_t = type_t<sequence_apply<MetafunctionClass, Seq>>;

    template<std::uint8_t I>
    using uint8_t = std::integral_constant<std::uint8_t, I>;
    template<std::uint16_t I>
    using uint16_t = std::integral_constant<std::uint16_t, I>;
    template<std::uint32_t I>
    using uint32_t = std::integral_constant<std::uint32_t, I>;
    template<std::uint64_t I>
    using uint64_t = std::integral_constant<std::uint64_t, I>;

    template<std::int8_t I>
    using int8_t = std::integral_constant<std::int8_t, I>;
    template<std::int16_t I>
    using int16_t = std::integral_constant<std::int16_t, I>;
    template<std::int32_t I>
    using int32_t = std::integral_constant<std::int32_t, I>;
    template<std::int64_t I>
    using int64_t = std::integral_constant<std::int64_t, I>;

    template<std::size_t I>
    using size_t = std::integral_constant<std::size_t, I>;

    template<bool B>
    using bool_ = std::integral_constant<bool, B>;
    using true_ = bool_<true>;
    using false_ = bool_<false>;

    template<char C>
    using char_ = std::integral_constant<char, C>;

    template<typename T, typename = void>
    struct is_integral : false_ {};
    template<typename T>
    struct is_integral<T, void_t<decltype(T::value)>> :
        true_
    {};

    template<typename T, typename = assert<is_integral<T>>>
    constexpr decltype(T::value) value()
    {
        return T::value;
    }

    template<template<typename...> class Function>
    struct defer
    {
        template<typename... Args>
        struct apply
        {
            template<typename Instance, bool = is_integral<Instance>::value>
            struct result
            {
                using type = Instance;
            };

            template<typename Instance>
            struct result<Instance, false>
            {
                using type = type_t<Instance>;
            };

            using type = type_t<result<Function<Args...>>>;
        };
    };

    struct and_
    {
        template<typename Lhs, typename Rhs>
        struct apply : assert<
            is_integral<Lhs>,
            is_integral<Rhs>
        >
        {
            using type = bool_<Lhs::value && Rhs::value>;
        };
    };

    template<typename Lhs, typename Rhs>
    using and_t = apply_t<and_, Lhs, Rhs>;

    struct or_
    {
        template<typename Lhs, typename Rhs>
        struct apply : assert<
            is_integral<Lhs>,
            is_integral<Rhs>
        >
        {
            using type = bool_<Lhs::value || Rhs::value>;
        };
    };

    template<typename Lhs, typename Rhs>
    using or_t = apply_t<or_, Lhs, Rhs>;

    struct add_
    {
        template<typename Lhs, typename Rhs>
        struct apply : assert<
            is_integral<Lhs>,
            is_integral<Rhs>
        >
        {
            using type = std::integral_constant<decltype(Lhs::value + Rhs::value), Lhs::value + Rhs::value>;
        };
    };

    template<typename Lhs, typename Rhs>
    using add_t = apply_t<add_, Lhs, Rhs>;

    template<typename... Ts>
    struct list
    {
        static constexpr std::size_t size = sizeof...(Ts);
    };

    template<char... Chars>
    using string = list<std::integral_constant<char, Chars>...>;

    struct DummyValueToType {};

    template<typename Seq, typename TypeToValue>
    class SequenceToArray;


    template<template<typename...> class Seq, typename Head, typename... Tail, typename TypeToValue>
    class SequenceToArray<Seq<Head, Tail...>, TypeToValue>
    {
    public:
        using value_type = typename TypeToValue::value_type;

        constexpr SequenceToArray() = default;

        static constexpr const value_type& get(std::size_t i)
        {
            return array[i];
        }

        static constexpr ConstArrayView<value_type> get()
        {
            return {Bounds::begin(), Bounds::size()};
        }

        struct Bounds
        {
            static constexpr std::size_t size()
            {
                return sizeof...(Tail) + 1;
            }

            static constexpr const value_type* begin()
            {
                return array;
            }

            static constexpr const value_type* end()
            {
                return array + size();
            }

            static constexpr const value_type* const cbegin()
            {
                return array;
            }

            static constexpr const value_type* const cend()
            {
                return array + size();
            }
        };

        constexpr std::size_t size() const
        {
            return Bounds::size();
        }

        constexpr const value_type& operator[](std::size_t i) const
        {
            return get(i);
        }

        constexpr const value_type* begin() const
        {
            return Bounds::begin();
        }

        constexpr const value_type* end() const
        {
            return Bounds::end();
        }

        constexpr const value_type* const cbegin() const
        {
            return Bounds::cbegin();
        }

        constexpr const value_type* const cend() const
        {
            return Bounds::cend();
        }

    private:
        static constexpr value_type array[] = {
            TypeToValue::template get<Head>(),
            TypeToValue::template get<Tail>()...
        };
    };

    template<template<typename...> class Seq, typename Head, typename... Tail, typename TypeToValue>
    constexpr typename TypeToValue::value_type SequenceToArray<Seq<Head, Tail...>, TypeToValue>::array[sizeof...(Tail) + 1];

    template<template<typename...> class Seq, typename TypeToValue>
    class SequenceToArray<Seq<>, TypeToValue>
    {
    public:
        using value_type = typename TypeToValue::value_type;

        constexpr SequenceToArray() = default;

        static constexpr const value_type& get(std::size_t i)
        {
            return *static_cast<void*>(nullptr);
        }

        struct Bounds
        {
            static constexpr std::size_t size()
            {
                return 0;
            }

            static constexpr const value_type* begin()
            {
                return nullptr;
            }

            static constexpr const value_type* end()
            {
                return nullptr;
            }

            static constexpr const value_type* const cbegin()
            {
                return nullptr;
            }

            static constexpr const value_type* const cend()
            {
                return nullptr;
            }
        };

        static constexpr ConstArrayView<value_type> get()
        {
            return {static_cast<value_type*>(nullptr), static_cast<std::size_t>(0)};
        }

        constexpr std::size_t size() const
        {
            return Bounds::size();
        }

        constexpr const value_type& operator[](std::size_t i) const
        {
            return get(i);
        }

        constexpr const value_type* begin() const
        {
            return Bounds::begin();
        }

        constexpr const value_type* end() const
        {
            return Bounds::end();
        }

        constexpr const value_type* const cbegin() const
        {
            return Bounds::cbegin();
        }

        constexpr const value_type* const cend() const
        {
            return Bounds::cend();
        }
    };

    template<typename T>
    struct IntegralConstantToValue
    {
        using value_type = T;

        template<typename IntegralConstant>
        static constexpr value_type get()
        {
            return IntegralConstant::value;
        }
    };

    template<typename T, T... Values>
    using PackToArray = SequenceToArray<
        list<std::integral_constant<T, Values>...>,
        IntegralConstantToValue<T>
    >;

    template<typename String>
    class StringToArray;

    template<template<typename...> class Seq, char... Chars>
    class StringToArray<Seq<std::integral_constant<char, Chars>...>> :
        public PackToArray<char, Chars..., '\0'>
    {
    public:
        static constexpr const char* c_str()
        {
            return PackToArray<char, Chars..., '\0'>::Bounds::begin();
        }
    };

    template<char... Chars>
    using PackStringToArray = StringToArray<list<std::integral_constant<char, Chars>...>>;

    template<typename Seq>
    struct functor;

    template<template<typename...> class Seq, typename... Ts>
    struct functor<Seq<Ts...>>
    {
        template<typename... Us>
        struct apply
        {
            using type = Seq<Us...>;
        };

        template<template<typename...> class Seq2, typename... Us>
        struct apply<Seq2<Us...>>
        {
            using type = Seq<Us...>;
        };
    };

    template<typename Seq, typename... Ts>
    using apply_functor = apply_t<functor<Seq>, Ts...>;

    template<typename Seq, typename... Ts>
    using functor_t = apply_functor<Seq, Ts...>;

    template<typename Lhs, typename Rhs>
    struct cat;

    template<template<typename...> class Seq,
             typename... Lhs, typename... Rhs>
    struct cat<Seq<Lhs...>, Seq<Rhs...>>
    {
        using type = Seq<Lhs..., Rhs...>;
    };

    template<typename Lhs, typename Rhs>
    using cat_t = type_t<cat<Lhs, Rhs>>;

    template<std::size_t I>
    using index_t = std::integral_constant<std::size_t, I>;

    template<std::size_t... Is>
    using index_sequence = list<index_t<Is>...>;

    namespace detail
    {
        template<typename Left, std::size_t Index, typename Right>
        struct split;

        template<template<typename...> class Seq, std::size_t Index,
                 typename... Left, typename Head, typename... Tail>
        struct split<Seq<Left...>, Index, Seq<Head, Tail...>>
        {
            using next = split<Seq<Left..., Head>, Index - 1, Seq<Tail...>>;

            using before = typename next::before;
            using left  = typename next::left;
            using head  = typename next::head;
            using right = typename next::right;
            using after = typename next::after;
        };

        template<template<typename...> class Seq,
            typename... Left, typename Head, typename... Tail>
        struct split<Seq<Left...>, 0, Seq<Head, Tail...>>
        {
            using before = Seq<Left...>;
            using left  = Seq<Left..., Head>;
            using head  = Head;
            using right = Seq<Head, Tail...>;
            using after = Seq<Tail...>;
        };
    }

    template<std::size_t Index, typename... Ts>
    using pack_split = detail::split<list<>, Index, list<Ts...>>;
    template<std::size_t Index, typename... Ts>
    using pack_split_left_t = typename pack_split<Index, Ts...>::left;
    template<std::size_t Index, typename... Ts>
    using pack_get_t = typename pack_split<Index, Ts...>::head;
    template<std::size_t Index, typename... Ts>
    using pack_split_right_t = typename pack_split<Index, Ts...>::right;
    template<std::size_t Index, typename... Ts>
    using pack_split_before_t = typename pack_split<Index, Ts...>::before;
    template<std::size_t Index, typename... Ts>
    using pack_split_after_t = typename pack_split<Index, Ts...>::after;
    template<typename... Ts>
    using pack_head_t = pack_get_t<0, Ts...>;
    template<typename... Ts>
    using pack_tail_t = pack_split_after_t<0, Ts...>;

    template<std::size_t Index, typename Seq>
    struct split;
    template<std::size_t Index, template<typename...> class Seq, typename... Ts>
    struct split<Index, Seq<Ts...>>
    {
        using splitter = detail::split<Seq<>, Index, Seq<Ts...>>;

        using before = typename splitter::before;
        using left  = typename splitter::left;
        using head  = typename splitter::head;
        using right = typename splitter::right;
        using after = typename splitter::after;
    };
    template<std::size_t Index, typename Seq>
    using split_left_t = typename split<Index, Seq>::left;
    template<std::size_t Index, typename Seq>
    using get_t = typename split<Index, Seq>::head;
    template<std::size_t Index, typename Seq>
    using split_right_t = typename split<Index, Seq>::right;
    template<std::size_t Index, typename Seq>
    using split_before_t = typename split<Index, Seq>::before;
    template<std::size_t Index, typename Seq>
    using split_after_t = typename split<Index, Seq>::after;
    template<typename Seq>
    using head_t = get_t<0, Seq>;
    template<typename Seq>
    using tail_t = split_after_t<0, Seq>;


    template<typename T, typename... Ts>
    using pack_prepend_t = list<T, Ts...>;
    template<typename T, typename... Ts>
    using pack_append_t = list<Ts..., T>;
    template<typename T, std::size_t Index, typename... Ts>
    using pack_insert_t = cat_t<pack_append_t<pack_split_left_t<Index, Ts...>, T>, pack_split_right_t<Index, Ts...>>;
    template<typename T, std::size_t Index, typename... Ts>
    using pack_remove_t = cat_t<pack_split_before_t<Index, Ts...>, pack_split_after_t<Index, Ts...>>;

    template<typename T, typename Seq>
    struct prepend;
    template<typename T, template<typename...> class Seq, typename... Ts>
    struct prepend<T, Seq<Ts...>>
    {
        using type = Seq<T, Ts...>;
    };
    template<typename T, typename Seq>
    struct append;
    template<typename T, template<typename...> class Seq, typename... Ts>
    struct append<T, Seq<Ts...>>
    {
        using type = Seq<Ts..., T>;
    };

    template<typename T, typename Seq>
    using prepend_t = type_t<prepend<T, Seq>>;
    template<typename T, typename Seq>
    using append_t = type_t<append<Seq, T>>;
    template<typename T, std::size_t Index, typename Seq>
    using insert_t = cat_t<append_t<split_left_t<Index, Seq>, T>, split_right_t<Index, Seq>>;
    template<typename T, std::size_t Index, typename Seq>
    using remove_t = cat_t<split_before_t<Index, Seq>, split_after_t<Index, Seq>>;


	template<typename Key, typename Value>
	struct pair
	{
		using key = Key;
		using value = Value;
	};

    template<typename Pair>
    using key_t = typename Pair::key;
    template<typename Pair>
    using value_t = typename Pair::value;

    template<typename... Ts>
    struct inherit : Ts... {};

    template<typename... Ts>
    struct inherit<list<Ts...>> : Ts... {};

	template<typename...>
	struct map;

	template<typename... Keys, typename... Values>
	struct map<pair<Keys, Values>...>
	{
		using keys   = list<Keys...>;
		using values = list<Values...>;
        using pairs  = list<pair<Keys, Values>...>;

        template<typename Key>
        using at_key = type_t<decltype(lookup((inherit<pairs>*)nullptr))>;
	private:
		template<typename Key, typename Value>
        identity<Value> lookup(pair<Key, Value>*);
	};

    template<typename Map>
    using keys_t = typename Map::keys;
    template<typename Map>
    using values_t = typename Map::values;
    template<typename Map>
    using pairs_t = typename Map::pairs;
    template<typename Map, typename Key>
    using at_key = typename Map::template at_key<Key>;

    template<typename Function, typename... Ts>
    struct pack_fmap
    {
        using type = list<apply_t<Function, Ts>...>;
    };

    template<typename Function, typename List>
    struct fmap;

    template<typename Function, template<typename...> class Seq, typename... Ts>
    struct fmap<Function, Seq<Ts...>>
    {
        using type = Seq<apply_t<Function, Ts>...>;
    };

    namespace detail
    {
        template<typename Predicate, typename FilteredSeq, typename Seq>
        struct filter;

        template<typename Predicate,
                 template<typename...> class Seq, typename... Filtered, typename Head, typename... Tail>
        struct filter<Predicate, Seq<Filtered...>, Seq<Head, Tail...>>
        {
            template<typename _Head = Head, bool = value<apply_t<Predicate, _Head>>()>
            struct next
            {
                using type = Seq<Filtered..., Head>;
            };

            template<typename _Head>
            struct next<_Head, false>
            {
                using type = Seq<Filtered...>;
            };

            using type = type_t<filter<Predicate, type_t<next<>>, Seq<Tail...>>>;
        };

        template<typename Predicate,
            template<typename...> class Seq, typename... Filtered>
        struct filter<Predicate, Seq<Filtered...>, Seq<>>
        {
            using type = Seq<Filtered...>;
        };
    }

    template<typename Function, typename Seed, typename Seq>
    struct foldl;

    template<typename Function, typename Seed,
             template<typename...> class Seq, typename Head, typename... Tail>
    struct foldl<Function, Seed, Seq<Head, Tail...>>
    {
        using type = type_t<
            foldl<Function,
                  apply_t<Function, Seed, Head>,
                  Seq<Tail...>
            >
        >;
    };

    template<typename Function, typename Seed,
             template<typename...> class Seq>
    struct foldl<Function, Seed, Seq<>>
    {
        using type = Seed;
    };

    template<typename Function, typename Seed, typename Seq>
    struct foldr;

    template<typename Function, typename Seed,
        template<typename...> class Seq, typename Head, typename... Tail>
    struct foldr<Function, Seed, Seq<Head, Tail...>>
    {
        using type = apply_t<
            Function, Head,
            type_t<foldr<Function, Seed, Seq<Tail...>>>
        >;
    };

    template<typename Function, typename Seed,
        template<typename...> class Seq>
    struct foldr<Function, Seed, Seq<>>
    {
        using type = Seed;
    };

    template<typename Function, typename... Ts>
    using pack_fmap_t = type_t<pack_fmap<Function, Ts...>>;
    template<typename Function, typename Seq>
    using fmap_t = type_t<fmap<Function, Seq>>;

    template<typename Function, typename Seed, typename... Seq>
    using pack_foldl = foldl<Function, Seed, list<Seq...>>;
    template<typename Function, typename Seed, typename... Seq>
    using pack_foldl_t = type_t<pack_foldl<Function, Seed, Seq...>>;
    template<typename Function, typename Seed, typename Seq>
    using foldl_t = type_t<foldl<Function, Seed, Seq>>;

    template<typename Function, typename Seed, typename... Seq>
    using pack_foldr = foldr<Function, Seed, list<Seq...>>;
    template<typename Function, typename Seed, typename... Seq>
    using pack_foldr_t = type_t<pack_foldr<Function, Seed, Seq...>>;
    template<typename Function, typename Seed, typename Seq>
    using foldr_t = type_t<foldr<Function, Seed, Seq>>;

    template<typename Predicate, typename Seq>
    using filter = detail::filter<Predicate, apply_functor<Seq>, Seq>;
    template<typename Predicate, typename Seq>
    using filter_t = type_t<filter<Predicate, Seq>>;
    template<typename Predicate, typename... Seq>
    using pack_filter = detail::filter<Predicate, list<>, list<Seq...>>;
    template<typename Predicate, typename... Seq>
    using pack_filter_t = type_t<pack_filter<Predicate, Seq...>>;

    template<typename Bs>
    using any_of = foldl<or_, false_, Bs>;
    template<typename Bs>
    using any_of_t = foldl_t<or_, false_, Bs>;
    template<typename... Bs>
    using pack_any_of = pack_foldl<or_, false_, Bs...>;
    template<typename... Bs>
    using pack_any_of_t = pack_foldl_t<or_, false_, Bs...>;

    template<typename Bs>
    using any_of = foldl<or_, false_, Bs>;
    template<typename Bs>
    using any_of_t = foldl_t<or_, false_, Bs>;
    template<typename... Bs>
    using pack_any_of = pack_foldl<or_, false_, Bs...>;
    template<typename... Bs>
    using pack_any_of_t = pack_foldl_t<or_, false_, Bs...>;

    template<typename... Seqs>
    using join = foldl<defer<cat>, apply_functor<pack_get_t<0, Seqs...>>, apply_functor<pack_get_t<0, Seqs...>, Seqs...>>;
    template<typename... Seqs>
    using join_t = type_t<join<Seqs...>>;

    namespace detail
    {
        template<template<typename...> class Seq, std::size_t N>
        struct make_index_sequence
        {
            static constexpr std::size_t n = (N % 2) ? ((N - 1) / 2) : (N / 2);
            static constexpr std::size_t m = N - n;

            struct adder
            {
                template<typename T>
                struct apply
                {
                    using type = apply_t<add_, size_t<n>, T>;
                };
            };

            using type = cat_t<
                type_t<make_index_sequence<Seq, n>>,
                fmap_t<adder, type_t<make_index_sequence<Seq, m>>>
            >;
        };

        template<template<typename...> class Seq>
        struct make_index_sequence<Seq, 1>
        {
            using type = Seq<size_t<0>>;
        };

        template<template<typename...> class Seq>
        struct make_index_sequence<Seq, 0>
        {
            using type = Seq<>;
        };
    }

    template<std::size_t N, template<typename...> class Seq = list>
    using make_index_sequence = type_t<detail::make_index_sequence<Seq, N>>;
    template<typename... Ts>
    using make_index_sequence_for = make_index_sequence<sizeof...(Ts)>;
    template<typename Seq>
    struct to_index_sequence;
    template<template<typename...> class Seq, typename... Ts>
    struct to_index_sequence<Seq<Ts...>>
    {
        using type = make_index_sequence<sizeof...(Ts), Seq>;
    };
    template<typename Seq>
    using to_index_sequence_t = type_t<to_index_sequence<Seq>>;

    template<template<typename...> class Zipper, typename... Seqs>
    struct zip
    {

    };

    template<template<typename...> class Function, typename... Args>
    struct bind
    {
        template<typename... Ts>
        struct apply
        {
            using indices = make_index_sequence_for<Ts...>;

        };
    };
}

}

#endif // SIPLASPLAS_UTILITY_META_H
