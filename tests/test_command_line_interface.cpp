/***************************************************************************
 *            test_command_line_interface.cpp
 *
 *  Copyright  2026  Luca Geretti
 *
 ****************************************************************************/

#include "utility/command_line_interface.hpp"
#include "macro/testing.hpp"

using namespace Ariadne;

class TestCommandLineInterface {
  public:
    Void test() {
        ARIADNE_TEST_CALL(test_argument_stream())
        ARIADNE_TEST_CALL(test_argument_pack())
        ARIADNE_TEST_CALL(test_parser_metadata_and_help())
        ARIADNE_TEST_CALL(test_base_interface())
        ARIADNE_TEST_CALL(test_composition())
        ARIADNE_TEST_CALL(test_validation())
        ARIADNE_TEST_CALL(test_priority_and_transactionality())
    }

    Void test_argument_stream() {
        ARIADNE_TEST_FAIL(ArgumentStream({}))

        ArgumentStream stream({"program","first"});
        ARIADNE_TEST_ASSERT(not stream.empty())
        ARIADNE_TEST_EQUALS(stream.size(),2u)
        ARIADNE_TEST_EQUALS(stream.peek(),"program")
        ARIADNE_TEST_EQUALS(stream.pop(),"program")
        ARIADNE_TEST_EQUALS(stream.pop(),"first")
        ARIADNE_TEST_ASSERT(stream.empty())
        ARIADNE_TEST_FAIL(stream.peek())
        ARIADNE_TEST_FAIL(stream.pop())
    }

    Void test_argument_pack() {
        Bool processed=false;
        ArgumentPack pack("test",[&processed]{ processed=true; },-3);
        ARIADNE_TEST_EQUALS(pack.id(),"test")
        ARIADNE_TEST_EQUALS(pack.priority(),-3)
        pack.process();
        ARIADNE_TEST_ASSERT(processed)
    }

    Void test_parser_metadata_and_help() {
        ArgumentParser valued=make_valued_argument_parser(
            "v","value","Set a value",
            [](String const&) -> VoidFunction { return []{}; });
        ArgumentParser flag=make_unvalued_argument_parser(
            "","flag","Enable a flag",[]{});

        ARIADNE_TEST_EQUALS(valued.short_id(),"v")
        ARIADNE_TEST_EQUALS(valued.long_id(),"value")
        ARIADNE_TEST_EQUALS(flag.short_id(),"")
        ARIADNE_TEST_EQUALS(flag.long_id(),"flag")
        ARIADNE_TEST_ASSERT(valued.help_description_header_size() > flag.help_description_header_size())
        ARIADNE_TEST_ASSERT(valued.help_description(2).find("<value>") != String::npos)
        ARIADNE_TEST_ASSERT(flag.help_description(2).find("[--flag]") != String::npos)

        ArgumentStream short_stream({"-v","1"});
        ArgumentStream long_stream({"--value","1"});
        ArgumentStream other_stream({"--other"});
        ARIADNE_TEST_ASSERT(valued.is_consumable(short_stream))
        ARIADNE_TEST_ASSERT(valued.is_consumable(long_stream))
        ARIADNE_TEST_ASSERT(not valued.is_consumable(other_stream))
    }

    Void test_base_interface() {
        CommandLineInterface cli;
        ARIADNE_TEST_ASSERT(cli.acquire({""}))
        ARIADNE_TEST_ASSERT(not cli.acquire({"","--unknown"}))
        ARIADNE_TEST_ASSERT(not cli.acquire({"","-h"}))
        ARIADNE_TEST_ASSERT(not cli.acquire({"","--help"}))

        const char* argv[]={"program"};
        ARIADNE_TEST_ASSERT(cli.acquire(1,argv))
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

        ARIADNE_TEST_EQUALS(contribution.name(),"test")
        ARIADNE_TEST_EQUALS(contribution.parsers().size(),2u)

        CommandLineInterface cli;
        cli.add(contribution).add(contribution);

        ARIADNE_TEST_ASSERT(cli.acquire({"","-v","3","--flag"}))
        ARIADNE_TEST_EQUALS(value,3u)
        ARIADNE_TEST_ASSERT(flag)

        value=0u;
        ARIADNE_TEST_ASSERT(cli.acquire({"","--value","4"}))
        ARIADNE_TEST_EQUALS(value,4u)

        ARIADNE_TEST_ASSERT(not cli.acquire({"","--value"}))
        ARIADNE_TEST_ASSERT(not cli.acquire({"","--value","-1"}))
        ARIADNE_TEST_ASSERT(not cli.acquire({"","--value","x"}))
        ARIADNE_TEST_ASSERT(not cli.acquire({"","--value","1","-v","2"}))
    }

    Void test_validation() {
        CommandLineInterface help_collision;
        ARIADNE_TEST_FAIL(help_collision.add(
            CommandLineContribution("help-long",{
                make_unvalued_argument_parser("x","help","collision",[]{})
            })))
        ARIADNE_TEST_FAIL(help_collision.add(
            CommandLineContribution("help-short",{
                make_unvalued_argument_parser("h","other","collision",[]{})
            })))

        CommandLineInterface internal_long_collision;
        ARIADNE_TEST_FAIL(internal_long_collision.add(
            CommandLineContribution("long-collision",{
                make_unvalued_argument_parser("a","same","first",[]{}),
                make_unvalued_argument_parser("b","same","second",[]{})
            })))

        CommandLineInterface internal_short_collision;
        ARIADNE_TEST_FAIL(internal_short_collision.add(
            CommandLineContribution("short-collision",{
                make_unvalued_argument_parser("a","first","first",[]{}),
                make_unvalued_argument_parser("a","second","second",[]{})
            })))

        CommandLineInterface existing_long_collision;
        existing_long_collision.add(CommandLineContribution("first",{
            make_unvalued_argument_parser("a","first","first",[]{})
        }));
        ARIADNE_TEST_FAIL(existing_long_collision.add(
            CommandLineContribution("second",{
                make_unvalued_argument_parser("b","first","second",[]{})
            })))

        CommandLineInterface existing_short_collision;
        existing_short_collision.add(CommandLineContribution("first",{
            make_unvalued_argument_parser("a","first","first",[]{})
        }));
        ARIADNE_TEST_FAIL(existing_short_collision.add(
            CommandLineContribution("second",{
                make_unvalued_argument_parser("a","second","second",[]{})
            })))
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

        ARIADNE_TEST_ASSERT(cli.acquire({"","--late","--zeta","--early","--alpha"}))
        ARIADNE_TEST_EQUALS(order.size(),4u)
        ARIADNE_TEST_EQUALS(order[0],"early")
        ARIADNE_TEST_EQUALS(order[1],"alpha")
        ARIADNE_TEST_EQUALS(order[2],"zeta")
        ARIADNE_TEST_EQUALS(order[3],"late")

        ARIADNE_TEST_ASSERT(not cli.acquire({"","--value","7","--unknown"}))
        ARIADNE_TEST_EQUALS(value,0u)
    }
};

Int main() {
    TestCommandLineInterface().test();
    return ARIADNE_TEST_FAILURES;
}
