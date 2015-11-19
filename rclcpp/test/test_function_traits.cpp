// Copyright 2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include <rclcpp/function_traits.hpp>

int func_no_args()
{
  return 0;
}

// NOLINTNEXTLINE(readability/casting)
int func_one_int(int)
{
  return 1;
}

int func_two_ints(int, int)
{
  return 2;
}

int func_one_int_one_char(int, char)
{
  return 3;
}

struct FunctionObjectNoArgs
{
  int operator()() const
  {
    return 0;
  }
};

struct FunctionObjectOneInt
{
  int operator()(int) const
  {
    return 1;
  }
};

struct FunctionObjectTwoInts
{
  int operator()(int, int) const
  {
    return 2;
  }
};

struct FunctionObjectOneIntOneChar
{
  int operator()(int, char) const
  {
    return 3;
  }
};

template<
  typename FunctorT,
  std::size_t Arity = 0,
  typename std::enable_if<
    rclcpp::function_traits::arity_comparator<Arity, FunctorT>::value>::type * = nullptr
>
int func_accept_callback(FunctorT callback)
{
  return callback();
}

template<
  typename FunctorT,
  typename std::enable_if<
    rclcpp::function_traits::check_arguments<FunctorT, int>::value
  >::type * = nullptr
>
int func_accept_callback(FunctorT callback)
{
  int a = 4;
  return callback(a);
}

template<
  typename FunctorT,
  typename std::enable_if<
    rclcpp::function_traits::check_arguments<FunctorT, int, int>::value
  >::type * = nullptr
>
int func_accept_callback(FunctorT callback)
{
  int a = 5;
  int b = 6;
  return callback(a, b);
}

template<
  typename FunctorT,
  typename std::enable_if<
    rclcpp::function_traits::check_arguments<FunctorT, int, char>::value
  >::type * = nullptr
>
int func_accept_callback(FunctorT callback)
{
  int a = 7;
  char b = 8;
  return callback(a, b);
}

/*
   Tests that funcion_traits calculates arity of several functors.
 */
TEST(TestFunctionTraits, arity) {
  // Test regular functions
  static_assert(
    rclcpp::function_traits::function_traits<decltype(func_no_args)>::arity == 0,
    "Functor does not accept arguments");

  static_assert(
    rclcpp::function_traits::function_traits<decltype(func_one_int)>::arity == 1,
    "Functor only accepts one argument");

  static_assert(
    rclcpp::function_traits::function_traits<decltype(func_two_ints)>::arity == 2,
    "Functor only accepts two arguments");

  static_assert(
    rclcpp::function_traits::function_traits<decltype(func_one_int_one_char)>::arity == 2,
    "Functor only accepts two arguments");

  // Test lambdas
  auto lambda_no_args = []() {
      return 0;
    };

  auto lambda_one_int = [](int one) {
      (void)one;
      return 1;
    };

  auto lambda_two_ints = [](int one, int two) {
      (void)one;
      (void)two;
      return 2;
    };

  auto lambda_one_int_one_char = [](int one, char two) {
      (void)one;
      (void)two;
      return 3;
    };

  static_assert(
    rclcpp::function_traits::function_traits<decltype(lambda_no_args)>::arity == 0,
    "Functor does not accept arguments");

  static_assert(
    rclcpp::function_traits::function_traits<decltype(lambda_one_int)>::arity == 1,
    "Functor only accepts one argument");

  static_assert(
    rclcpp::function_traits::function_traits<decltype(lambda_two_ints)>::arity == 2,
    "Functor only accepts two arguments");

  static_assert(
    rclcpp::function_traits::function_traits<decltype(lambda_one_int_one_char)>::arity == 2,
    "Functor only accepts two arguments");

  // Test objects that have a call operator
  static_assert(
    rclcpp::function_traits::function_traits<FunctionObjectNoArgs>::arity == 0,
    "Functor does not accept arguments");

  static_assert(
    rclcpp::function_traits::function_traits<FunctionObjectOneInt>::arity == 1,
    "Functor only accepts one argument");

  static_assert(
    rclcpp::function_traits::function_traits<FunctionObjectTwoInts>::arity == 2,
    "Functor only accepts two arguments");

  static_assert(
    rclcpp::function_traits::function_traits<FunctionObjectOneIntOneChar>::arity == 2,
    "Functor only accepts two arguments");
}

/*
   Tests that funcion_traits deducts the type of the arguments of several functors.
 */
