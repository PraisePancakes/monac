#ifndef PREP_HPP
#define PREP_HPP
#include <iostream>
#include <istream>
#include <mona/mona.hpp>

#include "../../utils/overload.hpp"
#include "../../utils/unit_parsers.hpp"
#include "normalizer.hpp"
#include "parsers.hpp"
namespace monac::prep {
using namespace utils;
// takes an input stream of normalized contents, retains dependencies required by this compilation unit, expands macros, and returns some information needed for the module's dependency phase.
// the lifetime of this object should never outlive the lifetime of the input stream.
// the returned info will be the basis of all read only lexical analysis operations (string_view) and parsing, let RVO handle the return contents on return.
// **NOTE**: must take in a normalized string of contents before expansion and parsing.

class preprocessor {
    std::string normalized;

    struct prep_info {
        std::string expanded;
    } m_info;

    std::unordered_map<std::string, prep::parsers::macro_definition> macro_map;
    void visit_macro_definition(prep::parsers::macro_definition def) {
        std::visit(utils::overload{
                       [this](prep::parsers::object obj) { this->macro_map[obj.identifier] = obj; },
                       [this](prep::parsers::function func) { this->macro_map[func.identifier] = func; },
                       [](auto) {}},
                   def);
    };
    std::string visit_macro_replacement(const std::string& key) const {
        if (!macro_map.contains(key)) return key;
        return std::visit(utils::overload{
                              [this](const prep::parsers::object obj) -> std::string {
                                  std::string result{""};
                                  for (const std::string& s : obj.replacement)
                                      result.append(visit_macro_replacement(s));
                                  return result;
                              },
                              [this](const prep::parsers::function func) -> std::string {
                                  std::string result{""};
                                  for (const std::string& s : func.replacement)
                                      result.append(visit_macro_replacement(s));
                                  return result;
                              }},
                          macro_map.at(key));
    };

    void visit_macro_expansion(const std::string& key) {
        if (!macro_map.contains(key)) {
            m_info.expanded.append(key);
            return;
        }
        std::visit(utils::overload{
                       [this](prep::parsers::object& obj) {
                           for (const auto& s : obj.replacement)
                               m_info.expanded.append(visit_macro_replacement(s));
                       },
                       [this](prep::parsers::function& func) {
                           for (const auto& s : func.replacement)
                               m_info.expanded.append(visit_macro_replacement(s));
                       }},
                   macro_map[key]);
    };

    void visit_macro_invocation(prep::parsers::macro_invocation val) {
        std::visit(utils::overload{
                       [this](prep::parsers::call caller) {
                           if (!macro_map.contains(caller.identifier)) {
                               this->m_info.expanded.append(caller.identifier).append("(");
                               for (std::size_t i{0}; i < caller.arguments.size(); i++) {
                                   visit_macro_expansion(caller.arguments[i]);
                                   if (i < caller.arguments.size() - 1) m_info.expanded.append(",");
                               };
                               m_info.expanded.append(")");

                           } else {
                               if (auto* func_def = std::get_if<prep::parsers::function>(&macro_map[caller.identifier])) {
                                   if (func_def->parameters.size() != func_def->parameter_map.size()) {
                                       // throw duplicate parameter
                                   }
                                   if (func_def->parameters.size() != caller.arguments.size()) {
                                       // throw mismatch args size as a preprocessor exception
                                   }
                                   /*
                                    @define ADD(a, b) b + a
                                    ADD(1, 3) -> 3 + 1
                                   */

                                   for (std::size_t i{0}; i < func_def->replacement.size(); i++) {
                                       if (!func_def->parameter_map.contains(func_def->replacement[i])) {
                                           visit_macro_expansion(func_def->replacement[i]);
                                       } else {
                                           visit_macro_expansion(caller.arguments[func_def->parameter_map[func_def->replacement[i]]]);
                                       }
                                   }
                               }
                           }
                       },
                       [this](prep::parsers::identifiable ident) {
                           if (!macro_map.contains(ident.identifier))
                               this->m_info.expanded.append(ident.identifier);
                           else
                               visit_macro_expansion(ident.identifier);
                       }},
                   val);
    };

   public:
    preprocessor() = delete;
    preprocessor(const std::string& normalized) : normalized{normalized} {};
    preprocessor(const preprocessor&) = default;
    preprocessor(preprocessor&&) = default;
    preprocessor& operator=(preprocessor&&) = delete;
    preprocessor& operator=(const preprocessor&) = delete;
    ~preprocessor() = default;

    prep_info process() {
        std::stringstream stream{normalized};
        // don't worry about import/exports. This class will only handle macro replacements (for now) and eventually expansions.
        while (!mona::_eof()(stream)) {
            auto define = parsers::define(stream);
            auto invoke_like = parsers::value(stream);
            if (define.has_value()) {
                visit_macro_definition(define.value());
            } else if (invoke_like.has_value()) {
                visit_macro_invocation(invoke_like.value());
            } else {
                m_info.expanded += stream.get();
            }
        };

        return m_info;
    };
};
};  // namespace monac::prep

#endif