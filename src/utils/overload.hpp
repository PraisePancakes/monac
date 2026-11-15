#pragma once

namespace utils {
template <typename... Fs>
struct overload : Fs... {
    using Fs::operator()...;
};

template <typename... Ts>
overload(Ts...) -> overload<Ts...>;
};  // namespace utils