TEST(TestFunctionTraits, argument_types) {
  // Test regular functions
  static_assert(
    std::is_same<
      int,
      rclcpp::function_traits::function_traits<decltype(func_one_int)>::template argument_type<0>
    >::value, "Functor accepts an int as first argument");

  static_assert(
    std::is_same<
      int,
      rclcpp::function_traits::function_traits<decltype(func_two_ints)>::template argument_type<0>
    >::value, "Functor accepts an int as first argument");

  static_assert(
    std::is_same<
      int,
      rclcpp::function_traits::function_traits<decltype(func_two_ints)>::template argument_type<1>
    >::value, "Functor accepts an int as second argument");

  static_assert(
    std::is_same<
      int,
      rclcpp::function_traits::function_traits<
        decltype(func_one_int_one_char)
      >::template argument_type<0>
    >::value, "Functor accepts an int as first argument");

  static_assert(
    std::is_same<
      char,
      rclcpp::function_traits::function_traits<
        decltype(func_one_int_one_char)
      >::template argument_type<1>
    >::value, "Functor accepts a char as second argument");

  // Test lambdas
  auto lambda_one_int = [](int one) {
      (void)one;
      return 1;
    };

  auto lambda_two_ints = [](int one, int two) {
      (void)one;
      (void)two;
      return 2;
    };

  auto lambda_one_int_one_char = [](int one, char two) {
      (void)one;
      (void)two;
      return 3;
    };

  static_assert(
    std::is_same<
      int,
      rclcpp::function_traits::function_traits<decltype(lambda_one_int)>::template argument_type<0>
    >::value, "Functor accepts an int as first argument");

  static_assert(
    std::is_same<
      int,
      rclcpp::function_traits::function_traits<decltype(lambda_two_ints)>::template argument_type<0>
    >::value, "Functor accepts an int as first argument");

  static_assert(
    std::is_same<
      int,
      rclcpp::function_traits::function_traits<decltype(lambda_two_ints)>::template argument_type<1>
    >::value, "Functor accepts an int as second argument");

  static_assert(
    std::is_same<
      int,
      rclcpp::function_traits::function_traits<
        decltype(lambda_one_int_one_char)
      >::template argument_type<0>
    >::value, "Functor accepts an int as first argument");

  static_assert(
    std::is_same<
      char,
      rclcpp::function_traits::function_traits<
        decltype(lambda_one_int_one_char)
      >::template argument_type<1>
    >::value, "Functor accepts a char as second argument");

  // Test objects that have a call operator
  static_assert(
    std::is_same<
      int,
      rclcpp::function_traits::function_traits<FunctionObjectOneInt>::template argument_type<0>
    >::value, "Functor accepts an int as first argument");

  static_assert(
    std::is_same<
      int,
      rclcpp::function_traits::function_traits<FunctionObjectTwoInts>::template argument_type<0>
    >::value, "Functor accepts an int as first argument");

  static_assert(
    std::is_same<
      int,
      rclcpp::function_traits::function_traits<FunctionObjectTwoInts>::template argument_type<1>
    >::value, "Functor accepts an int as second argument");

  static_assert(
    std::is_same<
      int,
      rclcpp::function_traits::function_traits<
        FunctionObjectOneIntOneChar
      >::template argument_type<0>
    >::value, "Functor accepts an int as first argument");

  static_assert(
    std::is_same<
      char,
      rclcpp::function_traits::function_traits<
        FunctionObjectOneIntOneChar
      >::template argument_type<1>
    >::value, "Functor accepts a char as second argument");
}

/*
   Tests that funcion_traits checks the types of the arguments of several functors.
 */
