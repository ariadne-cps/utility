/***************************************************************************
 *            test_command_line_interface.cpp
 *
 *  Copyright  2026  Luca Geretti
 *
 ****************************************************************************/

#include "utility/command_line_interface.hpp"
#include "testing/fixture.hpp"
#include "testing/checks.hpp"
#include <iostream>

using namespace Ariadne;

class TestCommandLineInterface : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;

    Void test_argument_stream() {
        ARIADNE_EXPECT_EXCEPTION(ctx(), ArgumentStream({}));

        ArgumentStream stream({"program","first"});
        ARIADNE_PROVE(ctx(), not stream.empty());
        ARIADNE_EXPECT_EQUAL(ctx(), stream.size(), 2u);
        ARIADNE_EXPECT_EQUAL(ctx(), stream.peek(), "program");
        ARIADNE_EXPECT_EQUAL(ctx(), stream.pop(), "program");
        ARIADNE_EXPECT_EQUAL(ctx(), stream.pop(), "first");
        ARIADNE_PROVE(ctx(), stream.empty());
        ARIADNE_EXPECT_EXCEPTION(ctx(), stream.peek());
        ARIADNE_EXPECT_EXCEPTION(ctx(), stream.pop());
    }

    Void test_argument_pack() {
        Bool processed=false;
        ArgumentPack pack("test",[&processed]{ processed=true; },-3);
        ARIADNE_EXPECT_EQUAL(ctx(), pack.id(), "test");
        ARIADNE_EXPECT_EQUAL(ctx(), pack.priority(), -3);
        pack.process();
        ARIADNE_PROVE(ctx(), processed);
    }

    Void test_parser_metadata_and_help() {
        ArgumentParser valued=make_valued_argument_parser(
            "v","value","Set a value",
            [](String const&) -> VoidFunction { return []{}; });
        ArgumentParser flag=make_unvalued_argument_parser(
            "","flag","Enable a flag",[]{});

        ARIADNE_EXPECT_EQUAL(ctx(), valued.short_id(), "v");
        ARIADNE_EXPECT_EQUAL(ctx(), valued.long_id(), "value");
        ARIADNE_EXPECT_EQUAL(ctx(), flag.short_id(), "");
        ARIADNE_EXPECT_EQUAL(ctx(), flag.long_id(), "flag");
        ARIADNE_PROVE(ctx(), valued.help_description_header_size() > flag.help_description_header_size());
        ARIADNE_PROVE(ctx(), valued.help_description(2).find("<value>") != String::npos);
        ARIADNE_PROVE(ctx(), flag.help_description(2).find("[--flag]") != String::npos);

        ArgumentStream short_stream({"-v","1"});
        ArgumentStream long_stream({"--value","1"});
        ArgumentStream other_stream({"--other"});
        ArgumentStream long_only_stream({"--flag"});
        ARIADNE_PROVE(ctx(), valued.is_consumable(short_stream));
        ARIADNE_PROVE(ctx(), valued.is_consumable(long_stream));
        ARIADNE_PROVE(ctx(), not valued.is_consumable(other_stream));
        ARIADNE_PROVE(ctx(), flag.is_consumable(long_only_stream));
    }

    Void test_base_interface() {
        CommandLineInterface cli;
        ARIADNE_PROVE(ctx(), cli.acquire({""}));
        ARIADNE_PROVE(ctx(), not cli.acquire({"","--unknown"}));
        ARIADNE_PROVE(ctx(), not cli.acquire({"","-h"}));
        ARIADNE_PROVE(ctx(), not cli.acquire({"","--help"}));

        const char* argv[]={"program"};
        ARIADNE_PROVE(ctx(), cli.acquire(1,argv));
    }

    Void test_composition() {
        Nat value=0u;
        Bool flag=false;

        CommandLineContribution contribution(
            "test",
            {
                make_valued_argument_parser(
                    "v","value","Set a non-negative value",
                    [&value](String const& argument) -> VoidFunction {
                        Int const parsed=std::stoi(argument);
                        if (parsed < 0) {
                            throw std::runtime_error("negative");
                        }
                        return [&value,parsed] {
                            value=static_cast<Nat>(parsed);
                        };
                    }),
                make_unvalued_argument_parser(
                    "f","flag","Enable the flag",
                    [&flag]{ flag=true; })
            });

        ARIADNE_EXPECT_EQUAL(ctx(), contribution.name(), "test");
        ARIADNE_EXPECT_EQUAL(ctx(), contribution.parsers().size(), 2u);

        CommandLineInterface cli;
        cli.add(contribution).add(contribution);

        ARIADNE_PROVE(ctx(), cli.acquire({"","-v","3","--flag"}));
        ARIADNE_EXPECT_EQUAL(ctx(), value, 3u);
        ARIADNE_PROVE(ctx(), flag);

        value=0u;
        ARIADNE_PROVE(ctx(), cli.acquire({"","--value","4"}));
        ARIADNE_EXPECT_EQUAL(ctx(), value, 4u);

        ARIADNE_PROVE(ctx(), not cli.acquire({"","--value"}));
        ARIADNE_PROVE(ctx(), not cli.acquire({"","--value","-1"}));
        ARIADNE_PROVE(ctx(), not cli.acquire({"","--value","x"}));
        ARIADNE_PROVE(ctx(), not cli.acquire({"","--value","1","-v","2"}));
    }

    Void test_validation() {
        CommandLineInterface help_collision;
        ARIADNE_EXPECT_EXCEPTION(ctx(), help_collision.add(
            CommandLineContribution("help-long",{
                make_unvalued_argument_parser("x","help","collision",[]{})
            })));
        ARIADNE_EXPECT_EXCEPTION(ctx(), help_collision.add(
            CommandLineContribution("help-short",{
                make_unvalued_argument_parser("h","other","collision",[]{})
            })));

        CommandLineInterface internal_long_collision;
        ARIADNE_EXPECT_EXCEPTION(ctx(), internal_long_collision.add(
            CommandLineContribution("long-collision",{
                make_unvalued_argument_parser("a","same","first",[]{}),
                make_unvalued_argument_parser("b","same","second",[]{})
            })));

        CommandLineInterface internal_short_collision;
        ARIADNE_EXPECT_EXCEPTION(ctx(), internal_short_collision.add(
            CommandLineContribution("short-collision",{
                make_unvalued_argument_parser("a","first","first",[]{}),
                make_unvalued_argument_parser("a","second","second",[]{})
            })));

        CommandLineInterface existing_long_collision;
        existing_long_collision.add(CommandLineContribution("first",{
            make_unvalued_argument_parser("a","first","first",[]{})
        }));
        ARIADNE_EXPECT_EXCEPTION(ctx(), existing_long_collision.add(
            CommandLineContribution("second",{
                make_unvalued_argument_parser("b","first","second",[]{})
            })));

        CommandLineInterface existing_short_collision;
        existing_short_collision.add(CommandLineContribution("first",{
            make_unvalued_argument_parser("a","first","first",[]{})
        }));
        ARIADNE_EXPECT_EXCEPTION(ctx(), existing_short_collision.add(
            CommandLineContribution("second",{
                make_unvalued_argument_parser("a","second","second",[]{})
            })));

        CommandLineInterface long_only;
        ARIADNE_EXPECT_NO_THROW(ctx(), long_only.add(
            CommandLineContribution("long-only",{
                make_unvalued_argument_parser("x","first","first",[]{}),
                make_unvalued_argument_parser("","second","second",[]{})
            })));
    }

    Void test_priority_and_transactionality() {
        List<String> order;
        Nat value=0u;

        CommandLineInterface cli;
        cli.add(CommandLineContribution("ordering",{
            make_unvalued_argument_parser(
                "l","late","Late action",
                [&order]{ order.append("late"); },
                10),
            make_unvalued_argument_parser(
                "z","zeta","Second middle action",
                [&order]{ order.append("zeta"); },
                0),
            make_unvalued_argument_parser(
                "a","alpha","First middle action",
                [&order]{ order.append("alpha"); },
                0),
            make_unvalued_argument_parser(
                "e","early","Early action",
                [&order]{ order.append("early"); },
                -10),
            make_valued_argument_parser(
                "v","value","Set a value",
                [&value](String const& argument) -> VoidFunction {
                    Int const parsed=std::stoi(argument);
                    return [&value,parsed] { value=static_cast<Nat>(parsed); };
                })
        }));

        ARIADNE_PROVE(ctx(), cli.acquire({"","--late","--zeta","--early","--alpha"}));
        ARIADNE_EXPECT_EQUAL(ctx(), order.size(), 4u);
        ARIADNE_EXPECT_EQUAL(ctx(), order[0], "early");
        ARIADNE_EXPECT_EQUAL(ctx(), order[1], "alpha");
        ARIADNE_EXPECT_EQUAL(ctx(), order[2], "zeta");
        ARIADNE_EXPECT_EQUAL(ctx(), order[3], "late");

        ARIADNE_PROVE(ctx(), not cli.acquire({"","--value","7","--unknown"}));
        ARIADNE_EXPECT_EQUAL(ctx(), value, 0u);
    }
};

int main() {
    Ariadne::Testing::Suite suite("command_line_interface");
    ARIADNE_ADD_TEST(suite, TestCommandLineInterface, test_argument_stream);
    ARIADNE_ADD_TEST(suite, TestCommandLineInterface, test_argument_pack);
    ARIADNE_ADD_TEST(suite, TestCommandLineInterface, test_parser_metadata_and_help);
    ARIADNE_ADD_TEST(suite, TestCommandLineInterface, test_base_interface);
    ARIADNE_ADD_TEST(suite, TestCommandLineInterface, test_composition);
    ARIADNE_ADD_TEST(suite, TestCommandLineInterface, test_validation);
    ARIADNE_ADD_TEST(suite, TestCommandLineInterface, test_priority_and_transactionality);
    Ariadne::Testing::ConsoleReporter reporter(std::cout);
    return suite.run(reporter).successful() ? 0 : 1;
}
