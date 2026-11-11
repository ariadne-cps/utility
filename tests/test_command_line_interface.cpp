/***************************************************************************
 *            test_command_line_interface.cpp
 *
 *  Copyright  2026  Luca Geretti
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

#include <iostream>

#include "utility/command_line_interface.hpp"

#include "ariadne-testing.hpp"

using namespace Ariadne;

class TestCommandLineInterface : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;

    Void test_argument_stream() {
        ARIADNE_TESTING_EXPECT_EXCEPTION(ArgumentStream({}));

        ArgumentStream stream({"program", "first"});
        ARIADNE_TESTING_PROVE(not stream.empty());
        ARIADNE_TESTING_EXPECT_EQUAL(stream.size(), 2u);
        ARIADNE_TESTING_EXPECT_EQUAL(stream.peek(), "program");
        ARIADNE_TESTING_EXPECT_EQUAL(stream.pop(), "program");
        ARIADNE_TESTING_EXPECT_EQUAL(stream.pop(), "first");
        ARIADNE_TESTING_PROVE(stream.empty());
        ARIADNE_TESTING_EXPECT_EXCEPTION(stream.peek());
        ARIADNE_TESTING_EXPECT_EXCEPTION(stream.pop());
    }

    Void test_argument_pack() {
        Bool processed = false;
        ArgumentPack pack("test", [&processed] { processed = true; }, -3);
        ARIADNE_TESTING_EXPECT_EQUAL(pack.id(), "test");
        ARIADNE_TESTING_EXPECT_EQUAL(pack.priority(), -3);
        pack.process();
        ARIADNE_TESTING_PROVE(processed);
    }

    Void test_parser_metadata_and_help() {
        ArgumentParser valued = make_valued_argument_parser(
            "v", "value", "Set a value",
            [](const String&) -> VoidFunction { return [] {}; });
        ArgumentParser flag = make_unvalued_argument_parser(
            "", "flag", "Enable a flag", [] {});

        ARIADNE_TESTING_EXPECT_EQUAL(valued.short_id(), "v");
        ARIADNE_TESTING_EXPECT_EQUAL(valued.long_id(), "value");
        ARIADNE_TESTING_EXPECT_EQUAL(flag.short_id(), "");
        ARIADNE_TESTING_EXPECT_EQUAL(flag.long_id(), "flag");
        ARIADNE_TESTING_EXPECT_GREATER(valued.help_description_header_size(), flag.help_description_header_size());
        ARIADNE_TESTING_PROVE(valued.help_description(2).find("<value>") != String::npos);
        ARIADNE_TESTING_PROVE(flag.help_description(2).find("[--flag]") != String::npos);

        ArgumentStream short_stream({"-v", "1"});
        ArgumentStream long_stream({"--value", "1"});
        ArgumentStream other_stream({"--other"});
        ArgumentStream long_only_stream({"--flag"});
        ARIADNE_TESTING_PROVE(valued.is_consumable(short_stream));
        ARIADNE_TESTING_PROVE(valued.is_consumable(long_stream));
        ARIADNE_TESTING_PROVE(not valued.is_consumable(other_stream));
        ARIADNE_TESTING_PROVE(flag.is_consumable(long_only_stream));
    }

    Void test_base_interface() {
        CommandLineInterface cli;
        ARIADNE_TESTING_PROVE(cli.acquire({""}));
        ARIADNE_TESTING_PROVE(not cli.acquire({"", "--unknown"}));
        ARIADNE_TESTING_PROVE(not cli.acquire({"", "-h"}));
        ARIADNE_TESTING_PROVE(not cli.acquire({"", "--help"}));

        const char* argv[] = {"program"};
        ARIADNE_TESTING_PROVE(cli.acquire(1, argv));
    }

    Void test_composition() {
        Nat value = 0u;
        Bool flag = false;

        CommandLineContribution contribution(
            "test",
            {make_valued_argument_parser(
                 "v", "value", "Set a non-negative value",
                 [&value](const String& argument) -> VoidFunction {
                     const Int parsed = std::stoi(argument);
                     if (parsed < 0) {
                         throw std::runtime_error("negative");
                     }
                     return [&value, parsed] {
                         value = static_cast<Nat>(parsed);
                     };
                 }),
             make_unvalued_argument_parser(
                 "f", "flag", "Enable the flag",
                 [&flag] { flag = true; })});

        ARIADNE_TESTING_EXPECT_EQUAL(contribution.name(), "test");
        ARIADNE_TESTING_EXPECT_EQUAL(contribution.parsers().size(), 2u);

        CommandLineInterface cli;
        cli.add(contribution).add(contribution);

        ARIADNE_TESTING_PROVE(cli.acquire({"", "-v", "3", "--flag"}));
        ARIADNE_TESTING_EXPECT_EQUAL(value, 3u);
        ARIADNE_TESTING_PROVE(flag);

        value = 0u;
        ARIADNE_TESTING_PROVE(cli.acquire({"", "--value", "4"}));
        ARIADNE_TESTING_EXPECT_EQUAL(value, 4u);

        ARIADNE_TESTING_PROVE(not cli.acquire({"", "--value"}));
        ARIADNE_TESTING_PROVE(not cli.acquire({"", "--value", "-1"}));
        ARIADNE_TESTING_PROVE(not cli.acquire({"", "--value", "x"}));
        ARIADNE_TESTING_PROVE(not cli.acquire({"", "--value", "1", "-v", "2"}));
    }

    Void test_validation() {
        CommandLineInterface help_collision;
        ARIADNE_TESTING_EXPECT_EXCEPTION(help_collision.add(
            CommandLineContribution("help-long", {make_unvalued_argument_parser("x", "help", "collision", [] {})})));
        ARIADNE_TESTING_EXPECT_EXCEPTION(help_collision.add(
            CommandLineContribution("help-short", {make_unvalued_argument_parser("h", "other", "collision", [] {})})));

        CommandLineInterface internal_long_collision;
        ARIADNE_TESTING_EXPECT_EXCEPTION(internal_long_collision.add(
            CommandLineContribution("long-collision", {make_unvalued_argument_parser("a", "same", "first", [] {}),
                                                       make_unvalued_argument_parser("b", "same", "second", [] {})})));

        CommandLineInterface internal_short_collision;
        ARIADNE_TESTING_EXPECT_EXCEPTION(internal_short_collision.add(
            CommandLineContribution("short-collision", {make_unvalued_argument_parser("a", "first", "first", [] {}),
                                                        make_unvalued_argument_parser("a", "second", "second", [] {})})));

        CommandLineInterface existing_long_collision;
        existing_long_collision.add(CommandLineContribution("first", {make_unvalued_argument_parser("a", "first", "first", [] {})}));
        ARIADNE_TESTING_EXPECT_EXCEPTION(existing_long_collision.add(
            CommandLineContribution("second", {make_unvalued_argument_parser("b", "first", "second", [] {})})));

        CommandLineInterface existing_short_collision;
        existing_short_collision.add(CommandLineContribution("first", {make_unvalued_argument_parser("a", "first", "first", [] {})}));
        ARIADNE_TESTING_EXPECT_EXCEPTION(existing_short_collision.add(
            CommandLineContribution("second", {make_unvalued_argument_parser("a", "second", "second", [] {})})));

        CommandLineInterface long_only;
        ARIADNE_TESTING_EXPECT_NO_THROW(long_only.add(
            CommandLineContribution("long-only", {make_unvalued_argument_parser("x", "first", "first", [] {}),
                                                  make_unvalued_argument_parser("", "second", "second", [] {})})));
    }

    Void test_priority_and_transactionality() {
        List<String> order;
        Nat value = 0u;

        CommandLineInterface cli;
        cli.add(CommandLineContribution("ordering", {make_unvalued_argument_parser(
                                                         "l", "late", "Late action",
                                                         [&order] { order.append("late"); },
                                                         10),
                                                     make_unvalued_argument_parser(
                                                         "z", "zeta", "Second middle action",
                                                         [&order] { order.append("zeta"); },
                                                         0),
                                                     make_unvalued_argument_parser(
                                                         "a", "alpha", "First middle action",
                                                         [&order] { order.append("alpha"); },
                                                         0),
                                                     make_unvalued_argument_parser(
                                                         "e", "early", "Early action",
                                                         [&order] { order.append("early"); },
                                                         -10),
                                                     make_valued_argument_parser(
                                                         "v", "value", "Set a value",
                                                         [&value](const String& argument) -> VoidFunction {
                                                             const Int parsed = std::stoi(argument);
                                                             return [&value, parsed] { value = static_cast<Nat>(parsed); };
                                                         })}));

        ARIADNE_TESTING_PROVE(cli.acquire({"", "--late", "--zeta", "--early", "--alpha"}));
        ARIADNE_TESTING_EXPECT_EQUAL(order.size(), 4u);
        ARIADNE_TESTING_EXPECT_EQUAL(order[0], "early");
        ARIADNE_TESTING_EXPECT_EQUAL(order[1], "alpha");
        ARIADNE_TESTING_EXPECT_EQUAL(order[2], "zeta");
        ARIADNE_TESTING_EXPECT_EQUAL(order[3], "late");

        ARIADNE_TESTING_PROVE(not cli.acquire({"", "--value", "7", "--unknown"}));
        ARIADNE_TESTING_EXPECT_EQUAL(value, 0u);
    }
};

int main(int argc, char** argv) {
    ARIADNE_TESTING_SUITE("command_line_interface");
    ARIADNE_TESTING_ADD_TEST(TestCommandLineInterface, test_argument_stream);
    ARIADNE_TESTING_ADD_TEST(TestCommandLineInterface, test_argument_pack);
    ARIADNE_TESTING_ADD_TEST(TestCommandLineInterface, test_parser_metadata_and_help);
    ARIADNE_TESTING_ADD_TEST(TestCommandLineInterface, test_base_interface);
    ARIADNE_TESTING_ADD_TEST(TestCommandLineInterface, test_composition);
    ARIADNE_TESTING_ADD_TEST(TestCommandLineInterface, test_validation);
    ARIADNE_TESTING_ADD_TEST(TestCommandLineInterface, test_priority_and_transactionality);
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc, argv);
}
