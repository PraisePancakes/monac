#ifndef PREP_HPP
#define PREP_HPP
#include <iostream>
#include <istream>
#include <mona/mona.hpp>

#include "../../utils/overload.hpp"
#include "normalizer.hpp"
#include "prep_parsers.hpp"
namespace monac::prep {
using namespace utils;
// takes an input stream of normalized contents, retains dependencies required by this compilation unit, expands macros, and returns some information needed for the module's dependency phase.
// the lifetime of this object should never outlive the lifetime of the input stream.
// the returned info will be the basis of all read only lexical analysis operations (string_view) and parsing, let RVO handle the return contents on return.
// **NOTE**: must take in a normalized string of contents before expansion and parsing.

class preprocessor {
    std::string m_normalized;

    struct prep_info {
        std::string expanded;
    } _m_info;
    std::unordered_map<std::string, parsers::macro_node_t> m_symbol_table;
    void _visit_control(parsers::macro_node_t macro) {
        std::visit(utils::overload{
                       [this](const parsers::variable var) {
                           this->m_symbol_table[var.id] = var;
                       },
                       [this](const parsers::function func) {
                           this->m_symbol_table[func.id] = func;
                       }},
                   macro);
    };

    bool _known_symbol(const std::string& id) const {
        return m_symbol_table.contains(id);
    };

    // @define ADD(a, b) a + b
    // @define FUNC(a, b) b + a
    // FUNC(  1  , ADD(2, 3)); -> 2 + 3 + 1
    std::vector<std::string> _parse_arguments(std::istream& is) {
        if (!mona::_match('(')(is)) throw std::runtime_error("missing opening paren '(' on macro invocation");
        std::vector<std::string> arguments;
        std::string current;
        unsigned int depth = 0;
        while (!mona::_eof()(is)) {
            if (auto lp = mona::_match('(')(is)) {
                ++depth;
                current += lp.value();
            } else if (auto rp = mona::_match(')')(is)) {
                if (depth == 0) {
                    if (!current.empty()) arguments.push_back(current);
                    return arguments;
                }
                --depth;
                current += rp.value();
            } else if (depth == 0 && mona::_match(',')(is)) {
                arguments.push_back(current);
                current.clear();
            } else {
                current += is.get();
            }
        }
        return arguments;
    }

    void _expand_ident(const std::string& id, std::istream& is) {
        if (!_known_symbol(id)) return;
        std::visit(utils::overload{
                       [this, &is](const parsers::function func) {
                           // parse -> replace -> expand
                           std::unordered_map<unsigned int, std::string> args_map;
                           utils::unit::opt_white_space(is);
                           std::vector<std::string> args = _parse_arguments(is);
                           for (auto i{0uz}; i < args.size(); i++) args_map[i] = args[i];

                           std::stringstream new_stream;
                           for (auto i{0uz}; i < func.replacement_list.size(); i++) {
                               std::string r = func.replacement_list[i];
                               if (func.parameters_map.contains(r))
                                   new_stream << args_map.at(func.parameters_map.at(r));
                               else
                                   new_stream << r;
                               if (i < func.replacement_list.size() - 1) new_stream << ' ';
                           };
                           process(new_stream);
                       },
                       [this, &is](const parsers::variable var) {
                           std::stringstream new_stream;
                           for (auto i{0uz}; i < var.replacement_list.size(); i++) {
                               std::string r = var.replacement_list[i];
                               new_stream << r;
                               if (i < var.replacement_list.size() - 1) new_stream << ' ';
                           };
                           process(new_stream);
                       }},
                   this->m_symbol_table[id]);
    };

   public:
    preprocessor() = delete;
    preprocessor(const std::string& normalized) : m_normalized{normalized} {
        std::cout << normalized << std::endl;
    };
    preprocessor(const preprocessor&) = default;
    preprocessor(preprocessor&&) = default;
    preprocessor& operator=(preprocessor&&) = delete;
    preprocessor& operator=(const preprocessor&) = delete;
    ~preprocessor() = default;

    prep_info process(std::istream& stream) {
        while (!mona::_eof()(stream)) {
            auto control = parsers::control_line(stream);
            if (control) {
                _visit_control(control.value());
                continue;
            }
            auto ident = utils::unit::identifier(stream);
            if (ident) {
                if (_known_symbol(ident.value()))
                    _expand_ident(ident.value(), stream);
                else
                    _m_info.expanded.append(ident.value());
            } else
                _m_info.expanded += stream.get();
        };
        return _m_info;
    };

    prep_info process() {
        std::stringstream stream{m_normalized};
        return process(stream);
    };
};
};  // namespace monac::prep

#endif