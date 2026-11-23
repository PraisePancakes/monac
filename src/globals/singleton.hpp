#ifndef SINGLETON_HPP
#define SINGLETON_HPP
namespace glob {

template <typename T_>
class singleton {
    singleton(const singleton&) = delete;
    singleton(singleton&&) = delete;
    singleton& operator=(singleton&&) = delete;
    singleton& operator=(const singleton&) = delete;

   protected:
    singleton() = default;
    virtual ~singleton() {};

   public:
    static T_& instance() {
        static T_ inst;
        return inst;
    };
};
};  // namespace glob
#endif