/***************************************************************************
 *            test_command_line_interface.cpp
 *
 *  Copyright  2026  Luca Geretti
 *
 ****************************************************************************/

#include "utility/command_line_interface.hpp"
#include "testing/testing.hpp"

using namespace Ariadne;

class TestCommandLineInterface {
  public:
    Ariadne::Testing::Context* _ctx = nullptr;
    Ariadne::Testing::Context& ctx() { return *_ctx; }

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
