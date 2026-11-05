/***************************************************************************
 *            ariadne-utility.hpp
 *
 *  Umbrella header for the standalone Ariadne Utility library.
 *
 ****************************************************************************/

#ifndef ARIADNE_UTILITY_UMBRELLA_HPP
#define ARIADNE_UTILITY_UMBRELLA_HPP

// Utility has no public runtime submodule dependencies; Build is CMake
// infrastructure and Testing is used only by the test suite.
#include "utility/module.hpp"

#include "utility/attribute.hpp"
#include "utility/binary_word.hpp"
#include "utility/dispatching.hpp"
#include "utility/exceptions.hpp"
#include "utility/iterator.hpp"
#include "utility/lazy.hpp"
#include "utility/lru_cache.hpp"
#include "utility/path.hpp"
#include "utility/pointer.hpp"
#include "utility/prototype.hpp"
#include "utility/randomiser.hpp"
#include "utility/range.hpp"
#include "utility/stack_trace.hpp"
#include "utility/stopwatch.hpp"
#include "utility/uniform_array.hpp"
#include "utility/uniform_list.hpp"
#include "utility/variant.inl.hpp"

#endif // ARIADNE_UTILITY_UMBRELLA_HPP
