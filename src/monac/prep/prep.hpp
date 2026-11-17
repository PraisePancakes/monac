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
    void _expand_ident(const std::string& id, std::istream& is) {
        if (!_known_symbol(id)) return;
        std::visit(utils::overload{
                       [this, &is](const parsers::function func) {
                           std::unordered_map<unsigned int, std::string> args_map;
                           unsigned int arg_pos{0};
                           mona::_match('(')(is);
                           utils::unit::opt_white_space(is);
                           while (!mona::_match(')')(is)) {
                               utils::unit::opt_white_space(is);
                               auto token = utils::unit::argument_token(is);
                               utils::unit::opt_white_space(is);
                               if (token) {
                                   args_map[arg_pos++] = token.value();
                                   if (_known_symbol(token.value())) _expand_ident(token.value(), is);
                               }
                               mona::_match(',')(is);
                           }
                           for (auto i{0uz}; i < func.replacement_list.size(); i++) {
                               std::string r = func.replacement_list[i];
                               if (func.parameters_map.contains(r)) {
                                   _m_info.expanded.append(args_map.at(func.parameters_map.at(r)));
                               } else /*eventually needs to be else if known symbol, expand token*/
                                   _m_info.expanded.append(r);
                               if (i < func.replacement_list.size() - 1)
                                   _m_info.expanded.append(" ");
                           };
                       },
                       [this, &is](const parsers::variable var) {
                           for (const auto replacement : var.replacement_list) {
                               if (_known_symbol(replacement))
                                   _expand_ident(replacement, is);
                               else
                                   _m_info.expanded.append(replacement);
                           }
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

    prep_info process() {
        std::stringstream stream{m_normalized};
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
        // don't worry about import/exports. This class will only handle macro replacements (for now) and eventually expansions.

        return _m_info;
    };
};
};  // namespace monac::prep

#endif