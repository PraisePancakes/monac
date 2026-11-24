#ifndef TRAITS_HPP
#define TRAITS_HPP
#include <iostream>
namespace utils::traits {
template <typename T>
concept char_like = std::same_as<std::remove_cv_t<T>, char> ||
                    std::same_as<std::remove_cv_t<T>, signed char> ||
                    std::same_as<std::remove_cv_t<T>, unsigned char> ||
                    std::same_as<std::remove_cv_t<T>, wchar_t> ||
                    std::same_as<std::remove_cv_t<T>, char8_t> ||
                    std::same_as<std::remove_cv_t<T>, char16_t> ||
                    std::same_as<std::remove_cv_t<T>, char32_t>;

template <typename T>
concept tuple_like = requires { std::tuple_size<T>::value; };
};  // namespace utils::traits

#endif