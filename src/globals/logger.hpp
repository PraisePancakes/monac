#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "singleton.hpp"
namespace glob {

#define LIST_OF_LOG_TYPES \
    X(FATAL)              \
    X(ERROR)              \
    X(WARNING)            \
    X(EXCEPT)             \
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

namespace _internal_glob {
struct log_data {
#define X(V) \
    unsigned int V##S;
    LIST_OF_LOG_TYPES
#undef X
};

class logger final : public singleton<logger> {
    log_data _m_data;
    void update_log_data(const log_info& i) {
        switch (i.type) {
#define X(V)                    \
    case log_info::LOG_TYPE::V: \
        _m_data.V##S++;         \
        break;
            LIST_OF_LOG_TYPES
#undef X
        };
    };
    std::ostream* _m_nonown_stream;

   public:
    void log(const log_info& info, std::ostream& os) {
        if (!_m_nonown_stream) _m_nonown_stream = &os;
        update_log_data(info);
        os << "[MONAC-" << info.type_to_string(info.type) << "] " << info.what << std::endl;
    };

    void report() {
        if (_m_nonown_stream) {
            (*_m_nonown_stream) << "Generated: \n";
#define X(V) \
    (*_m_nonown_stream) << #V << "S " << _m_data.V##S << "\n";
            LIST_OF_LOG_TYPES
#undef X
        }
    };
    ~logger() {};
};

static logger& out = logger::instance();
}  // namespace _internal_glob

void log(log_info::LOG_TYPE type, const std::string& what, std::ostream& out_stream) {
    _internal_glob::out.log(log_info{.type = type, .what = what}, out_stream);
};
void report() {
    _internal_glob::out.report();
};
};  // namespace glob
#endif