/***************************************************************************
 *            test_command_line_interface.cpp
 *
 *  Copyright  2026  Luca Geretti
 *
 ****************************************************************************/

#include "utility/command_line_interface.hpp"
#include "testing/testing.hpp"
#include <iostream>

using namespace Ariadne;

class TestCommandLineInterface {
  public:
    Ariadne::Testing::Context* _ctx = nullptr;
    Ariadne::Testing::Context& ctx() { return *_ctx; }

    Void test_argument_stream() {
        ctx().expect_exception([&] { ArgumentStream({}); }, "ArgumentStream({})");

        ArgumentStream stream({"program","first"});
        ctx().prove((not stream.empty()), "not stream.empty()");
        ctx().expect_equal((stream.size()), (2u), "stream.size(),2u");
        ctx().expect_equal((stream.peek()), ("program"), "stream.peek(),\"program\"");
        ctx().expect_equal((stream.pop()), ("program"), "stream.pop(),\"program\"");
        ctx().expect_equal((stream.pop()), ("first"), "stream.pop(),\"first\"");
        ctx().prove((stream.empty()), "stream.empty()");
        ctx().expect_exception([&] { stream.peek(); }, "stream.peek()");
        ctx().expect_exception([&] { stream.pop(); }, "stream.pop()");
    }

    Void test_argument_pack() {
        Bool processed=false;
        ArgumentPack pack("test",[&processed]{ processed=true; },-3);
        ctx().expect_equal((pack.id()), ("test"), "pack.id(),\"test\"");
        ctx().expect_equal((pack.priority()), (-3), "pack.priority(),-3");
        pack.process();
        ctx().prove((processed), "processed");
    }

    Void test_parser_metadata_and_help() {
        ArgumentParser valued=make_valued_argument_parser(
            "v","value","Set a value",
            [](String const&) -> VoidFunction { return []{}; });
        ArgumentParser flag=make_unvalued_argument_parser(
            "","flag","Enable a flag",[]{});

        ctx().expect_equal((valued.short_id()), ("v"), "valued.short_id(),\"v\"");
        ctx().expect_equal((valued.long_id()), ("value"), "valued.long_id(),\"value\"");
        ctx().expect_equal((flag.short_id()), (""), "flag.short_id(),\"\"");
        ctx().expect_equal((flag.long_id()), ("flag"), "flag.long_id(),\"flag\"");
        ctx().prove((valued.help_description_header_size() > flag.help_description_header_size()), "valued.help_description_header_size() > flag.help_description_header_size()");
        ctx().prove((valued.help_description(2).find("<value>") != String::npos), "valued.help_description(2).find(\"<value>\") != String::npos");
        ctx().prove((flag.help_description(2).find("[--flag]") != String::npos), "flag.help_description(2).find(\"[--flag]\") != String::npos");

        ArgumentStream short_stream({"-v","1"});
        ArgumentStream long_stream({"--value","1"});
        ArgumentStream other_stream({"--other"});
        ArgumentStream long_only_stream({"--flag"});
        ctx().prove((valued.is_consumable(short_stream)), "valued.is_consumable(short_stream)");
        ctx().prove((valued.is_consumable(long_stream)), "valued.is_consumable(long_stream)");
        ctx().prove((not valued.is_consumable(other_stream)), "not valued.is_consumable(other_stream)");
        ctx().prove((flag.is_consumable(long_only_stream)), "flag.is_consumable(long_only_stream)");
    }

    Void test_base_interface() {
        CommandLineInterface cli;
        ctx().prove((cli.acquire({""})), "cli.acquire({\"\"})");
        ctx().prove((not cli.acquire({"","--unknown"})), "not cli.acquire({\"\",\"--unknown\"})");
        ctx().prove((not cli.acquire({"","-h"})), "not cli.acquire({\"\",\"-h\"})");
        ctx().prove((not cli.acquire({"","--help"})), "not cli.acquire({\"\",\"--help\"})");

        const char* argv[]={"program"};
        ctx().prove((cli.acquire(1,argv)), "cli.acquire(1,argv)");
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

        ctx().expect_equal((contribution.name()), ("test"), "contribution.name(),\"test\"");
        ctx().expect_equal((contribution.parsers().size()), (2u), "contribution.parsers().size(),2u");

        CommandLineInterface cli;
        cli.add(contribution).add(contribution);

        ctx().prove((cli.acquire({"","-v","3","--flag"})), "cli.acquire({\"\",\"-v\",\"3\",\"--flag\"})");
        ctx().expect_equal((value), (3u), "value,3u");
        ctx().prove((flag), "flag");

        value=0u;
        ctx().prove((cli.acquire({"","--value","4"})), "cli.acquire({\"\",\"--value\",\"4\"})");
        ctx().expect_equal((value), (4u), "value,4u");

        ctx().prove((not cli.acquire({"","--value"})), "not cli.acquire({\"\",\"--value\"})");
        ctx().prove((not cli.acquire({"","--value","-1"})), "not cli.acquire({\"\",\"--value\",\"-1\"})");
        ctx().prove((not cli.acquire({"","--value","x"})), "not cli.acquire({\"\",\"--value\",\"x\"})");
        ctx().prove((not cli.acquire({"","--value","1","-v","2"})), "not cli.acquire({\"\",\"--value\",\"1\",\"-v\",\"2\"})");
    }

