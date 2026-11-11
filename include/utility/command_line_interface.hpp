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

/** @brief Exception reporting missing argument value. */
class MissingArgumentValueException : public std::runtime_error {
  public:
    /** @brief Construct a \c MissingArgumentValueException from argument. */
    explicit MissingArgumentValueException(const String& argument) : std::runtime_error(argument) {}
};

/** @brief Exception reporting invalid argument value. */
class InvalidArgumentValueException : public std::runtime_error {
  public:
    /** @brief Construct a \c InvalidArgumentValueException from argument. */
    explicit InvalidArgumentValueException(const String& argument) : std::runtime_error(argument) {}
};

/** @brief Exception reporting duplicate argument identifier. */
class DuplicateArgumentIdentifierException : public std::runtime_error {
  public:
    /** @brief Construct a \c DuplicateArgumentIdentifierException from identifier. */
    explicit DuplicateArgumentIdentifierException(const String& identifier) : std::runtime_error(identifier) {}
};

/** @brief Cursor over command-line argument tokens. */
class ArgumentStream {
  public:
    /** @brief Construct a \c ArgumentStream from args. */
    explicit ArgumentStream(const List<String>& args);

    /** @brief Inspect the next command-line token without consuming it. */
    String peek() const;
    /** @brief Consume the next command-line token. */
    String pop();
    /** @brief Test whether the container is empty. */
    Bool empty() const;
    /** @brief Return the number of elements. */
    SizeType size() const;

  private:
    std::queue<String> _args;
};

/** @brief Parsed argument identifier, value processor, and priority. */
class ArgumentPack {
  public:
    /** @brief Construct a \c ArgumentPack from id, processor, priority. */
    ArgumentPack(const String& id, const VoidFunction& processor, Int priority = 0);

    /** @brief Return the argument identifier. */
    const String& id() const;
    /** @brief Return the processing priority. */
    Int priority() const;
    /** @brief Process the supplied command-line argument. */
    Void process() const;

  private:
    String _id;
    VoidFunction _processor;
    Int _priority;
};

/** @brief Polymorphic interface for argument parser. */
class ArgumentParserInterface {
  public:
    /** @brief Return the short option identifier. */
    virtual const String& short_id() const = 0;
    /** @brief Return the long option identifier. */
    virtual const String& long_id() const = 0;
    /** @brief Test whether the next token matches this parser. */
    virtual Bool is_consumable(const ArgumentStream& stream) const = 0;
    /** @brief Consume and process the matching command-line option. */
    virtual ArgumentPack consume(ArgumentStream& stream) const = 0;
    /** @brief Return the width of the option header in help output. */
    virtual SizeType help_description_header_size() const = 0;
    /** @brief Return the option help text. */
    virtual String help_description(SizeType num_chars_to_separate_instructions) const = 0;
    /** @brief Destroy the object and release its owned resources. */
    virtual ~ArgumentParserInterface() = default;
};

/** @brief Concrete parser matching and processing one command-line option. */
class ArgumentParser {
  public:
    /** @brief Construct a \c ArgumentParser from parser. */
    template<class P>
    ArgumentParser(P parser)
        : _ptr(std::make_shared<P>(std::move(parser))) {}

    /** @brief Return the short option identifier. */
    const String& short_id() const { return _ptr->short_id(); }
    /** @brief Return the long option identifier. */
    const String& long_id() const { return _ptr->long_id(); }
    /** @brief Test whether the next token matches this parser. */
    Bool is_consumable(const ArgumentStream& stream) const { return _ptr->is_consumable(stream); }
    /** @brief Consume and process the matching command-line option. */
    ArgumentPack consume(ArgumentStream& stream) const { return _ptr->consume(stream); }
    /** @brief Return the width of the option header in help output. */
    SizeType help_description_header_size() const { return _ptr->help_description_header_size(); }
    /** @brief Return the option help text. */
    String help_description(SizeType n) const { return _ptr->help_description(n); }

  private:
    std::shared_ptr<const ArgumentParserInterface> _ptr;
};

/** @brief Create a parser for an option followed by a value. */
ArgumentParser make_valued_argument_parser(
    const String& short_id,
    const String& long_id,
    const String& instructions,
    const ArgumentValueProcessor& processor,
    Int priority = 0);

/** @brief Create a parser for a flag without an associated value. */
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
    /** @brief Construct a \c CommandLineContribution from name, parsers. */
    CommandLineContribution(const String& name, const List<ArgumentParser>& parsers);

    /** @brief Return the name. */
    const String& name() const;
    /** @brief Return the registered argument parsers. */
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
    /** @brief Construct a \c CommandLineInterface with the specified characteristics. */
    CommandLineInterface();

    /** @brief Add the contribution and validate its command-line option identifiers. */
    CommandLineInterface& add(const CommandLineContribution& contribution);

    /** @brief Acquire the supplied arguments or working storage. */
    Bool acquire(int argc, const char* argv[]) const;
    /** @brief Acquire the supplied arguments or working storage. */
    Bool acquire(const List<String>& args) const;

  private:
    /** @brief Print usage and descriptions for the registered command-line options. */
    Void _print_help() const;
    /** @brief Validate a contribution before adding its command-line parsers. */
    Void _validate_contribution(const CommandLineContribution& contribution) const;

    List<ArgumentParser> _parsers;
    Set<String> _contribution_names;
};

} // namespace Ariadne

#endif // ARIADNE_UTILITY_COMMAND_LINE_INTERFACE_HPP
