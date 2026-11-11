/***************************************************************************
 *            utility/command_line_interface.hpp
 *
 *  Copyright  2007-26  Luca Geretti
 *
 ****************************************************************************/

/*
 *  This file is part of Ariadne Utility.
 *
 *  Ariadne Utility is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ariadne Utility is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne Utility.  If not, see <https://www.gnu.org/licenses/>.
 */

/** @file utility/command_line_interface.hpp
 *  @brief Generic support for composing and acquiring command-line arguments.
 */

#ifndef ARIADNE_UTILITY_COMMAND_LINE_INTERFACE_HPP
#define ARIADNE_UTILITY_COMMAND_LINE_INTERFACE_HPP

#include <functional>
#include <memory>
#include <queue>
#include <stdexcept>
#include <utility>

#include "utility/container.hpp"
#include "utility/macros.hpp"
#include "utility/string.hpp"
#include "utility/typedefs.hpp"

namespace Ariadne {

using VoidFunction = std::function<Void()>;
using ArgumentValueProcessor = std::function<VoidFunction(const String&)>;

class MissingArgumentValueException : public std::runtime_error {
  public:
    explicit MissingArgumentValueException(const String& argument) : std::runtime_error(argument) {}
};

class InvalidArgumentValueException : public std::runtime_error {
  public:
    explicit InvalidArgumentValueException(const String& argument) : std::runtime_error(argument) {}
};

class DuplicateArgumentIdentifierException : public std::runtime_error {
  public:
    explicit DuplicateArgumentIdentifierException(const String& identifier) : std::runtime_error(identifier) {}
};

class ArgumentStream {
  public:
    explicit ArgumentStream(const List<String>& args);

    String peek() const;
    String pop();
    Bool empty() const;
    SizeType size() const;

  private:
    std::queue<String> _args;
};

class ArgumentPack {
  public:
    ArgumentPack(const String& id, const VoidFunction& processor, Int priority = 0);

    const String& id() const;
    Int priority() const;
    Void process() const;

  private:
    String _id;
    VoidFunction _processor;
    Int _priority;
};

class ArgumentParserInterface {
  public:
    virtual const String& short_id() const = 0;
    virtual const String& long_id() const = 0;
    virtual Bool is_consumable(const ArgumentStream& stream) const = 0;
    virtual ArgumentPack consume(ArgumentStream& stream) const = 0;
    virtual SizeType help_description_header_size() const = 0;
    virtual String help_description(SizeType num_chars_to_separate_instructions) const = 0;
    virtual ~ArgumentParserInterface() = default;
};

class ArgumentParser {
  public:
    template<class P>
    ArgumentParser(P parser)
        : _ptr(std::make_shared<P>(std::move(parser))) {}

    const String& short_id() const { return _ptr->short_id(); }
    const String& long_id() const { return _ptr->long_id(); }
    Bool is_consumable(const ArgumentStream& stream) const { return _ptr->is_consumable(stream); }
    ArgumentPack consume(ArgumentStream& stream) const { return _ptr->consume(stream); }
    SizeType help_description_header_size() const { return _ptr->help_description_header_size(); }
    String help_description(SizeType n) const { return _ptr->help_description(n); }

  private:
    std::shared_ptr<const ArgumentParserInterface> _ptr;
};

ArgumentParser make_valued_argument_parser(
    const String& short_id,
    const String& long_id,
    const String& instructions,
    const ArgumentValueProcessor& processor,
    Int priority = 0);

ArgumentParser make_unvalued_argument_parser(
    const String& short_id,
    const String& long_id,
    const String& instructions,
    const VoidFunction& processor,
    Int priority = 0);

//! \brief A named set of command-line arguments contributed by one module.
//! \details Contribution names are used to make repeated composition idempotent.
class CommandLineContribution {
  public:
    CommandLineContribution(const String& name, const List<ArgumentParser>& parsers);

    const String& name() const;
    const List<ArgumentParser>& parsers() const;

  private:
    String _name;
    List<ArgumentParser> _parsers;
};

//! \brief A composable command-line interface.
//! \details The base interface only provides -h/--help. Module-specific
//! arguments are added explicitly through CommandLineContribution objects.
class CommandLineInterface {
  public:
    CommandLineInterface();

    CommandLineInterface& add(const CommandLineContribution& contribution);

    Bool acquire(int argc, const char* argv[]) const;
    Bool acquire(const List<String>& args) const;

  private:
    Void _print_help() const;
    Void _validate_contribution(const CommandLineContribution& contribution) const;

    List<ArgumentParser> _parsers;
    Set<String> _contribution_names;
};

} // namespace Ariadne

#endif // ARIADNE_UTILITY_COMMAND_LINE_INTERFACE_HPP
