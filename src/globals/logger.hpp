#pragma once
#include <unordered_set>
#include <vector>

#include "singleton.hpp"
namespace glob {
#define LIST_OF_LOG_TYPES \
    X(FATAL)              \
    X(ERROR)              \
    X(WARNING)            \
    X(INFO)

struct log_info {
    enum class LOG_TYPE {
#define X(V) V,
        LIST_OF_LOG_TYPES
#undef X
    } type;

    static std::string type_to_string(LOG_TYPE t) {
        switch (t) {
#define X(V)          \
    case LOG_TYPE::V: \
        return #V;
            LIST_OF_LOG_TYPES
#undef X
        };
    };

    std::string what;
};

template <typename...>
inline constexpr auto is_unique_set = std::true_type{};

template <typename T, typename... Ts>
inline constexpr auto is_unique_set<T, Ts...> = std::conditional_t<((std::is_same_v<T, Ts> || ...) || !is_unique_set<Ts...>), std::false_type, std::true_type>::value;

struct log_data {
#define X(V) \
    unsigned int V##S;
    LIST_OF_LOG_TYPES
#undef X
};

class logger : public singleton<logger> {
    log_data m_data;
    void update_log_data(const log_info& i) {
        switch (i.type) {
#define X(V)                    \
    case log_info::LOG_TYPE::V: \
        m_data.V##S++;          \
        break;
            LIST_OF_LOG_TYPES
#undef X
        };
    };
    std::unordered_set<std::ostream*> m_non_owning_stream_set;
    std::vector<std::ostream*> m_non_owning_streams;

    void add_stream(std::ostream& os) {
        if (!m_non_owning_stream_set.contains(&os)) {
            m_non_owning_stream_set.insert(&os);
            m_non_owning_streams.push_back(&os);
        };
    }

   public:
    template <typename... Streams>
        requires((std::is_base_of_v<std::basic_ostream<char>, std::remove_reference_t<Streams>> && ...) &&
                 is_unique_set<std::remove_cvref_t<Streams>...>)
    void log(const log_info& info, Streams&&... s) {
        (add_stream(s), ...);
        update_log_data(info);
        ((std::forward<Streams>(s) << "[MONAC-" << info.type_to_string(info.type) << "] " << info.what << std::endl), ...);
    };

    void report() {
        for (const auto& s : m_non_owning_streams) {
            if (s) {
                (*s) << "Generated: \n";
#define X(V) \
    (*s) << #V << "S " << m_data.V##S << "\n";
                LIST_OF_LOG_TYPES
#undef X
            }
        };
    };
    ~logger() {};
};

};  // namespace glob