    Void test_validation() {
        CommandLineInterface help_collision;
        ctx().expect_exception([&] { help_collision.add(
            CommandLineContribution("help-long",{
                make_unvalued_argument_parser("x","help","collision",[]{})
            })); }, "help_collision.add( CommandLineContribution(\"help-long\",{ make_unvalued_argument_parser(\"x\",\"help\",\"collision\",[]{}) }))");
        ctx().expect_exception([&] { help_collision.add(
            CommandLineContribution("help-short",{
                make_unvalued_argument_parser("h","other","collision",[]{})
            })); }, "help_collision.add( CommandLineContribution(\"help-short\",{ make_unvalued_argument_parser(\"h\",\"other\",\"collision\",[]{}) }))");

        CommandLineInterface internal_long_collision;
        ctx().expect_exception([&] { internal_long_collision.add(
            CommandLineContribution("long-collision",{
                make_unvalued_argument_parser("a","same","first",[]{}),
                make_unvalued_argument_parser("b","same","second",[]{})
            })); }, "internal_long_collision.add( CommandLineContribution(\"long-collision\",{ make_unvalued_argument_parser(\"a\",\"same\",\"first\",[]{}), make_unvalued_argument_parser(\"b\",\"same\",\"second\",[]{}) }))");

        CommandLineInterface internal_short_collision;
        ctx().expect_exception([&] { internal_short_collision.add(
            CommandLineContribution("short-collision",{
                make_unvalued_argument_parser("a","first","first",[]{}),
                make_unvalued_argument_parser("a","second","second",[]{})
            })); }, "internal_short_collision.add( CommandLineContribution(\"short-collision\",{ make_unvalued_argument_parser(\"a\",\"first\",\"first\",[]{}), make_unvalued_argument_parser(\"a\",\"second\",\"second\",[]{}) }))");

        CommandLineInterface existing_long_collision;
        existing_long_collision.add(CommandLineContribution("first",{
            make_unvalued_argument_parser("a","first","first",[]{})
        }));
        ctx().expect_exception([&] { existing_long_collision.add(
            CommandLineContribution("second",{
                make_unvalued_argument_parser("b","first","second",[]{})
            })); }, "existing_long_collision.add( CommandLineContribution(\"second\",{ make_unvalued_argument_parser(\"b\",\"first\",\"second\",[]{}) }))");

        CommandLineInterface existing_short_collision;
        existing_short_collision.add(CommandLineContribution("first",{
            make_unvalued_argument_parser("a","first","first",[]{})
        }));
        ctx().expect_exception([&] { existing_short_collision.add(
            CommandLineContribution("second",{
                make_unvalued_argument_parser("a","second","second",[]{})
            })); }, "existing_short_collision.add( CommandLineContribution(\"second\",{ make_unvalued_argument_parser(\"a\",\"second\",\"second\",[]{}) }))");

        CommandLineInterface long_only;
        ctx().expect_no_throw([&] { long_only.add(
            CommandLineContribution("long-only",{
                make_unvalued_argument_parser("x","first","first",[]{}),
                make_unvalued_argument_parser("","second","second",[]{})
            })); }, "long_only.add( CommandLineContribution(\"long-only\",{ make_unvalued_argument_parser(\"x\",\"first\",\"first\",[]{}), make_unvalued_argument_parser(\"\",\"second\",\"second\",[]{}) }))");
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

        ctx().prove((cli.acquire({"","--late","--zeta","--early","--alpha"})), "cli.acquire({\"\",\"--late\",\"--zeta\",\"--early\",\"--alpha\"})");
        ctx().expect_equal((order.size()), (4u), "order.size(),4u");
        ctx().expect_equal((order[0]), ("early"), "order[0],\"early\"");
        ctx().expect_equal((order[1]), ("alpha"), "order[1],\"alpha\"");
        ctx().expect_equal((order[2]), ("zeta"), "order[2],\"zeta\"");
        ctx().expect_equal((order[3]), ("late"), "order[3],\"late\"");

        ctx().prove((not cli.acquire({"","--value","7","--unknown"})), "not cli.acquire({\"\",\"--value\",\"7\",\"--unknown\"})");
        ctx().expect_equal((value), (0u), "value,0u");
    }
};

int main() {
    Ariadne::Testing::Suite suite("command_line_interface");
    suite.add("argument_stream", [&](Ariadne::Testing::Context& t) {
        TestCommandLineInterface fixture;
        fixture._ctx = &t;
        fixture.test_argument_stream();
    });
    suite.add("argument_pack", [&](Ariadne::Testing::Context& t) {
        TestCommandLineInterface fixture;
        fixture._ctx = &t;
        fixture.test_argument_pack();
    });
    suite.add("parser_metadata_and_help", [&](Ariadne::Testing::Context& t) {
        TestCommandLineInterface fixture;
        fixture._ctx = &t;
        fixture.test_parser_metadata_and_help();
    });
    suite.add("base_interface", [&](Ariadne::Testing::Context& t) {
        TestCommandLineInterface fixture;
        fixture._ctx = &t;
        fixture.test_base_interface();
    });
    suite.add("composition", [&](Ariadne::Testing::Context& t) {
        TestCommandLineInterface fixture;
        fixture._ctx = &t;
        fixture.test_composition();
    });
    suite.add("validation", [&](Ariadne::Testing::Context& t) {
        TestCommandLineInterface fixture;
        fixture._ctx = &t;
        fixture.test_validation();
    });
    suite.add("priority_and_transactionality", [&](Ariadne::Testing::Context& t) {
        TestCommandLineInterface fixture;
        fixture._ctx = &t;
        fixture.test_priority_and_transactionality();
    });
    Ariadne::Testing::ConsoleReporter reporter(std::cout);
    return suite.run(reporter).successful() ? 0 : 1;
}
