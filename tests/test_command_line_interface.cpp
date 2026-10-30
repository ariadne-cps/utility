/***************************************************************************
 *            test_command_line_interface.cpp
 *
 *  Copyright  2026  Luca Geretti
 *
 ****************************************************************************/

#include "utility/command_line_interface.hpp"
#include "bootstrap.hpp"

using namespace Ariadne;

class TestCommandLineInterface {
  public:
    Void test() {
        bootstrap::call([&] { test_argument_stream(); }, "test_argument_stream()")
        bootstrap::call([&] { test_argument_pack(); }, "test_argument_pack()")
        bootstrap::call([&] { test_parser_metadata_and_help(); }, "test_parser_metadata_and_help()")
        bootstrap::call([&] { test_base_interface(); }, "test_base_interface()")
        bootstrap::call([&] { test_composition(); }, "test_composition()")
        bootstrap::call([&] { test_validation(); }, "test_validation()")
        bootstrap::call([&] { test_priority_and_transactionality(); }, "test_priority_and_transactionality()")
    }

    Void test_argument_stream() {
        bootstrap::throws([&] { ArgumentStream({}); }, "ArgumentStream({})")

        ArgumentStream stream({"program","first"});
        bootstrap::require((not stream.empty()), "not stream.empty()")
        bootstrap::equal((stream.size()), (2u), "stream.size(),2u")
        bootstrap::equal((stream.peek()), ("program"), "stream.peek(),\"program\"")
        bootstrap::equal((stream.pop()), ("program"), "stream.pop(),\"program\"")
        bootstrap::equal((stream.pop()), ("first"), "stream.pop(),\"first\"")
        bootstrap::require((stream.empty()), "stream.empty()")
        bootstrap::throws([&] { stream.peek(); }, "stream.peek()")
        bootstrap::throws([&] { stream.pop(); }, "stream.pop()")
    }

    Void test_argument_pack() {
        Bool processed=false;
        ArgumentPack pack("test",[&processed]{ processed=true; },-3);
        bootstrap::equal((pack.id()), ("test"), "pack.id(),\"test\"")
        bootstrap::equal((pack.priority()), (-3), "pack.priority(),-3")
        pack.process();
        bootstrap::require((processed), "processed")
    }

    Void test_parser_metadata_and_help() {
        ArgumentParser valued=make_valued_argument_parser(
            "v","value","Set a value",
            [](String const&) -> VoidFunction { return []{}; });
        ArgumentParser flag=make_unvalued_argument_parser(
            "","flag","Enable a flag",[]{});

        bootstrap::equal((valued.short_id()), ("v"), "valued.short_id(),\"v\"")
        bootstrap::equal((valued.long_id()), ("value"), "valued.long_id(),\"value\"")
        bootstrap::equal((flag.short_id()), (""), "flag.short_id(),\"\"")
        bootstrap::equal((flag.long_id()), ("flag"), "flag.long_id(),\"flag\"")
        bootstrap::require((valued.help_description_header_size() > flag.help_description_header_size()), "valued.help_description_header_size() > flag.help_description_header_size()")
        bootstrap::require((valued.help_description(2).find("<value>") != String::npos), "valued.help_description(2).find(\"<value>\") != String::npos")
        bootstrap::require((flag.help_description(2).find("[--flag]") != String::npos), "flag.help_description(2).find(\"[--flag]\") != String::npos")

        ArgumentStream short_stream({"-v","1"});
        ArgumentStream long_stream({"--value","1"});
        ArgumentStream other_stream({"--other"});
        ArgumentStream long_only_stream({"--flag"});
        bootstrap::require((valued.is_consumable(short_stream)), "valued.is_consumable(short_stream)")
        bootstrap::require((valued.is_consumable(long_stream)), "valued.is_consumable(long_stream)")
        bootstrap::require((not valued.is_consumable(other_stream)), "not valued.is_consumable(other_stream)")
        bootstrap::require((flag.is_consumable(long_only_stream)), "flag.is_consumable(long_only_stream)")
    }

    Void test_base_interface() {
        CommandLineInterface cli;
        bootstrap::require((cli.acquire({""})), "cli.acquire({\"\"})")
        bootstrap::require((not cli.acquire({"","--unknown"})), "not cli.acquire({\"\",\"--unknown\"})")
        bootstrap::require((not cli.acquire({"","-h"})), "not cli.acquire({\"\",\"-h\"})")
        bootstrap::require((not cli.acquire({"","--help"})), "not cli.acquire({\"\",\"--help\"})")

        const char* argv[]={"program"};
        bootstrap::require((cli.acquire(1,argv)), "cli.acquire(1,argv)")
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

        bootstrap::equal((contribution.name()), ("test"), "contribution.name(),\"test\"")
        bootstrap::equal((contribution.parsers().size()), (2u), "contribution.parsers().size(),2u")

        CommandLineInterface cli;
        cli.add(contribution).add(contribution);

        bootstrap::require((cli.acquire({"","-v","3","--flag"})), "cli.acquire({\"\",\"-v\",\"3\",\"--flag\"})")
        bootstrap::equal((value), (3u), "value,3u")
        bootstrap::require((flag), "flag")

        value=0u;
        bootstrap::require((cli.acquire({"","--value","4"})), "cli.acquire({\"\",\"--value\",\"4\"})")
        bootstrap::equal((value), (4u), "value,4u")

        bootstrap::require((not cli.acquire({"","--value"})), "not cli.acquire({\"\",\"--value\"})")
        bootstrap::require((not cli.acquire({"","--value","-1"})), "not cli.acquire({\"\",\"--value\",\"-1\"})")
        bootstrap::require((not cli.acquire({"","--value","x"})), "not cli.acquire({\"\",\"--value\",\"x\"})")
        bootstrap::require((not cli.acquire({"","--value","1","-v","2"})), "not cli.acquire({\"\",\"--value\",\"1\",\"-v\",\"2\"})")
    }

