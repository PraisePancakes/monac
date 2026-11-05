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
    std::ostream* m_nonown_stream;

   public:
    void log(const log_info& info, std::ostream& os) {
        if (!m_nonown_stream) m_nonown_stream = &os;
        update_log_data(info);
        os << "[MONAC-" << info.type_to_string(info.type) << "] " << info.what << std::endl;
    };

    void report() {
        if (m_nonown_stream) {
            (*m_nonown_stream) << "Generated: \n";
#define X(V) \
    (*m_nonown_stream) << #V << "S " << m_data.V##S << "\n";
            LIST_OF_LOG_TYPES
#undef X
        }
    };
    ~logger() {};
};

};  // namespace glob