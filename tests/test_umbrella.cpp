// Compile and exercise representative Utility APIs through the public umbrella.
#include "ariadne-utility.hpp"

#include <cstdlib>

int main() {
    Ariadne::Array<int> values{1, 2, 3};
    Ariadne::Stopwatch<Ariadne::Milliseconds> timer;
    timer.click();
    return values.size() == 3u && timer.duration().count() >= 0
        ? EXIT_SUCCESS : EXIT_FAILURE;
}