    Void test_validation() {
        CommandLineInterface help_collision;
        bootstrap::throws([&] { help_collision.add(
            CommandLineContribution("help-long",{
                make_unvalued_argument_parser("x","help","collision",[]{})
            })); }, "help_collision.add( CommandLineContribution(\"help-long\",{ make_unvalued_argument_parser(\"x\",\"help\",\"collision\",[]{}) }))")
        bootstrap::throws([&] { help_collision.add(
            CommandLineContribution("help-short",{
                make_unvalued_argument_parser("h","other","collision",[]{})
            })); }, "help_collision.add( CommandLineContribution(\"help-short\",{ make_unvalued_argument_parser(\"h\",\"other\",\"collision\",[]{}) }))")

        CommandLineInterface internal_long_collision;
        bootstrap::throws([&] { internal_long_collision.add(
            CommandLineContribution("long-collision",{
                make_unvalued_argument_parser("a","same","first",[]{}),
                make_unvalued_argument_parser("b","same","second",[]{})
            })); }, "internal_long_collision.add( CommandLineContribution(\"long-collision\",{ make_unvalued_argument_parser(\"a\",\"same\",\"first\",[]{}), make_unvalued_argument_parser(\"b")

        CommandLineInterface internal_short_collision;
        bootstrap::throws([&] { internal_short_collision.add(
            CommandLineContribution("short-collision",{
                make_unvalued_argument_parser("a","first","first",[]{}),
                make_unvalued_argument_parser("a","second","second",[]{})
            })); }, "internal_short_collision.add( CommandLineContribution(\"short-collision\",{ make_unvalued_argument_parser(\"a\",\"first\",\"first\",[]{}), make_unvalued_argument_parser")

        CommandLineInterface existing_long_collision;
        existing_long_collision.add(CommandLineContribution("first",{
            make_unvalued_argument_parser("a","first","first",[]{})
        }));
        bootstrap::throws([&] { existing_long_collision.add(
            CommandLineContribution("second",{
                make_unvalued_argument_parser("b","first","second",[]{})
            })); }, "existing_long_collision.add( CommandLineContribution(\"second\",{ make_unvalued_argument_parser(\"b\",\"first\",\"second\",[]{}) }))")

        CommandLineInterface existing_short_collision;
        existing_short_collision.add(CommandLineContribution("first",{
            make_unvalued_argument_parser("a","first","first",[]{})
        }));
        bootstrap::throws([&] { existing_short_collision.add(
            CommandLineContribution("second",{
                make_unvalued_argument_parser("a","second","second",[]{})
            })); }, "existing_short_collision.add( CommandLineContribution(\"second\",{ make_unvalued_argument_parser(\"a\",\"second\",\"second\",[]{}) }))")

        CommandLineInterface long_only;
        bootstrap::call([&] { long_only.add(
            CommandLineContribution("long-only",{
                make_unvalued_argument_parser("x","first","first",[]{}),
                make_unvalued_argument_parser("","second","second",[]{})
            })); }, "long_only.add( CommandLineContribution(\"long-only\",{ make_unvalued_argument_parser(\"x\",\"first\",\"first\",[]{}), make_unvalued_argument_parser(\"\",\"second\",\"second\"")
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

        bootstrap::require((cli.acquire({"","--late","--zeta","--early","--alpha"})), "cli.acquire({\"\",\"--late\",\"--zeta\",\"--early\",\"--alpha\"})")
        bootstrap::equal((order.size()), (4u), "order.size(),4u")
        bootstrap::equal((order[0]), ("early"), "order[0],\"early\"")
        bootstrap::equal((order[1]), ("alpha"), "order[1],\"alpha\"")
        bootstrap::equal((order[2]), ("zeta"), "order[2],\"zeta\"")
        bootstrap::equal((order[3]), ("late"), "order[3],\"late\"")

        bootstrap::require((not cli.acquire({"","--value","7","--unknown"})), "not cli.acquire({\"\",\"--value\",\"7\",\"--unknown\"})")
        bootstrap::equal((value), (0u), "value,0u")
    }
};

Int main() {
    TestCommandLineInterface().test();
    return bootstrap::failures;
}
