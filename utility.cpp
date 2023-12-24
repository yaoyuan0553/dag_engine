export module utility;

import <type_traits>;
import <concepts>;

export import <iostream>;
export import <string>;
export import <vector>;

export namespace yy
{

template <typename T>
struct ToString;

}

namespace yy::internal {

template <typename T>
concept external_to_string_lvalue_invocable = requires(ToString<T> obj) {
    { obj(std::declval<T>()) };
};

template <typename T>
concept external_to_string_rvalue_invocable = requires(ToString<T>&& obj) {
    { std::move(obj)(std::declval<T>()) };
};

template <typename T>
concept has_member_to_string = requires(const T a) {
    { a.to_string() } -> std::convertible_to<std::string>;
};

template <typename T>
concept has_external_to_string = requires(T a) {
    { ToString<T>{}(a) } -> std::convertible_to<std::string>;
    requires !external_to_string_lvalue_invocable<T>;
    requires external_to_string_rvalue_invocable<T>;
};

template <typename T>
concept has_to_string = has_member_to_string<T> || has_external_to_string<T>;


}

export namespace yy
{

template <internal::has_to_string T>
std::string to_string(const T& obj) {
    if constexpr (internal::has_member_to_string<T>) {
        return obj.to_string();
    } else if constexpr (internal::has_external_to_string<T>) {
        return ToString<T>{}(obj);
    } else {
        static_assert(!sizeof(T), "unsupported type!");
    }
}

}

export
template <typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T>& vec)
{
    const char* sep = "";
    os << '[';
    for (auto& v: vec) {
        os << sep << v;
        sep = ", ";
    }
    os << ']';
    return os;
}

export
template <yy::internal::has_to_string T>
std::ostream& operator<<(std::ostream& os, const T& obj) {
    yy::to_string(obj);
}

//struct Blah {
//    int x;
//};
//
//namespace yy {
//template <>
//struct ToString<Blah> {
//    std::string operator()(Blah b) const && {
//        return "1";
//    }
//};
//}
//
//static_assert(yy::internal::has_to_string<Blah>);
