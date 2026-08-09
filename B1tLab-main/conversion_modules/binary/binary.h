#pragma once

#define BINARY_10_29_2025

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "built_ins.h"

namespace BinaryConversion {

constexpr inline uint16_t f_truncation{50};
constexpr inline uint16_t f_wrap_around{51};

constexpr inline bool dec_to_bin{0};
constexpr inline bool bin_to_dec{1};

// Standard formats:
// INT formats are ones that work with integer like inputs, FRACs only support
// fractions. Using an int input on a frac format or a frac input on an int
// format will result in the loss of data warning Note! There is an implicit
// format UNSIGNED, which is summoned only via the u or U prefix after the
// number
const inline std::vector<std::string_view> int_formats{
    "UNSIGNED", "SIGN_MAG", "ONES_COMP", "TWOS_COMP"};
const inline std::vector<std::string_view> conversion_commmands{
    "DEC_TO_BIN", "BIN_TO_DEC", "SDEC_TO_BIN", "SBIN_TO_DEC"};

// Struct used for handling binary strings (especially useful in namespace
// FORMAT)
struct BinStringParser {

  std::string value{};
  std::string intermediate_value{};
  uint16_t flag{};
};

namespace Utils {

// Converts the absolute value of whole part of a number (e.g. -54.3 =>  54 ) to
// a mathematically accurate binary representation
std::string intDecToBin(uint64_t integer_part);

// Converts the absolute value of fractional that is less than one (e.g. 1/3,
// 1/5, 5/5) to a mathematically accurate binary sequence
std::string fracDecToBin(uint64_t numerator, uint64_t denominator,
                         uint16_t precision);

// Converts the absolute value of whole part of a number (e.g. -0011101.010101
// => 0011101) to a mathematically accurate deciaml representation
BuiltIns::IntParser intBinToDec(std::string integer_part);

// This function converts binary input that starts from a given digit place to a
// proper decimal representation
BuiltIns::IntParser fracBinToDec(std::string fractional_part);

// Converts the absolute value of fractional part of a number (e.g.
// -0011101.010101 => 010101) to a mathematically accurate decimal
// representation (NOTE! Does't support ... e.g. 0.10101011010... will cause an
// error) This function utilizes fractionCalculator() in order to calculate
// periodic digits
BuiltIns::IntParser fracBinToDecPeriod(std::string digits_in_period,
                                       uint64_t start_index);

// Flips binary digits (0 -> 1, 1 -> 0)
std::string binaryFlip(std::string input);

// Adds two absolute binary inputs
std::string binAdd(std::string operand1, std::string operand2);

// Returns true if input is raw binary, otherwise returns false
bool isRawBin(std::string binary_input, std::vector<uint16_t> allowed_bit_size);

bool isAFlag(std::string input, std::vector<std::string_view> allowed_flags);
} // namespace Utils

namespace Diagnostics {

void intermediateValue(std::string value);

void wrapAroundWarning(uint16_t bit_size);

void truncationWarning(uint16_t bit_size);

void precisionLossWarning();

void setFormatMessage(std::string FORMAT, bool conversion_type,
                      bool is_a_fraction);

void setDefaultMessage(std::vector<std::string> default_formats);

void showLocal(std::vector<std::string> formats, bool conversion_type);
} // namespace Diagnostics

namespace DefaultConversion {
/*This is mathematically accurate conversion, where sign is preserved as a
separate character (e.g. '+', '-'), number representation is adaptive until the
limit value is reached, if a rational number has an inf sequence, this sequence
will be placed in proper period notation (e.g. 0.1 dec->bin = 0.000(1100) ), if
there is an irrational number it will be written until the 64-bit limit is
reached, and then noted with three dots at the end (e.g. pi
= 11.0010010000111111011...), also trailing zeros for fractions and leading
zeros for integers will be automatically ignored.
*/
std::string decToBin(std::vector<std::string> &input_vector);
std::string binToDec(std::vector<std::string> &input_vector);

} // namespace DefaultConversion

namespace StandardConversion {
/*This is hardware accurate conversion, where sign is preserved  based on the
format, the size is specified via the proper flag (e.g. --resize_to_16 or
--resize_to_double) Default formats are TWOS_COMP for integers and
IEEE754_BINARY for fractions.
*/

/*
Specific modes for the STANDARD_CONVERSION family.
The converters support two primare modes: Value-based and Format-based.
Value-based works like in any ordinary converter where you pass value and
opional resize flag as an argument Format-based has 5 sub-types:
--set_global sets choosen format for both SDEC_TO_BIN and SBIN_TO_DEC
--set_local sets choosen format explicitly for given converter (e.g. SDEC_TO_BIN
--set_local FORMAT will only affect SDEC_TO_BIN)
--set_default resets formats for the whole STANDARD_CONVERSION family to default
ones (TWOS_COMP, IEEE754_BINARY)
--show_global shows all of the currently active formats for the
STANDARD_CONVERISON family
--show_local shows all of the currently active formats for the specific
converter

*/
const inline int set_global{10};
const inline int set_local{11};
const inline int set_default{12};
const inline int show_global{13};
const inline int show_local{14};
const inline int value_mode{15};

const inline int int_format{
    20}; // All formats that can only be used on integer inputs
const inline int frac_format{
    21}; // All formats that are primarly used for fractional values

const inline std::vector<std::string_view> flag_mode{
    "--set_global", "--set_local", "--set_default", "--show_global",
    "--show_local"};

// By checking the spelling of the input, returns two modes: Default (Value,
// flag), Format Configurable
uint16_t modeSelector(std::string argument);

// First element is for INT type formats, second for FRAC type
// default_formats is of type std::string and not std::string_view because it
// works mostly with local variables
inline std::vector<std::string> default_formats{
    static_cast<std::string>(int_formats[3]), ""};
inline std::vector<std::string> sdec_to_bin_formats{default_formats};
inline std::vector<std::string> sbin_to_dec_formats{default_formats};

std::string standardConverter(std::vector<std::string> &input,
                              bool conversion_type);

} // namespace StandardConversion

namespace Format {

/*This namespace has all format-related functions includin converters, type
   deducers, etc.

*/

// Specific Format-calling function
BinStringParser formatOutputer(bool conversion_type, BuiltIns::Value input,
                               uint16_t bit_size, std::string_view FORMAT);

// Deduces the type of the input_format.  1 -> INT format, 2 -> FRAC format, 0
// -> Error state
uint16_t formatType(std::string_view input_format);

std::string formatConverter(bool conversion_type, std::string integer_value,
                            std::string resize_flag, std::string_view FORMAT);

const inline std::vector<std::string_view> frac_formats{};

namespace Int {

const inline std::vector<uint16_t> bit_size_presets{8, 16, 32, 64};
const inline std::vector<std::string_view> resize_flags{
    "--resize_to_8", "--resize_to_16", "--resize_to_32", "--resize_to_64"};

// Fully Resolves bit resizeding, providing appropriate flags
BinStringParser bitResizer(std::string input, bool account_for_sign_bit,
                           uint16_t bit_size);

// resizes a binary string to the least compatible length
BinStringParser autoResize(std::string input);

// resizes a binary string to the manually selected length
BinStringParser resizeTo(std::string input, uint16_t bit_width);

// Converts a binary mvalue into rvalue
BinStringParser binToRaw(BuiltIns::Value input, uint16_t bit_size,
                         std::string FORMAT);

// Returns the bit size value if the proper flag is provided, otherwise returns
// F_OPERAND_INVALID_INP, as an error state
uint16_t resizeFlagParser(std::string flag);

namespace Unsigned {

// Mimics the wrap-around logic. Note! funciton parameter is the absolute value
// of the negative input.
uint64_t wrapAround(uint64_t negative_input, uint16_t bit_size);

BinStringParser decToRaw(BuiltIns::Value input, uint16_t resize_flag,
                         bool display_IR = true);

BinStringParser rawToDec(BuiltIns::Value input, uint16_t bit_size);

} // namespace Unsigned

namespace SignMagnitude {

BinStringParser decToRaw(BuiltIns::Value input, uint16_t resize_flag,
                         bool display_IR = true);

BinStringParser rawToDec(BuiltIns::Value input, uint16_t bit_size);

} // namespace SignMagnitude

namespace OnesComplement {

BinStringParser decToRaw(BuiltIns::Value input, uint16_t bit_size,
                         bool display_IR = true);

BinStringParser rawToDec(BuiltIns::Value input, uint16_t bit_size);

} // namespace OnesComplement

namespace TwosComplement {

BinStringParser decToRaw(BuiltIns::Value input, uint16_t bit_size,
                         bool display_IR = true);

BinStringParser rawToDec(BuiltIns::Value input, uint16_t bit_size);
} // namespace TwosComplement

} // namespace Int

} // namespace Format

} // namespace BinaryConversion
