/***************************************************************************
 *            utility/randomiser.hpp
 *
 *  Copyright  2011-20  Luca Geretti
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

/** @file utility/randomiser.hpp
 *  @brief Generators of random numbers for a type.
 *  \details The values are generated uniformly in the provided interval.
 */

#ifndef ARIADNE_UTILITY_RANDOMISER_HPP
#define ARIADNE_UTILITY_RANDOMISER_HPP

#include <chrono>
#include <random>

namespace Ariadne {

/** @brief Owner of the random-number engine shared by randomisers. */
class RandomGenerator {
  public:
    /** @brief Construct a \c RandomGenerator with the specified characteristics. */
    RandomGenerator() {
        std::random_device rd;
        std::mt19937::result_type seed = rd() ^ ((std::mt19937::result_type)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() +
                                                 (std::mt19937::result_type)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
        _engine = std::mt19937(seed);
    }

    /** @brief Return the random-number engine. */
    std::mt19937& engine() { return _engine; };

  private:
    std::mt19937 _engine;
};

static RandomGenerator RANDOM_GENERATOR;

/** @brief Polymorphic interface for randomiser. */
template<class T>
class RandomiserInterface {
  public:
    /** @brief Return the requested element or stored value. */
    virtual T get() = 0;
};

/** @brief Common engine access for random-number distributions. */
template<class T, class D>
class RandomiserBase : public RandomiserInterface<T> {
  protected:
    /** @brief Construct a \c RandomiserBase from min, max. */
    RandomiserBase(T min, T max) : _distribution(D(min, max)) {}
    D _distribution;

  public:
    /** @brief Return the requested element or stored value. */
    T get() override { return this->_distribution(RANDOM_GENERATOR.engine()); }
};

/** @brief Uniform distribution over a real-valued interval. */
template<class T>
struct UniformRealRandomiser : public RandomiserBase<T, std::uniform_real_distribution<T>> {
  public:
    /** @brief Construct a \c UniformRealRandomiser from min, max. */
    UniformRealRandomiser(T min, T max) : RandomiserBase<T, std::uniform_real_distribution<T>>(min, max) {}
};

/** @brief Uniform distribution over an integer-valued interval. */
template<class T>
struct UniformIntRandomiser : public RandomiserBase<T, std::uniform_int_distribution<T>> {
  public:
    /** @brief Construct a \c UniformIntRandomiser from min, max. */
    UniformIntRandomiser(T min, T max) : RandomiserBase<T, std::uniform_int_distribution<T>>(min, max) {}
};

} // namespace Ariadne

#endif /* ARIADNE_UTILITY_RANDOMISER_HPP */