TEST(TestFunctionTraits, check_arguments) {
  // Test regular functions
  static_assert(
    rclcpp::function_traits::check_arguments<decltype(func_one_int), int>::value,
    "Functor accepts a single int as arguments");

  static_assert(
    !rclcpp::function_traits::check_arguments<decltype(func_one_int), char>::value,
    "Functor does not accept a char as argument");

  static_assert(
    !rclcpp::function_traits::check_arguments<decltype(func_one_int), char, int>::value,
    "Functor does not accept two arguments");

  static_assert(
    !rclcpp::function_traits::check_arguments<decltype(func_two_ints), int>::value,
    "Functor does not accept a single int as argument, requires two ints");

  static_assert(
    rclcpp::function_traits::check_arguments<decltype(func_two_ints), int, int>::value,
    "Functor accepts two ints as arguments");

  static_assert(
    !rclcpp::function_traits::check_arguments<decltype(func_two_ints), bool, int>::value,
    "Functor does not accept a bool and an int as arguments, requires two ints");

  static_assert(
    !rclcpp::function_traits::check_arguments<decltype(func_two_ints), int, char>::value,
    "Functor does not accept an int and a char as arguments, requires two ints");

  static_assert(
    rclcpp::function_traits::check_arguments<decltype(func_one_int_one_char), int, char>::value,
    "Functor accepts an int and a char as arguments");

  // Test lambdas
  auto lambda_one_int = [](int one) {
      (void)one;
      return 1;
    };

  auto lambda_two_ints = [](int one, int two) {
      (void)one;
      (void)two;
      return 2;
    };

  auto lambda_one_int_one_char = [](int one, char two) {
      (void)one;
      (void)two;
      return 3;
    };

  static_assert(
    rclcpp::function_traits::check_arguments<decltype(lambda_one_int), int>::value,
    "Functor accepts an int as the only argument");

  static_assert(
    rclcpp::function_traits::check_arguments<decltype(lambda_two_ints), int, int>::value,
    "Functor accepts two ints as arguments");

  static_assert(
    rclcpp::function_traits::check_arguments<decltype(lambda_one_int_one_char), int, char>::value,
    "Functor accepts an int and a char as arguments");

  // Test objects that have a call operator
  static_assert(
    rclcpp::function_traits::check_arguments<FunctionObjectOneInt, int>::value,
    "Functor accepts an int as the only argument");

  static_assert(
    rclcpp::function_traits::check_arguments<FunctionObjectTwoInts, int, int>::value,
    "Functor accepts two ints as arguments");

  static_assert(
    rclcpp::function_traits::check_arguments<FunctionObjectOneIntOneChar, int, char>::value,
    "Functor accepts an int and a char as arguments");
}

/*
   Tests that same_arguments work.
*/
TEST(TestFunctionTraits, same_arguments) {
  auto lambda_one_int = [](int one) {
      (void)one;
      return 1;
    };

  auto lambda_two_ints = [](int one, int two) {
      (void)one;
      (void)two;
      return 1;
    };

  static_assert(
    rclcpp::function_traits::same_arguments<
      decltype(lambda_one_int), decltype(func_one_int)
    >::value,
    "Lambda and function have the same arguments");

  static_assert(
    !rclcpp::function_traits::same_arguments<
      decltype(lambda_two_ints), decltype(func_one_int)
    >::value,
    "Lambda and function have different arguments");

  static_assert(
    !rclcpp::function_traits::same_arguments<
      decltype(func_one_int_one_char), decltype(func_two_ints)
    >::value,
    "Functions have different arguments");

  static_assert(
    !rclcpp::function_traits::same_arguments<
      decltype(lambda_one_int), decltype(lambda_two_ints)
    >::value,
    "Lambdas have different arguments");

  static_assert(
    rclcpp::function_traits::same_arguments<FunctionObjectTwoInts, decltype(func_two_ints)>::value,
    "Functor and function have the same arguments");

  static_assert(
    rclcpp::function_traits::same_arguments<
      FunctionObjectTwoInts, decltype(lambda_two_ints)>::value,
    "Functor and lambda have the same arguments");
}

/*
   Tests that functions are matched via SFINAE.
 */
TEST(TestFunctionTraits, sfinae_match) {
  EXPECT_EQ(0, func_accept_callback(func_no_args));

  EXPECT_EQ(1, func_accept_callback(func_one_int));

  EXPECT_EQ(2, func_accept_callback(func_two_ints));

  EXPECT_EQ(3, func_accept_callback(func_one_int_one_char));

  auto lambda_no_args = []() {
      return 0;
    };

  auto lambda_one_int = [](int one) {
      (void)one;
      return 1;
    };

  auto lambda_two_ints = [](int one, int two) {
      (void)one;
      (void)two;
      return 2;
    };

  auto lambda_one_int_one_char = [](int one, char two) {
      (void)one;
      (void)two;
      return 3;
    };

  EXPECT_EQ(0, func_accept_callback(lambda_no_args));

  EXPECT_EQ(1, func_accept_callback(lambda_one_int));

  EXPECT_EQ(2, func_accept_callback(lambda_two_ints));

  EXPECT_EQ(3, func_accept_callback(lambda_one_int_one_char));

  EXPECT_EQ(0, func_accept_callback(FunctionObjectNoArgs()));

  EXPECT_EQ(1, func_accept_callback(FunctionObjectOneInt()));

  EXPECT_EQ(2, func_accept_callback(FunctionObjectTwoInts()));

  EXPECT_EQ(3, func_accept_callback(FunctionObjectOneIntOneChar()));
}
