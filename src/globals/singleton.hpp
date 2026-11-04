#pragma once

namespace glob {

template <typename T>
class singleton {
    singleton(const singleton&) = delete;
    singleton(singleton&&) = delete;
    singleton& operator=(singleton&&) = delete;
    singleton& operator=(const singleton&) = delete;

   protected:
    singleton() = default;
    virtual ~singleton() {};

   public:
    static T& instance() {
        static T inst;
        return inst;
    };
};
};  // namespace glob