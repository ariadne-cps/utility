/***************************************************************************
 *            command_line_interface.cpp
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

#include "utility/command_line_interface.hpp"

#include <algorithm>
#include <iostream>

namespace Ariadne {

ArgumentStream::ArgumentStream(const List<String>& args) {
    ARIADNE_PRECONDITION(args.size() > 0)
    for (const auto& arg: args) {
        _args.push(arg);
    }
}

String ArgumentStream::peek() const {
    ARIADNE_PRECONDITION(not empty())
    return _args.front();
}

String ArgumentStream::pop() {
    String value = peek();
    _args.pop();
    return value;
}

Bool ArgumentStream::empty() const {
    return _args.empty();
}

SizeType ArgumentStream::size() const {
    return _args.size();
}

ArgumentPack::ArgumentPack(const String& id, const VoidFunction& processor, Int priority)
    : _id(id), _processor(processor), _priority(priority) {}

const String& ArgumentPack::id() const {
    return _id;
}

Int ArgumentPack::priority() const {
    return _priority;
}

Void ArgumentPack::process() const {
    _processor();
}

class ArgumentParserBase : public ArgumentParserInterface {
  protected:
    ArgumentParserBase(
        const String& short_id,
        const String& long_id,
        const String& instructions,
        Int priority)
        : _short_id(short_id), _long_id(long_id), _instructions(instructions), _priority(priority) {}

  public:
    const String& short_id() const override { return _short_id; }
    const String& long_id() const override { return _long_id; }

    Bool is_consumable(const ArgumentStream& stream) const override;
    ArgumentPack consume(ArgumentStream& stream) const override;
    SizeType help_description_header_size() const override;
    String help_description(SizeType num_chars_to_align_instructions) const override;

    virtual Bool requires_value() const = 0;

  protected:
    virtual VoidFunction create_processor(ArgumentStream& stream) const = 0;

  private:
    String _short_id;
    String _long_id;
    String _instructions;
    Int _priority;
};

SizeType ArgumentParserBase::help_description_header_size() const {
    SizeType result = 4 + long_id().size();
    if (not short_id().empty()) {
        result += 4 + short_id().size();
    }
    if (requires_value()) {
        result += 8;
    }
    return result;
}

String ArgumentParserBase::help_description(SizeType num_chars_to_separate_instructions) const {
    StringStream ss;
    if (short_id().empty()) {
        ss << "[--" << long_id() << "]";
    } else {
        ss << "[-" << short_id() << " | --" << long_id() << "]";
    }
    if (requires_value()) {
        ss << " <value>";
    }
    ss << std::string(num_chars_to_separate_instructions, ' ') << _instructions;
    return ss.str();
}

Bool ArgumentParserBase::is_consumable(const ArgumentStream& stream) const {
    const String argument = stream.peek();
    if (not short_id().empty() and argument == String("-") + short_id()) {
        return true;
    }
    return argument == String("--") + long_id();
}

ArgumentPack ArgumentParserBase::consume(ArgumentStream& stream) const {
    stream.pop();
    return ArgumentPack(long_id(), create_processor(stream), _priority);
}

class ValuedArgumentParserBase : public ArgumentParserBase {
  public:
    ValuedArgumentParserBase(
        const String& short_id,
        const String& long_id,
        const String& instructions,
        Int priority)
        : ArgumentParserBase(short_id, long_id, instructions, priority) {}

    Bool requires_value() const override { return true; }

    VoidFunction create_processor(ArgumentStream& stream) const override final {
        if (stream.empty()) {
            throw MissingArgumentValueException(long_id());
        }
        try {
            return _create_processor(stream);
        } catch (std::exception&) {
            throw InvalidArgumentValueException(long_id());
        }
    }

  protected:
    virtual VoidFunction _create_processor(ArgumentStream& stream) const = 0;
};

class UnvaluedArgumentParserBase : public ArgumentParserBase {
  public:
    UnvaluedArgumentParserBase(
        const String& short_id,
        const String& long_id,
        const String& instructions,
        Int priority)
        : ArgumentParserBase(short_id, long_id, instructions, priority) {}

    Bool requires_value() const override { return false; }
};

class FunctionalValuedArgumentParser : public ValuedArgumentParserBase {
  public:
    FunctionalValuedArgumentParser(
        const String& short_id,
        const String& long_id,
        const String& instructions,
        const ArgumentValueProcessor& processor,
        Int priority)
        : ValuedArgumentParserBase(short_id, long_id, instructions, priority), _processor(processor) {}

  protected:
    VoidFunction _create_processor(ArgumentStream& stream) const override {
        const String value = stream.pop();
        return _processor(value);
    }

  private:
    ArgumentValueProcessor _processor;
};

class FunctionalUnvaluedArgumentParser : public UnvaluedArgumentParserBase {
  public:
    FunctionalUnvaluedArgumentParser(
        const String& short_id,
        const String& long_id,
        const String& instructions,
        const VoidFunction& processor,
        Int priority)
        : UnvaluedArgumentParserBase(short_id, long_id, instructions, priority), _processor(processor) {}

  protected:
    VoidFunction create_processor(ArgumentStream&) const override {
        return _processor;
    }

  private:
    VoidFunction _processor;
};

ArgumentParser make_valued_argument_parser(
    const String& short_id,
    const String& long_id,
    const String& instructions,
    const ArgumentValueProcessor& processor,
    Int priority) {
    return FunctionalValuedArgumentParser(
        short_id, long_id, instructions, processor, priority);
}

ArgumentParser make_unvalued_argument_parser(
    const String& short_id,
    const String& long_id,
    const String& instructions,
    const VoidFunction& processor,
    Int priority) {
    return FunctionalUnvaluedArgumentParser(
        short_id, long_id, instructions, processor, priority);
}

CommandLineContribution::CommandLineContribution(
    const String& name,
    const List<ArgumentParser>& parsers)
    : _name(name), _parsers(parsers) {}

const String& CommandLineContribution::name() const {
    return _name;
}

const List<ArgumentParser>& CommandLineContribution::parsers() const {
    return _parsers;
}

class HelpArgumentParser : public UnvaluedArgumentParserBase {
  public:
    HelpArgumentParser()
        : UnvaluedArgumentParserBase(
              "h", "help", "Show this list of supported arguments", 0) {}

  protected:
    VoidFunction create_processor(ArgumentStream&) const override {
        return [] {};
    }
};

CommandLineInterface::CommandLineInterface()
    : _parsers({HelpArgumentParser()}), _contribution_names() {}

Void CommandLineInterface::_validate_contribution(
    const CommandLineContribution& contribution) const {
    const auto& added = contribution.parsers();
    for (SizeType i = 0; i < added.size(); ++i) {
        const auto& parser = added[i];
        for (const auto& existing: _parsers) {
            if (parser.long_id() == existing.long_id()) {
                throw DuplicateArgumentIdentifierException(parser.long_id());
            }
            if (not parser.short_id().empty() and parser.short_id() == existing.short_id()) {
                throw DuplicateArgumentIdentifierException(parser.short_id());
            }
        }
        for (SizeType j = 0; j < i; ++j) {
            const auto& previous = added[j];
            if (parser.long_id() == previous.long_id()) {
                throw DuplicateArgumentIdentifierException(parser.long_id());
            }
            if (not parser.short_id().empty() and parser.short_id() == previous.short_id()) {
                throw DuplicateArgumentIdentifierException(parser.short_id());
            }
        }
    }
}

CommandLineInterface& CommandLineInterface::add(
    const CommandLineContribution& contribution) {
    if (_contribution_names.contains(contribution.name())) {
        return *this;
    }
    _validate_contribution(contribution);
    for (const auto& parser: contribution.parsers()) {
        _parsers.append(parser);
    }
    _contribution_names.insert(contribution.name());
    return *this;
}

Bool CommandLineInterface::acquire(int argc, const char* argv[]) const {
    List<String> args;
    for (Int i = 0; i < argc; ++i) {
        args.append(String(argv[i]));
    }
    return acquire(args);
}

Bool CommandLineInterface::acquire(const List<String>& args) const {
    ArgumentStream stream(args);
    stream.pop();

    Set<String> acquired_ids;
    List<ArgumentPack> packs;

    while (not stream.empty()) {
        Bool consumed = false;
        for (const auto& parser: _parsers) {
            if (parser.is_consumable(stream)) {
                consumed = true;
                try {
                    ArgumentPack pack = parser.consume(stream);
                    if (pack.id() == "help") {
                        pack.process();
                        _print_help();
                        return false;
                    }
                    if (acquired_ids.contains(pack.id())) {
                        std::clog << "Argument '" << pack.id() << "' specified multiple times.\n\n"
                                  << parser.help_description(4) << std::endl;
                        return false;
                    }
                    acquired_ids.insert(pack.id());
                    packs.append(pack);
                } catch (MissingArgumentValueException& exc) {
                    std::clog << "A value is required by the '" << exc.what()
                              << "' argument, but it is not supplied.\n\n"
                              << parser.help_description(4) << std::endl;
                    return false;
                } catch (InvalidArgumentValueException& exc) {
                    std::clog << "Invalid '" << exc.what()
                              << "' argument value, see the usage below:\n\n"
                              << parser.help_description(4) << std::endl;
                    return false;
                }
                break;
            }
        }
        if (not consumed) {
            std::clog << "Unrecognised command-line argument '" << stream.peek()
                      << "'" << std::endl
                      << std::endl;
            _print_help();
            return false;
        }
    }

    std::stable_sort(
        packs.begin(), packs.end(),
        [](const ArgumentPack& left, const ArgumentPack& right) {
            if (left.priority() != right.priority()) {
                return left.priority() < right.priority();
            }
            return left.id() < right.id();
        });

    for (const auto& pack: packs) {
        pack.process();
    }

    return true;
}

Void CommandLineInterface::_print_help() const {
    List<SizeType> header_sizes(_parsers.size(), 0);
    SizeType max_header_size = 0;
    for (SizeType i = 0; i < _parsers.size(); ++i) {
        header_sizes[i] = _parsers[i].help_description_header_size();
        if (header_sizes[i] > max_header_size) {
            max_header_size = header_sizes[i];
        }
    }

    std::clog << "Supported arguments:" << std::endl;
    for (SizeType i = 0; i < _parsers.size(); ++i) {
        std::clog << "    "
                  << _parsers[i].help_description(4 + max_header_size - header_sizes[i])
                  << std::endl;
    }
}

} // namespace Ariadne
