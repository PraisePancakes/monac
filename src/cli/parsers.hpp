#pragma once
#include <mona/mona.hpp>
namespace cli::parsers {
inline auto file_parser = mona::_sequence(mona::_alnum("expected filename"), mona::_match('.', "expected file separator"), mona::_alnum("expected file extension"));
};