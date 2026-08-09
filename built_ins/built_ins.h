#pragma once

#define BUILT_INS_5_8_2026

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace BuiltIns {
// NOTE! None using special characaters for function arguments will result in
// wrong or even undefined behavior

// Fundamental Settings (DO NOT CHANGE!)
constexpr inline int kMaxBitValue{64};
constexpr inline int kMaxMantissa{16};

/*NOTE!Any number system that will be implemented should be represented
in a growing manner where each next digit should be greater than the previous
but less than the next (e.g. 0,1,2,3...). Not only that, but their ASCII
representations should also be arranged in the same code point growing order!
*/
inline std::string kDecimalSys{"0123456789"};
inline std::string kBinarySys{"01"};

inline std::string kMaxDec{"18446744073709551615"};
inline std::string kMaxBin{
    "1111111111111111111111111111111111111111111111111111111111111111"};

// Event flags
constexpr inline uint16_t f_invalid_input{1};
constexpr inline uint16_t f_out_of_bounds{5};
constexpr inline uint16_t f_mantissa_overflow{6};
constexpr inline uint16_t f_precision_loss{7};
constexpr inline uint16_t f_exit{8};

// All default keywords
const inline std::vector<std::string_view> commands{"exit"};

// struct for handling command inputs by user (e.g. 'SDEC_TO_BIN 54
// --resize_to_16')
struct UserInput {

  uint16_t flag{};

  // Needed mainly for diagnostics
  std::string unparsed_input{};
  std::vector<std::string> tokens{};
  std::string invalid_token{};
};

// The main  struct for user argument parsing
struct Value {

  // Whole part of input
  std::string whole_part{};

  // Fractional part of input
  std::string frac_part{};

  std::string frac_nonperiod_part{}; // Digits outside of the periodic notation
  std::string frac_period_part{};    // Digits inside the periodic notation
  bool is_a_frac{false};

  bool sign{0};
  // Useful for handling inputs like +5, where the '+' sign is written
  // explicitly
  bool has_explicit_plus{0};

  uint16_t flag{0};
};

// The main output struct for internal conversion integer-based functions
struct IntParser {

  bool sign{0};

  uint64_t numerator{};
  // To prevent division by zero issues
  uint64_t denominator{1};
  uint16_t flag{};
};

// Power function with unsigned integer only support
uint64_t power(uint64_t base, uint64_t exponent);

// String based division function, that doesn't utilize floating point
// arithmetics.
Value decimalDivision(uint64_t numerator, uint64_t denominator,
                      uint16_t precision);

namespace InpHandling {

// Initial parsing of the whole input before CommandCheck
UserInput getParsedInput(UserInput input);

std::string removeComments(std::string_view unparsed_string);

// Splits an input string into a vector containing tokens.
std::vector<std::string> tokenizer(std::string unparsed_string,
                                   std::vector<std::string> vector);

// Gets the first token from the input, returns it and deletes it from teh
// input_vector
std::string getFirstToken(std::vector<std::string> &input_vector);

// By selecting the number for the argument we fetch a specific word out of the
// input line. If argument exceeds the amount of words in an input_line, the
// function returns an empty string.
std::string inputFetch(std::string input_line, uint16_t argument);

namespace InpEdit {

// Converts a wide character into a '?' in order to prevent crashes
void inputFilter(std::string &input, int &cursor);

// Removes literal suffixes from a stirng
std::string removeSuffix(std::string input);

// Removes all empty spaces from a string.
std::string spaceRemoval(std::string input);

// Removes all trailing spaces from a string
std::string trailingSpaceRemoval(std::string input);

// Removes all newlines
std::string replaceNewline(std::string input);

// Strips all leading '0' characters from the input string. If the input string
// is empty, or if all characters are removed during this process, the function
// ensures the result is "0".
std::string leadingZeroRemoval(std::string input);

// Strips all trailing '0' characters from the input string. If the input string
// is empty, or if all characters are removed during this process, the function
// ensures the result is "0".
std::string trailingZeroRemoval(std::string input);

// Appends the sign before the first character of the input string, based on
// whether the bool sign is 0 or 1.
std::string signAppender(std::string input, bool sign);

// Removes the sign of input, so (+5 becomes 5, 5 stays 5, -5 becomes 5 )
std::string signRemoval(std::string input);

// Removes the brackets from input (e.g. 54665(54) becomes 5466554 )
std::string periodNotationRemoval(std::string input);

// Removes the decimal separator from the input
std::string dotRemoval(std::string input);

// Expands the digits in period into their full form (e.g. input 53 becomes
// 5353535353...)
std::string periodExpander(std::string input, uint64_t precision);

// Separates the input to an integer part and fractional part
Value decimalSeparator(std::string input);

// Sorts the fractional part of a number in either non-period notation or period
// notation (e.g. if fractional part is 369(555), the non-period notation will
// store 369, whereas period will store 555 )
Value periodSorter(std::string fractinoal_part);

} // namespace InpEdit

namespace InpCheck {

// Checks whether if the input is signed or unsigned (checks the literal suffix)
bool isUnsigned(std::string input);

// Checks whether input is positive or negative ('+' = 0, '-' = 1)
bool isNegative(std::string input, bool include_neg_zero = false);

// Checks whether an input is an appropriate number for a given number system
bool isNumber(std::string input, std::string num_sys);

// Compares the absolute values of two whole inputs
bool isAbsGreater(std::string input, std::string threshold);

// Checks whether does the input exceed the threshold or no. Supports only whole
// numbers Both for the input and for the max_num
bool isGreaterThan(std::string input, std::string max_num);

// Compares whether are two strings completely identical
bool isEqual(std::string input, std::string threshold);

// Checks whether is the input smaller than the threshold
bool isLessThan(std::string input, std::string threshold);

// Checks whether does an input contain floating point or not
bool isFraction(std::string input);

// Checks whether does a fractional input contain period or no
bool isPeriodic(std::string input);

// Counts the number of leading zero digits in a given number
uint64_t leadingZeroCounter(std::string input);
} // namespace InpCheck

// The parser of user input that checks the inputed number for its validity,
// whether does it exceed limits, whats the sign of it and stores info about the
// number in properly
Value valueParser(std::string input, std::string num_sys,
                  std::string min_int_num, std::string max_int_num,
                  uint64_t max_frac_digit_amount = kMaxMantissa,
                  bool remove_leading_zeroes = true,
                  bool remove_trailing_zeros = true,
                  bool include_neg_zero = false);

// Converts any string based absolute whole input into an integer.
IntParser toDec(std::string input);

// Converts any string based fractional part to an ordinary fraction. (e.g. (3)
// -> 3/9)
IntParser toRational(std::string fractional_input);
} // namespace InpHandling

namespace Diagnostics {

// General purpose error handler that will call a specific error based on the
// flag
bool errorHandler(std::string input, uint16_t flag, std::string min_num,
                  std::string max_num);

// Any invalid input for a given converter function will result in this error
void conversionError(std::string input);

// Any invalid command written in the console will result in this error
void invalidCommandError(std::string input);

// Any input that is greater or less than the value that the given function can
// support will result in this error
void overflowError(std::string min_num, std::string max_num);

// Any input whose amount of fractional digits exceeds the given function's
// limit will result in this error
void mantissaOverflowError(std::string max_frac_digit);

} // namespace Diagnostics

} // namespace BuiltIns
