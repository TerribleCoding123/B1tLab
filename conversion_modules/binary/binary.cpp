//FIX ERROR LOGIC IN STANDARD_CONVERSION
#include <iostream>
#include <string>
#include <string_view> 
#include <cstdint>
#include <vector> //For period notation


#include "binary.h"
#include "built_ins.h"
#include "view.h"

namespace BINARY_CONVERSION {
	
	namespace UTILS {

		std::string intDecToBin(uint64_t integer_part) {
			//The conversion is done by constantly dividing the integer_part by 2, assigning the zero index  of bin_output
			//by whatever value the remainder will have after the division (either 1 or 0 because we divide by 2),
			//and repeating this process yet again, but with the dividend of the previous division. 


			uint64_t remainder{ integer_part };

			std::string bin_output{};

			if (remainder == 0) {
				
				bin_output = "0";
			}

			while (remainder >= 1) {

			
				bin_output.insert(0, std::to_string(remainder % 2));
				
				remainder /= 2;
			}

			
			return bin_output;
		}	
		
		std::string fracDecToBin(uint64_t numerator, uint64_t denominator, uint16_t precision) {
			/* In order to avoid any floating point errors in multiplication, we multiply integers! How this works:
				* in an ordinary decimal fraction to binary conversion, we perform multiplication on fractions, like this:
				* 0.625 x 2 = 1.25 -> 1 (subtract one); 0.25 x 2 = 0.5 -> 10; 0.5 x 2 = 1.0 -> 101. Making it work with floating points
				* may result in rounding error creeping too far in large numbers during multiplication, hence distorting the output.
				* In order to fix that we represent a fraction as an integer numerator and denominator, and perform any
				* arithmetic operations on them separately, so both the numerator and the denominator will always keep their integer values.
				*
				* By constantly multiplying the numerator by two,
				* we can compare whether has it become greater than the denominator or no. If after the multiplication it is still less than
				* the denominator, output is appended with 0, and numerator is kept intact. However, if after the multiplication the numerator
				* became greater than the denominator, output is appended with 1, numerator is subtracted with the denominator's value
				* (equivalent to the fraction being subtracted with one), and the whole process is repeated yet again until either the numerator
				* becomes equal to the denominator (in this scenario we terminate calculations and append our output with 1), or the numerator
				* repeats itself (in this scenario, output digits, that correspond to the repeating numerator values, are enclosed with round
				* braces, indicating the period).
			*/

			std::string output{};

			//value_buffer is needed to properly handle repeating binary fractions. It stores all 
			//of the previos values of the numerator and compares them to the current value of the numerator
			//if they are equal, then the number is periodic in binary.
			std::vector<uint64_t> value_buffer{};

			//We only need the remainder, (so we drop the whole value)
			numerator = numerator % denominator;

			//Early return. If we input 0 in the numerator, the program will output .(0), which is fine, but not optimal
			if (numerator == 0) {
				
				output = "0";
				return output;
			}

			//Recursive loop prevention
			if (denominator == 0) {

				return "";
			}
			

			while (numerator != denominator) {

				if (output.length() >= precision) {

					output.append("...");
					return output;
					
				}

				//The push_back statement must be before the numerator *= 2; statement, to 
				//ensure that we record  the previos value of the numerator and not the current one.
				value_buffer.push_back(numerator);
				
				numerator *= 2;

				if (numerator < denominator) {
					
					output += '0';
				}

				else if (numerator > denominator) {
					
					output += '1';
					numerator -= denominator;
				}

				//Here, all of the values in the buffer are checked for a match with a current numerator
				for (uint64_t index{ 0 }; index < value_buffer.size(); index++) {
				
					if (value_buffer[index] == numerator) {
						
						output.insert(index, 1, '(');
						output += ')';

						return output;
					}
				}

			}

			//When the loop ends naturally (numerator != denominator becomes false), we append a missing '1'
			output += '1';
			
			

			return output;
		}

		BUILT_INS::int_parser intBinToDec(std::string integer_part) {
			//The conversion is done by summing up powers of two multiplied by either 1 or 0 
			//So input like 101 will be 1 * 2^2 (corresponding to most significand digit) + 0 * 2^1 + 1 * 2^0
			

			BUILT_INS::int_parser output;
			
			if (integer_part.length() == 0) {

				output.flag = F_INVALID_INP;
				return output;
			}

			uint64_t bit_size{ integer_part.length() };
			
			//Actual conversion
			for (char i : integer_part) {

				bit_size--;

				if (i == '1') {

					output.numerator += BUILT_INS::Power(2, bit_size);
				}

				else if (i != '0') {
					
					
					output = BUILT_INS::int_parser{};
					output.flag = F_INVALID_INP;
					return output;
				}

		
			}

			return output;
		
		}
		
		BUILT_INS::int_parser fracBinToDec(std::string fractional_part) {

			/*
			* In order to avoid ANY floating point rounding, we use only integer arithmetics!
			* Consider the fractional input 1101 (e.g. 0.1101) in order to convert this input into a decimally representable
			* fraction we should use floating point arithmetics: 1*2^(-1) + 1*2^(-2) + 0*2^(-3) + 1*2^(-4). This
			* is the standard conversion method, but it involves fractions and binary floating point arithmetics, so,
			* instead lets look at how we can represent binary fractions in decimal without the use  of floats. We can 
			* rewrite our previous equation by factoring out the lowest exponent: 2^(-4)*( 1*2^(3) + 1*2^(2) + 0*2^(1) + 1*2^(0) ). Now,
			* we can evaluate separately the numerator and the denominator parts. 
			* 
			* More general formula: 
			* 1011...._2 = 2^(-k) * ( 1*2^(k-1) + 0*2^(k-2) + 1*2^(k-3) + 1*2^(k-4) + ... + m*2^(0) )
			* k - the amount of digits in a binary fraction 
			* m - either 0 or 1.
			*/

			BUILT_INS::int_parser output;

			
			//Note that a numerator part can be evaluated just by using intBinToDec!
			
			output = UTILS::intBinToDec(fractional_part);

			//Early return so the denominator wont be overwritten
			if (output.flag != 0) {

				output.numerator = 0;
				return output;
			}

			output.denominator = BUILT_INS::Power(2, fractional_part.length());

			return output;
		}

		BUILT_INS::int_parser fracBinToDecPeriod(std::string digits_in_period, uint64_t start_index) {
			/*This function doens't use any floating point arithmetics, so integer only calculations! How this works:
			* lets imagine a periodic sequence like 0.(0011)_2 =  0*2^(-1) + 0*2^(-2) + 1*2^(-3) + 1*2^(-4)  + 0*2^(-5) + 0*2^(-6) + 1*2^(-7) + 1*2^(-8) ... 
			* We see that that digits go on repeating forever. We can factor out the exponent and write this sequence in a different manner:
			* 0.(0011)_2 = 2^(0) * ( 0*2^(-1) + 0*2^(-2) + 1*2^(-3) + 1*2^(-4) ) + 2^(-4) * ( 0*2^(-1) + 0*2^(-2) + 1*2^(-3) + 1*2^(-4) ) +
			* + 2^(-8) * ( 0*2^(-1) + 0*2^(-2) + 1*2^(-3) + 1*2^(-4) ) + 2^(-12) * ( 0*2^(-1) + 0*2^(-2) + 1*2^(-3) + 1*2^(-4) )... .Let's factor it out
			* yet again. Now, we get an infinite sequence like: ( 0*2^(-1) + 0*2^(-2) + 1*2^(-3) + 1*2^(-4) ) * ( 2^(0) + 2^(-4) + 2^(-8) + 2^(-16)... )
			* We can see that exponents in the infinite sequence are multiplies of 4, why? Because the amount of digits in period notation  is 4!
			* The exponent in 0*2^(-1) is 1, because the first digit in period notaiton happens to be the first digit after the decimal separator.
			* We can reinterpret the infinite sum as: (sigma)(bottom part: i = 0)(top part: -inf ) ( 2^(4i) * ( 0*2^(-1) + 0*2^(-2) + 1*2^(-3) + 1*2^(-4) ). 
			* Now, we can make a more general equation: (sigma)(bottom part: i = 0)(top part: -inf ) ( 2^(k * i) * ( 0*2^(-(x+1)) + 0*2^(-(x+2)) + 1*2^(-(x+3)) + 1*2^(-(x+4)) +...+ m*2^(-(x+k))).
			* where, k - amount of digits in period, x - index of a first digit in period after a decimal separator (note the first digit after a decimal point will have the x index of 0),
			* m - represents a binary number, either 1 or 0. Now, we can expand this sigma notation to get a finite, converging sequence. By doing so, we will
			* get (1 / (1 - 2^(-k))) * ( 0*2^(-(x+1) + 0*2^(-(x+2)) + 1*2^(-(x+3)) + 1*2^(-(x+4)) +...+ m*2^(-(x+k))). We can rewrite this to get a nicer form by
			* factoring out x:  (1 / (1 - 2^(-k))) * ( 0*2^(-x)*2^(-1) + 0*2^(-x)*2^(-2) + 0*2^(-x)*2^(-3) + 0*2^(-x)*2^(-4) +...+ m*2^(-x)*2^(-k)) = 
			* 2^(-x) * (1 / (1 - 2^(-k))) * ( 0*2^(-1) + 0*2^(-2) + 0*2^(-3) + 0*2^(-4) + ... + m*2^(-k)) 
			*/


			//formula: 2^(-x) * ( 1/ (1 - 2^(-k) ) ) * ( 1*2^(-1) + 1*2^(-2) + 0*2^(-3) + ... + m*2^(-k) )
			//k - amount of digits in period
			//x - index of a first digit in period
			//m - either 0 or 1
			BUILT_INS::int_parser output;
			
			output.flag = fracBinToDec(digits_in_period).flag;

			if (output.flag != 0) {
				
				output.numerator = 0;
				output.denominator = 1;
				return output;
			}
			
			//2^(-x) related calculations:
			//We only need the denominator, because the numerator is 1 and hence wont affect multiplication
			uint64_t denom_digit_shift{ BUILT_INS::Power(2, start_index) };

			
			// ( 1/ (1 - 2^(-k) ) ) related calculations:
			// We can rewrite the 1/(1 - 2^(-k)) to a much more readable (2^(k))/(2^(k) - 1)
			uint64_t num_sigma{ BUILT_INS::Power(2, digits_in_period.length() )};
			uint64_t denom_sigma{ BUILT_INS::Power(2, digits_in_period.length()) - 1 };


			// ( 1*2^(-1) + 1*2^(-2) + 0*2^(-3) + ... + m*2^(-k) ) related calculations:


			uint64_t num_period{ fracBinToDec(digits_in_period).numerator };
			uint64_t denom_period{ fracBinToDec(digits_in_period).denominator };
			

			output.numerator =  num_sigma * num_period;
			output.denominator = denom_digit_shift * denom_sigma * denom_period;

			return output;
		}

		std::string BinaryFlip(std::string input) {

			std::string output{};


			for (char i : input) {

				
				if (i == '0') {
					
					output += '1';
				}

				else if (i == '1') {
					
					output += '0';
				}

				//built-in error handler
				else {

					return "";
				}

			}
			return output;
		}

		std::string BinAdd(std::string operand1, std::string operand2) {
			
			std::string output_string{ "" };

			//Early return for empty string inputs
			if (operand1.length() == 0 || operand2.length() == 0) {
				return "";
			}

			//These variables will be assigned with the last digits of operand_1 and operand_2, and will be
			//constantly overwritten by the next digits to the left, so we can carry a digt-by-digit addition.
			int op1_bit{};
			int op2_bit{};

			//The carry is needed to properly handle the cases ,where the addition of two binary 
			//digits produces a number consisting of two digits: e.g. 1 + 1 = 10.
			int carry{ 0 };
		
			uint64_t max_string_size{};

			max_string_size =  std::max(operand1.length(), operand2.length());

			//These statements are needed to match both operands in their size, so the further loop logic won't
			//be broken. By adding leading zeros to the top of the shortest operand, we ensure equality in the 
			//amount of digits
			operand1.insert(0, max_string_size - operand1.length(), '0');
			operand2.insert(0, max_string_size - operand2.length(), '0');

			for (int i = 1; i <= max_string_size; i++) {

				//By subtracting ASCII index of each digit with ASCII index of zero, we convert characters into integers
				//There is a specific reason why op1_bit and op2_bit are signed: if their ASCII code point
				//will ever be less than 0 (representing any other wrong input), it can be detected with a single
				//if statement.
				op1_bit = operand1[operand1.length() - i] - '0';  
				op2_bit = operand2[operand2.length() - i] - '0'; 

				//If the result of sum exceeds 3 or becomes negative (which is impossible because carry + 
				//op1_bit + op2_bit cant be greater than one, given that 0 < carry, op1_bit, op2_bit <= 1), we error.
				if (op1_bit + op2_bit + carry > 3 || op1_bit + op2_bit + carry < 0) {

					return "";
				}

				//The result of the sum is either 0 or 1. 
				else if (op1_bit + op2_bit + carry <= 1) {

					output_string.insert(0, std::to_string(op1_bit + op2_bit + carry));

					//If carry was the number containing the one, then we already inserted it into 
					//the output_string, hence, we dont need it anymore.
					if (carry == 1) {
						
						carry--;
					}
				}

				//In the following cases, because the number consists of two digits (2_10 = 10_2; 3_10 = 11_2 ), the 
				//latter digit is added to the output_string, whereas the former digit is added to the carry.
				else if (op1_bit + op2_bit + carry == 2) {
					
					carry = 1;
					output_string.insert(0, 1, '0');
				}

				else if (op1_bit + op2_bit + carry == 3) {
					carry = 1;
					output_string.insert(0, 1, '1');
				}

				
			}

			//This if-statement accounts for the leftover carry, that wasn't added during the loop
			if (carry == 1) {

				output_string.insert(0, 1, '1');
			}

			//Unsigned wrap-around-like resizeding, where if the number exceeds the given bit range, 
			//the most significant digits of it will be erased away, until the number can fit 
			if (output_string.length() > MAX_BIT_VALUE) { 

				output_string.erase(0, output_string.length() - MAX_BIT_VALUE);

				
			}

			//By default leading zeros are removed, because their resizing behavior is weird (their amount
			//depends on the size of the longest operand, and they do not resize the number to any default size)
			//output_string = BUILT_INS::INP_HANDLING::INP_EDIT::LeadingZeroRemoval(output_string);
			
			
			return output_string;
		}
		
		bool isRawBin(std::string binary_input, std::vector<uint16_t> allowed_bit_size) {
			//For input to be raw binary, it should not have an explicit sign (e.g. +101 or -110), and should
			//exactly match the allowed_bit_size

			if (binary_input.length() == 0) return false; 

			if (binary_input[0] == '+' || binary_input[0] == '-') return false; 

			for (uint64_t bit_size : allowed_bit_size) {

				if (binary_input.length() == bit_size) {

					return true;
				}
			}
			
			
			return false;
		}
		
		bool isAFlag(std::string input, std::vector<std::string_view> allowed_flags) {

			bool is_a_flag{ false };

			for (std::string_view allowed_flag : allowed_flags) {

				if (input == allowed_flag) {
					
					is_a_flag = true;
				}
			}

			return is_a_flag;
		}

		replxx::Replxx::colors_t& RvalueHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, replxx::Replxx::Color desired_color) {


			std::vector<std::string> tokens{};
			tokens = BUILT_INS::INP_HANDLING::Tokenizer(input_buffer, tokens);

			size_t index{};
			size_t offset{};

			//rValue can only be invoked in SBIN_TO_DEC conversion!
			bool is_rValue_possible{ false };


			for (std::string token : tokens) {

				if (token == "SBIN_TO_DEC") {

					is_rValue_possible = true;
				}

				if (BINARY_CONVERSION::UTILS::isRawBin(token, BINARY_CONVERSION::FORMAT::INT::bit_size_presets) == true && BUILT_INS::INP_HANDLING::INP_CHECK::IsANumber(token, BINARY_SYS) == true && is_rValue_possible) {

					index = input_buffer.find(token);

					if (index != input_buffer.npos) {

						while (offset < token.length()) {

							color_buffer[index + offset] = desired_color;
							offset++;
						}

						offset = 0;
					}

					//Ensures that even if there are two identical rValues, both will be highlighted
					index++;

					is_rValue_possible = false;
				}


			}


			return color_buffer;
		}

	}

	namespace DIAGNOSTICS {

		
		void IntermediateValue(std::string value) {

			std::cout << VIEW::italic;
			std::cout <<"Intermediate value: " << VIEW::reset_italic << value  << '\n';
		}

		void WrapAroundWarning(uint16_t bit_size) {

			std::cout << VIEW::UTILS::rgb666ToString(VIEW::warning_yellow[0], VIEW::warning_yellow[1], VIEW::warning_yellow[2]);
			std::cout << VIEW::italic << "The value has been wrapped around within " << VIEW::reset_italic;
			std::cout << bit_size;
			std::cout << VIEW::italic << " bits." << VIEW::reset_italic << '\a' << '\n';
			std::cout << VIEW::reset;
		}

		void TruncationWarning(uint16_t bit_size) {

			std::cout << VIEW::UTILS::rgb666ToString(VIEW::warning_yellow[0], VIEW::warning_yellow[1], VIEW::warning_yellow[2]);
			std::cout << VIEW::italic;
			std::cout << "The value has been truncated to " << bit_size << " bits." << '\a' << '\n';
			std::cout << VIEW::reset << VIEW::reset_italic;
		}

		void PrecisionLossWarning() {

			std::cout << VIEW::UTILS::rgb666ToString(VIEW::warning_yellow[0], VIEW::warning_yellow[1], VIEW::warning_yellow[2]);
			std::cout << VIEW::italic;
			std::cout << "Precision loss during conversion. \a \n";
			std::cout << VIEW::reset << VIEW::reset_italic;
		}

		void SetFormatMessage(std::string FORMAT, bool conversion_type, bool is_a_fraction) {

			std::string conversion_mode{};
			
			if (conversion_type == DecToBin) {

				
				conversion_mode = "SDEC_TO_BIN";
			}
			else {

				
				conversion_mode = "SBIN_TO_DEC";
			}

			std::string conversion_color{ VIEW::UTILS::rgb666ToString(VIEW::sconv_yellow[0], VIEW::sconv_yellow[1], VIEW::sconv_yellow[2]) };
			conversion_mode.insert(0, conversion_color);
			conversion_mode += VIEW::reset;

			std::string value_type{};
			if (is_a_fraction == true) {

				value_type = "fractional";

			}
			else {

				value_type = "integer";
			}
			
			std::string ANSI_blue{ VIEW::UTILS::rgb666ToString(VIEW::format_blue[0], VIEW::format_blue[1], VIEW::format_blue[2]) };

			std::cout << VIEW::italic << "Format '" << VIEW::reset_italic; 
			std::cout << ANSI_blue + FORMAT + std::string(VIEW::reset);
			std::cout << VIEW::italic << "' has been set for '" << VIEW::reset_italic;
			std::cout << conversion_mode;
			std::cout << VIEW::italic << "' " << value_type << " conversions." << VIEW::reset_italic;
			
		}

		void SetDefaultMessage(std::vector<std::string> default_formats) {

			std::string ANSI_blue{ VIEW::UTILS::rgb666ToString(VIEW::format_blue[0], VIEW::format_blue[1], VIEW::format_blue[2]) };
			std::string conversion_color{ VIEW::UTILS::rgb666ToString(VIEW::sconv_yellow[0], VIEW::sconv_yellow[1], VIEW::sconv_yellow[2]) };

			std::string SDEC_TO_BIN_conversion{conversion_color + "SDEC_TO_BIN" + std::string(VIEW::reset)};
			std::string SBIN_TO_DEC_conversion{ conversion_color + "SBIN_TO_DEC" + std::string(VIEW::reset) };
			

			std::cout << VIEW::italic << "Default formats have been set for '" << VIEW::reset_italic;
			std::cout << SDEC_TO_BIN_conversion;
			std::cout << VIEW::italic << "' and '" << VIEW::reset_italic;
			std::cout << SBIN_TO_DEC_conversion;
			std::cout << VIEW::italic << "': " << VIEW::reset_italic << '\n';
			std::cout << VIEW::italic << "INT  : "  << VIEW::reset_italic << ANSI_blue +  default_formats[0] + std::string(VIEW::reset) << '\n';
			std::cout << VIEW::italic << "FRAC : "  << VIEW::reset_italic << ANSI_blue + default_formats[1] + std::string(VIEW::reset);
		}
		
		void ShowLocal(std::vector<std::string> formats, bool conversion_type) {

			std::string conversion_mode{};

			if (conversion_type == DecToBin) {

				conversion_mode = "SDEC_TO_BIN";
			}
			else {

				conversion_mode = "SBIN_TO_DEC";
			}

			std::string conversion_color{ VIEW::UTILS::rgb666ToString(VIEW::sconv_yellow[0], VIEW::sconv_yellow[1], VIEW::sconv_yellow[2]) };
			conversion_mode.insert(0, conversion_color);
			conversion_mode += VIEW::reset;

			std::string ANSI_blue{ VIEW::UTILS::rgb666ToString(VIEW::format_blue[0], VIEW::format_blue[1], VIEW::format_blue[2]) };


			std::cout << '\n';
			std::cout << conversion_mode << ':' << '\n';
			std::cout << VIEW::italic << "INT  : " << VIEW::reset_italic;
			std::cout <<  ANSI_blue +  formats[0] + std::string(VIEW::reset) << '\n';
			std::cout << VIEW::italic << "FRAC : " << VIEW::reset_italic;
			std::cout << ANSI_blue + formats[1] + std::string(VIEW::reset);
		}

		
	}

	namespace DEFAULT_CONVERSION {

		std::string DEC_TO_BIN(std::vector<std::string> &input_vector ) {
			
			std::string input_value{ BUILT_INS::INP_HANDLING::GetFirstToken(input_vector) };

			BUILT_INS::value parsed_input{ BUILT_INS::INP_HANDLING::InpValueParser(input_value, DECIMAL_SYS, std::string("-") + MAX_64DEC_NUM, MAX_64DEC_NUM) };
			
			if (BUILT_INS::DIAGNOSTICS::ErrorHandler(input_value, parsed_input.flag,  std::string("-") + MAX_64DEC_NUM , MAX_64DEC_NUM) == true) {
				
				return "";
			}

			
			//output is constructed by adding components of a number
			std::string return_val{};

			if (parsed_input.is_a_frac == true) {
				
				return_val += '.';

				BUILT_INS::int_parser output;

				output = BUILT_INS::INP_HANDLING::ToFraction(parsed_input.FRAC_digit_val);

				parsed_input.FRAC_digit_val = UTILS::fracDecToBin(output.numerator, output.denominator, MAX_FRAC_DIGITS);

				if (output.numerator == output.denominator) {

					parsed_input.FRAC_digit_val = '0';
					parsed_input.WHOLE_return_val = std::to_string(BUILT_INS::INP_HANDLING::ToDec(parsed_input.WHOLE_return_val).numerator + 1);

				}
				
				return_val.append(parsed_input.FRAC_digit_val);

			}

			
			parsed_input.WHOLE_return_val = UTILS::intDecToBin(BUILT_INS::INP_HANDLING::ToDec(parsed_input.WHOLE_return_val).numerator);
			parsed_input.WHOLE_return_val = BUILT_INS::INP_HANDLING::INP_EDIT::SignAppender(parsed_input.WHOLE_return_val, parsed_input.sign);

			return_val.insert(0, parsed_input.WHOLE_return_val);
			
			return return_val;
		}
		std::string BIN_TO_DEC(std::vector<std::string>& input_vector) {
			
			std::string input_value{ BUILT_INS::INP_HANDLING::GetFirstToken(input_vector) };

			BUILT_INS::value parsed_input{ BUILT_INS::INP_HANDLING::InpValueParser(input_value, BINARY_SYS, std::string("-") + MAX_64BIN_NUM, MAX_64BIN_NUM ) };
			
			if (BUILT_INS::DIAGNOSTICS::ErrorHandler(input_value, parsed_input.flag, std::string("-") + MAX_64BIN_NUM, MAX_64BIN_NUM) == true) {
				
				return "";
			}
		
			
			//output is constructed by adding components parts of a number
			std::string return_val{};
			
			parsed_input.WHOLE_return_val = std::to_string(UTILS::intBinToDec(parsed_input.WHOLE_return_val).numerator);
			
			if (parsed_input.is_a_frac == true) {
		
				return_val += '.';
				
				BUILT_INS::int_parser output;

				uint64_t numerator{ UTILS::fracBinToDec(parsed_input.FRAC_nonperiod_val).numerator };
				uint64_t denominator{ UTILS::fracBinToDec(parsed_input.FRAC_nonperiod_val).denominator };

				//Add the logic for carry 
				if (BUILT_INS::INP_HANDLING::INP_CHECK::IsPeriodic(parsed_input.FRAC_digit_val) == true) {

					uint64_t period_index{ parsed_input.FRAC_nonperiod_val.length() };
					
					uint64_t period_numerator{ BINARY_CONVERSION::UTILS::fracBinToDecPeriod(parsed_input.FRAC_period_val, period_index).numerator };
					uint64_t period_denominator{ BINARY_CONVERSION::UTILS::fracBinToDecPeriod(parsed_input.FRAC_period_val, period_index).denominator };
					
					//Addition of these two fractions 
					numerator = numerator * period_denominator + period_numerator * denominator;
					denominator *= period_denominator;

					
				}

				
				//Carry handling (for inputs like 101.(111))
				if (numerator == denominator) {

					parsed_input.WHOLE_return_val = std::to_string(BUILT_INS::INP_HANDLING::ToDec(parsed_input.WHOLE_return_val).numerator + 1);

					parsed_input.FRAC_digit_val = "0";
				}

				else {

					parsed_input.FRAC_digit_val = BUILT_INS::DecimalDivision(numerator, denominator, MAX_FRAC_DIGITS).FRAC_digit_val;
				}
				

				return_val += parsed_input.FRAC_digit_val; 

				if (BUILT_INS::DecimalDivision(numerator, denominator, MAX_FRAC_DIGITS).flag == F_IMPRECISE_OUTPUT) {
					
					return_val.append("...");
				}
			}
		
			parsed_input.WHOLE_return_val = BUILT_INS::INP_HANDLING::INP_EDIT::SignAppender(parsed_input.WHOLE_return_val, parsed_input.sign);

			//We don't assign the WHOLE_return_val to return_val, because we first need to handle carry logic, and only then add it all together
			return_val.insert(0, parsed_input.WHOLE_return_val);
		
			return return_val;
		}
	}

	namespace STANDARD_CONVERSION {
		
		
		uint16_t modeSelector(std::string argument) {
		
			

			//Mode 1: Error State:
			if (argument.length() == 0) {

				return F_INVALID_INP;
			}
			
			
				
			//Mode 10: --set_global
			if (argument == flag_mode[0]) {

				return set_global;
			}

			//Mode 11: --set_local
			else if (argument == flag_mode[1]) {

				return set_local;
			}

			//Mode 12: --set_default
			else if (argument == flag_mode[2]) {

				return set_default;
			}

			//Mode 13: --show_global
			else if (argument == flag_mode[3]) {

				return show_global;
			}

			//Mode 14: --show_local
			else if (argument == flag_mode[4]) {

				return show_local;
			}
				
			

			return value_mode;

		}
		
		std::string STANDARD_CONVERTER(std::vector<std::string> &input, bool conversion_type) {
			//Conversion type is a bool because we either convert DEC_TO_BIN or BIN_TO_DEC
			//Built-in formats: 
			//INT - TWOS_COMP
			//FRAC - IEEE754 FAMILY

			std::string input_token{ BUILT_INS::INP_HANDLING::GetFirstToken(input) };
			std::string return_val{};
			
			//std::string user_input_string{input_token + ' ' + arg2};
			
			
			
			//Initial parsing
			uint16_t converter_mode{ modeSelector(input_token) };

			//Error state
			if (converter_mode == F_INVALID_INP) {
			
				BUILT_INS::DIAGNOSTICS::ConversionError(input_token);
				return "";
			}

			//Value-based mode
			else if (converter_mode == value_mode) {
				//built - in formats
				//INT - TWOS_COMP
				//FRAC - IEEE754 FAMILY

				//Before extracting the data for flag, we check whether is it a valid flag
				//If so, the std::string flag will be overwritten, if no, input vector keeps its token
				std::string flag{};
				bool is_a_flag = false;
				
				if (!input.empty()) {

					is_a_flag = UTILS::isAFlag(input[0], FORMAT::INT::resize_flags);
				}

				if (is_a_flag == true) {
					flag = BUILT_INS::INP_HANDLING::GetFirstToken(input);
				}

				

				if (conversion_type == DecToBin) {

					return_val = FORMAT::FORMAT_CONVERTER(conversion_type, input_token, flag, SDEC_TO_BIN_formats[0]);
				}
				
				else {

					return_val = FORMAT::FORMAT_CONVERTER(conversion_type, input_token, flag, SBIN_TO_DEC_formats[0]);
				}


			}

			//Format-change
			else if (converter_mode == set_local || converter_mode == set_global){
				
				std::string user_format{ BUILT_INS::INP_HANDLING::GetFirstToken(input)};

			
				uint16_t type{ FORMAT::formatType(user_format) };

				//UNSIGNED isn't recognized as a configurable format, its just an implicit one which can be called via U prefix
				if (type == F_INVALID_INP || user_format == int_formats[0]) {


					BUILT_INS::DIAGNOSTICS::ConversionError(user_format);
					return "";

				}

				if (converter_mode == set_global) {

					
					if (type == INT_FORMAT) {

						SDEC_TO_BIN_formats[0] = user_format;
						SBIN_TO_DEC_formats[0] = user_format;
						
						DIAGNOSTICS::SetFormatMessage(user_format, DecToBin, false);
						std::cout << "\n";
						DIAGNOSTICS::SetFormatMessage(user_format, BinToDec, false);
					}

					else if (type == FRAC_FORMAT) {

						SDEC_TO_BIN_formats[0] = user_format;
						SBIN_TO_DEC_formats[0] = user_format;
						
						DIAGNOSTICS::SetFormatMessage(user_format, DecToBin, true);
						std::cout << "\n";
						DIAGNOSTICS::SetFormatMessage(user_format, BinToDec, true);
					}

				}

				else if (converter_mode == set_local) {

					if (conversion_type == DecToBin) {

						if (type == INT_FORMAT) {

							SDEC_TO_BIN_formats[0] = user_format;
							DIAGNOSTICS::SetFormatMessage(user_format, DecToBin, false);
						}

						else if (type == FRAC_FORMAT) {

							SDEC_TO_BIN_formats[1] = user_format;
							DIAGNOSTICS::SetFormatMessage(user_format, DecToBin, true);
						}
					}
					
					else {

						if (type == INT_FORMAT) {

							SBIN_TO_DEC_formats[0] = user_format;
							DIAGNOSTICS::SetFormatMessage(user_format, BinToDec, false);
						}

						else if (type == FRAC_FORMAT) {

							SBIN_TO_DEC_formats[1] = user_format;
							DIAGNOSTICS::SetFormatMessage(user_format, BinToDec, true);
						}
					}

					
				}
			}

			//No arg flags
			else {

				

				if (converter_mode == set_default) {

					SDEC_TO_BIN_formats = default_formats;
					SBIN_TO_DEC_formats = default_formats;
					DIAGNOSTICS::SetDefaultMessage(default_formats);

				}

				else if (converter_mode == show_local) {

					if (conversion_type == DecToBin) {

						DIAGNOSTICS::ShowLocal(SDEC_TO_BIN_formats, conversion_type);
					}

					else {

						DIAGNOSTICS::ShowLocal(SBIN_TO_DEC_formats, conversion_type);
					}
					
				}

				else if (converter_mode == show_global) {

					DIAGNOSTICS::ShowLocal(SDEC_TO_BIN_formats, DecToBin);
					std::cout << '\n';
					DIAGNOSTICS::ShowLocal(SBIN_TO_DEC_formats, BinToDec);
				}
			}

			return return_val;
		}
		
	}

	//Note! Format converter functions assume number systems are properly set 
	namespace FORMAT {

		bin_string_parser FormatOutputer(bool conversion_type, BUILT_INS::value input, uint16_t bit_size, std::string_view FORMAT) {

			bin_string_parser output;


			//Integer-type 
			
			//UNSIGNED 
			if (FORMAT == int_formats[0]) {

				if (conversion_type == DecToBin) {

					output = FORMAT::INT::Unsigned::DecToRaw(input, bit_size);

					
					
				}
					
				else {
					
					output = FORMAT::INT::Unsigned::RawToDec(input, bit_size);
				}
				
			}
			
			//SIGN_MAG
			else if (FORMAT == int_formats[1]) {

				if (conversion_type == DecToBin) {

					output = FORMAT::INT::SignMagnitude::DecToRaw(input, bit_size);
				}

				else {

					output = FORMAT::INT::SignMagnitude::RawToDec(input, bit_size);
				}

				
			}

			//ONES_COMP
			else if (FORMAT == int_formats[2]) {

				if (conversion_type == DecToBin) {

					output = FORMAT::INT::OnesComplement::DecToRaw(input, bit_size);
				}

				else {

					output =  FORMAT::INT::OnesComplement::RawToDec(input, bit_size);
				}

				
			}

			//TWOS_COMP
			else if (FORMAT == int_formats[3]) {

				if (conversion_type == DecToBin) {

					output = FORMAT::INT::TwosComplement::DecToRaw(input, bit_size);

				}

				
				else {

					output = FORMAT::INT::TwosComplement::RawToDec(input, bit_size);

				}

			}

			else {

				BUILT_INS::DIAGNOSTICS::ConversionError(static_cast<std::string>(FORMAT));
			}

			return output;
		}

		uint16_t formatType(std::string_view input_format) {
			
			for (std::string_view int_format : int_formats) {

				if (input_format == int_format) {

					return STANDARD_CONVERSION::INT_FORMAT;
				}
			}

			for (std::string_view frac_format : FORMAT::frac_formats) {

				if (input_format == frac_format) {

					return STANDARD_CONVERSION::FRAC_FORMAT;
				}
			}

			return F_INVALID_INP;
		}

		std::string FORMAT_CONVERTER(bool conversion_type, std::string input_value, std::string resize_flag, std::string_view FORMAT) {
			//FDEC_TO_BIN handles two general cases:
			//If input is unsigned (e.g. -5u), it is fully processed in the converter itself
			//In any other case, it is converted in the format-specific function

			bin_string_parser output;

			

			//resize_flag resolve
			uint16_t bit_size{ INT::ResizeFlagParser(resize_flag) };
			if (bit_size == F_INVALID_INP) {

				
				BUILT_INS::DIAGNOSTICS::ConversionError(resize_flag);
				return "";
			}
			
			//user_input will be processed while input_value is kept for diagnostic messages
			std::string user_input{ input_value };

			//Suffix handling 
			if (BUILT_INS::INP_HANDLING::INP_CHECK::IsUnsigned(user_input) == true) {

				
				FORMAT = int_formats[0];
				user_input = BUILT_INS::INP_HANDLING::INP_EDIT::RemoveSuffix(user_input);
			}
			
			//Parameters for the conversion
			std::string max_val{};
			std::string num_sys{};
			//Cuts both the trailing and leading zeros
			bool remove_zeros{};
			
			
			if (conversion_type == DecToBin) {
				max_val = MAX_64DEC_NUM;
				num_sys = DECIMAL_SYS;
				remove_zeros = true;
			}

			else {
				
				max_val = MAX_64BIN_NUM;
				num_sys = BINARY_SYS;
				//Because binary inputs can be Rvalues, we may need both leading and trailing zeros 
				remove_zeros = false;
			}

			//Input validation
			BUILT_INS::value parsed_value{ BUILT_INS::INP_HANDLING::InpValueParser(user_input, num_sys, std::string("-") + max_val, max_val, 16, remove_zeros, remove_zeros, true ) };
			if (BUILT_INS::DIAGNOSTICS::ErrorHandler(input_value, parsed_value.flag, std::string("-") + max_val, max_val) == true) {
				
				return "";

			}

			//If we have a fractional like input being processed on integer-compatible formats, we raise a warning
			if (parsed_value.is_a_frac == true && formatType(FORMAT) == 1) {

				DIAGNOSTICS::PrecisionLossWarning();
			}

			output = FormatOutputer(conversion_type, parsed_value, bit_size, FORMAT);

			return output.value;

		}

		namespace INT {
			
			

			bin_string_parser BitResizer(std::string input, bool account_for_sign_bit, uint16_t bit_size) {

				bin_string_parser output;

				uint16_t biased_bit{};

				//Sign bit should not carry any numeric value, this way we essentially bias the input to 7, 15, 31 and 63 bits
				if (account_for_sign_bit == true) {

					biased_bit++;
				}

				if (bit_size == 0) {

					output = AutoResize(std::string("0", biased_bit) + input);

				}

				else if (bit_size == 8 || bit_size == 16 || bit_size == 32 || bit_size == 64) {

					if (input.length() + biased_bit > bit_size) {

					
						output.flag = F_TRUNCATION;
					}
					
					output.value = ResizeTo(input, bit_size ).value;
					

				}

				else {

					output.flag = F_INVALID_INP;
					output.value = bin_string_parser{}.value;
					return output;
				}


				return output;
			}

			bin_string_parser AutoResize(std::string input) {

				bin_string_parser output;



				if (input.length() <= 8) {

					output = ResizeTo(input, 8);
				}

				else if (input.length() <= 16) {

					output = ResizeTo(input, 16);
				}

				else if (input.length() <= 32) {

					output = ResizeTo(input, 32);
				}

				else {

					//Truncation
					output = ResizeTo(input, 64);
				}



				return output;
			}

			bin_string_parser ResizeTo(std::string input, uint16_t bit_width) {

				bin_string_parser output;

				output.value = input;



				if (input.length() > bit_width) {

					//Simply cuts off the unnescesarry values
					output.value = output.value.substr(input.length() - bit_width);
					//Warning that the value has been wrapped around
					output.flag = F_TRUNCATION;
				}

				else if (input.length() < bit_width) {

					output.value.insert(0, bit_width - input.length(), '0');
				}


				return output;
			}

			bin_string_parser BinToRaw(BUILT_INS::value input, uint16_t bit_size, std::string FORMAT) {
				
				bin_string_parser output;

				output.flag = input.flag;
				if (output.flag != 0) return output;

				output.value = input.WHOLE_return_val;
				output.value = BUILT_INS::INP_HANDLING::INP_EDIT::LeadingZeroRemoval(output.value);

				bool account_for_sign_bit{ true };
				
				if (FORMAT == int_formats[0]) {
					//Unsigned does not have any sign bit
					account_for_sign_bit = false;
				}

				output = BitResizer(output.value, account_for_sign_bit, bit_size);

				

				if (output.flag == F_INVALID_INP) {

					return output;
				}


				//Wrap-around logic
				if (input.sign == 1) {

					//UNSIGNED
					if (FORMAT == int_formats[0]) {

						if (output.value != "0") {

							uint16_t displayed_bit_size{ bit_size };

							if (bit_size == 0)
							{
								//If no flag provided, by default we wrap around 8 bit integer
								displayed_bit_size = 8;
							}

							output.value = UTILS::BinaryFlip(output.value);
							output.value = UTILS::BinAdd(output.value, "1");
							output.value = ResizeTo(output.value, displayed_bit_size).value;
							
							output.flag = F_WRAP_AROUND;


						}
					}

					//SIGN_MAG
					else if (FORMAT == int_formats[1]) {

						output.value[0] = '1';
					}

					//ONES_COMP
					else if (FORMAT == int_formats[2]) {

						output.value = UTILS::BinaryFlip(output.value);
					}

					//TWOS_COMP 
					else if (FORMAT == int_formats[3]) {
						
						//In order to apply the assymetry in Two's Complement, we check if the input equals to the lower bound of Two's complement, if so, we remove the F_TRUNCATION
						//exponent overflows!
						
						uint64_t min_bound{ BINARY_CONVERSION::UTILS::intBinToDec(input.WHOLE_return_val).numerator };
						
						if (min_bound == BUILT_INS::Power(2, 7) || min_bound == BUILT_INS::Power(2, 15) || min_bound == BUILT_INS::Power(2, 31) || min_bound == BUILT_INS::Power(2, 63)) {
							
							//Removing all of the leading zeros added by automatic resizing
							//Because the lower bound of Two's Complement is a power of two, the leading bit will always be 1, hence 
							//it is safe to remove all of the leading zeros, as the bit size will be kept
							if (bit_size == 0) {

								output.value = BUILT_INS::INP_HANDLING::INP_EDIT::LeadingZeroRemoval(output.value);
							}
							
							output.flag = 0;
						}

						output.value = UTILS::BinaryFlip(output.value);
						std::string appended_val{ UTILS::BinAdd(output.value, "1") };

						if (output.value.length() != appended_val.length()) {

							//Cropping out the digit that appeared in front
							output.value = appended_val.substr(1);
						}

						else {

							output.value = appended_val;
						}

						
					}
				}

				

				return output;

			}


			uint16_t ResizeFlagParser(std::string flag) {

				if (flag.length() == 0) {

					return 0;
				}

				else if (flag == FORMAT::INT::resize_flags[0]) {

					return 8;
				}

				else if (flag == FORMAT::INT::resize_flags[1]) {

					return 16;
				}

				else if (flag == FORMAT::INT::resize_flags[2]) {

					return 32;
				}

				else if (flag == FORMAT::INT::resize_flags[3]) {

					return 64;
				}


				//Zero is reserved for the error state 
				return F_INVALID_INP;
			}

			

			
			//Value conversion to binary -> Value resizeding -> Value Conversion to Format.
			namespace Unsigned {
				
				uint64_t wrapAround(uint64_t negative_input, uint16_t bit_size) {

					//Early-return to prevent any possible errors
					if (bit_size > 64) {
						return 0;
					}

					uint64_t max_int{ BUILT_INS::Power(2,bit_size) };

					//Hardware level overflow (prevents division by zero in the last return statement, when max_int  itself wraps around)
					if (bit_size == 64) {

						return 0 - negative_input;

					}

					//To mimick the wrap around behavior we subtract from max_int the negative input, but this
					//will overflow if negative_input > max_int. In order to avoid putting another conditional statement
					//we can subtract ((negative_input + max_int) % max_int) instead, this way the value will never get greater 
					//than max_int, however with the following expression (max_int - (negative_input + max_int) % max_int) we don't
					//have zero, but rather instead of this, max_int is represented. To fix even this we take a remainder, if the expression
					//will ever evaluate to 256 (given, for instance, 8 bits) 256%256 = 0
					return (max_int - (negative_input + max_int) % max_int) % max_int;
				}

				bin_string_parser DecToRaw(BUILT_INS::value input, uint16_t bit_size, bool display_IR) {

					bin_string_parser output;

					output.flag = input.flag; 
					if (output.flag != 0) return output;

					uint64_t integer_value{ BUILT_INS::INP_HANDLING::ToDec(input.WHOLE_return_val).numerator };
					
					//-0 is meaningless in UNSIGNED, and because we don't want to envoke Wrap around message, we
					//change it to positive 0.
					if (integer_value == 0 && input.sign == 1) {

						input.sign = 0;
					}
					
					input.WHOLE_return_val = UTILS::intDecToBin(integer_value);

					output = BinToRaw(input, bit_size, static_cast<std::string>(int_formats[0]));

					if (output.flag == F_WRAP_AROUND) {

						if (bit_size == 0) {

							DIAGNOSTICS::WrapAroundWarning(8);
						}

						else {

							DIAGNOSTICS::WrapAroundWarning(bit_size);

						}
						
						output.intermediate_value =  std::to_string(UTILS::intBinToDec(output.value).numerator);
						
					}

					if (output.flag == F_TRUNCATION) {

						if (bit_size == 0) {

							DIAGNOSTICS::TruncationWarning(64);

						}

						else {

							DIAGNOSTICS::TruncationWarning(bit_size);

						}
						
						output.intermediate_value = std::to_string(UTILS::intBinToDec(output.value).numerator);
						
					}

					if (display_IR && output.intermediate_value.length() != 0) {
						DIAGNOSTICS::IntermediateValue(output.intermediate_value);
					}

					return output;
				}
				
				bin_string_parser RawToDec(BUILT_INS::value input, uint16_t bit_size) {

					bin_string_parser output;

					output.flag = input.flag;
					if (output.flag != 0) return output;

					//-0 is meaningless for unsigneds
					if (BUILT_INS::INP_HANDLING::INP_EDIT::LeadingZeroRemoval(input.WHOLE_return_val) == "0" && input.sign == 1) {

						input.sign = 0;
					}

					//MValue handling
					if (input.has_explicit_plus == true || input.sign == 1 || UTILS::isRawBin(input.WHOLE_return_val, bit_size_presets) == false) {

						input.WHOLE_return_val = BUILT_INS::INP_HANDLING::INP_EDIT::LeadingZeroRemoval(input.WHOLE_return_val);
						input.WHOLE_return_val = std::to_string(UTILS::intBinToDec(input.WHOLE_return_val).numerator);
						
						output = DecToRaw(input, bit_size, false);
						
						DIAGNOSTICS::IntermediateValue(output.value);
					}
					
					//Rvalue handling
					else {
						
						output.value = input.WHOLE_return_val;

						//IR will be output in any way because we modified our initial rValue with the resize flag
						if (bit_size != output.value.length() && bit_size != 0) {

							output = BitResizer(output.value, false, bit_size);

							if (output.flag == F_TRUNCATION) {


								if (bit_size == 0) {

									DIAGNOSTICS::TruncationWarning(64);
									DIAGNOSTICS::IntermediateValue(output.value);
								}

								else {

									DIAGNOSTICS::TruncationWarning(bit_size);
								}
							}
							DIAGNOSTICS::IntermediateValue(output.value);
						}
					}

					output.value = std::to_string(UTILS::intBinToDec(output.value).numerator);

					return output;
				}
			
			}

			namespace SignMagnitude {
				
				bin_string_parser DecToRaw(BUILT_INS::value input, uint16_t bit_size, bool display_IR) {
					
					bin_string_parser output;

					output.flag = input.flag;
					if (output.flag != 0) return output;

					uint64_t integer_value{ BUILT_INS::INP_HANDLING::ToDec(input.WHOLE_return_val).numerator };
					input.WHOLE_return_val = UTILS::intDecToBin(integer_value);

					output = BinToRaw(input, bit_size, static_cast<std::string>(int_formats[1]));

					if (output.flag == F_TRUNCATION) {

						if (bit_size == 0) {

							DIAGNOSTICS::TruncationWarning(64);

						}

						else {

							DIAGNOSTICS::TruncationWarning(bit_size);

						}

						//Removing the sign bit, so it won't mess up with calculations for the absolute value 
						output.intermediate_value =  std::to_string(UTILS::intBinToDec(output.value.substr(1)).numerator);
						
						if (output.value[0] == '1') {
							
							output.intermediate_value.insert(0, 1, '-');
						}	
										
					
						
					}

					if (display_IR && output.intermediate_value.length() != 0) {
						DIAGNOSTICS::IntermediateValue(output.intermediate_value);
					}

					return output;
				}	

				bin_string_parser RawToDec(BUILT_INS::value input, uint16_t bit_size) {

					bin_string_parser output;

					output.flag = input.flag;
					if (output.flag != 0) return output;


					//MValue handling
					if (input.has_explicit_plus == true || input.sign == 1 || UTILS::isRawBin(input.WHOLE_return_val, bit_size_presets) == false) {

						input.WHOLE_return_val = BUILT_INS::INP_HANDLING::INP_EDIT::LeadingZeroRemoval(input.WHOLE_return_val);
						input.WHOLE_return_val = std::to_string(UTILS::intBinToDec(input.WHOLE_return_val).numerator);

						output = DecToRaw(input, bit_size, false);

						DIAGNOSTICS::IntermediateValue(output.value);
					}

					//Rvalue handling
					else {

						output.value = input.WHOLE_return_val;

						if (bit_size != 0 && bit_size != output.value.length()) {

							//Preserving the sign as a leading bit
							if (bit_size > output.value.length() && output.value[0] == '1') {

							
								output.value[0] = '0';
								output = BitResizer(output.value, false, bit_size);
								output.value[0] = '1';

							}

							else {

								output = BitResizer(output.value, false, bit_size);

								if (output.flag == F_TRUNCATION) {


									if (bit_size == 0) {

										DIAGNOSTICS::TruncationWarning(64);
										DIAGNOSTICS::IntermediateValue(output.value);
									}

									else {

										DIAGNOSTICS::TruncationWarning(bit_size);
									}
								}
								
							}

							DIAGNOSTICS::IntermediateValue(output.value);
						}
					}

					std::string sign{};
					if (output.value[0] == '1') {

						sign = "-";
						output.value = output.value.substr(1);
					}

					output.value = sign + std::to_string(UTILS::intBinToDec(output.value).numerator);

					return output;
				}

			}
			namespace OnesComplement {

				bin_string_parser DecToRaw(BUILT_INS::value input, uint16_t bit_size, bool display_IR) {

					bin_string_parser output;

					output.flag = input.flag;
					if (output.flag != 0) return output;

					uint64_t integer_value{ BUILT_INS::INP_HANDLING::ToDec(input.WHOLE_return_val).numerator };
					input.WHOLE_return_val = UTILS::intDecToBin(integer_value);

					output = BinToRaw(input, bit_size, static_cast<std::string>(int_formats[2]));

					if (output.flag == F_TRUNCATION) {

						if (bit_size == 0) {

							DIAGNOSTICS::TruncationWarning(64);

						}

						else {

							DIAGNOSTICS::TruncationWarning(bit_size);

						}

						
						output.intermediate_value =  output.value;
						
						if (output.intermediate_value[0] == '1') {
							
							output.intermediate_value = UTILS::BinaryFlip(output.intermediate_value);
							output.intermediate_value = std::to_string(UTILS::intBinToDec(output.intermediate_value).numerator);
							output.intermediate_value.insert(0, 1, '-');
							
							
							
						}

						else {
							output.intermediate_value = std::to_string(UTILS::intBinToDec(output.intermediate_value).numerator);

						}

						
						
					}

					if (display_IR && output.intermediate_value.length() != 0) {
						DIAGNOSTICS::IntermediateValue(output.intermediate_value);
					}

					return output;


				}

				bin_string_parser RawToDec(BUILT_INS::value input, uint16_t bit_size) {

					bin_string_parser output;

					output.flag = input.flag;
					if (output.flag != 0) return output;


					//MValue handling
					if (input.has_explicit_plus == true || input.sign == 1 || UTILS::isRawBin(input.WHOLE_return_val, bit_size_presets) == false) {

						input.WHOLE_return_val = BUILT_INS::INP_HANDLING::INP_EDIT::LeadingZeroRemoval(input.WHOLE_return_val);
						input.WHOLE_return_val = std::to_string(UTILS::intBinToDec(input.WHOLE_return_val).numerator);

						output = DecToRaw(input, bit_size, false);

						DIAGNOSTICS::IntermediateValue(output.value);
					}

					//Rvalue handling
					else {

						output.value = input.WHOLE_return_val;

						if (bit_size != 0 && bit_size != output.value.length()) {

							//Preserving the sign as a leading bit
							if (bit_size > output.value.length() && output.value[0] == '1') {


								//Because the input is negative, we should append '1's instead of '0's
								output.value.insert(0, bit_size - output.value.length() , '1');

							}

							else {

								output = BitResizer(output.value, false, bit_size);

								if (output.flag == F_TRUNCATION) {


									if (bit_size == 0) {

										DIAGNOSTICS::TruncationWarning(64);
										DIAGNOSTICS::IntermediateValue(output.value);
									}

									else {

										DIAGNOSTICS::TruncationWarning(bit_size);
									}
								}

							}

							DIAGNOSTICS::IntermediateValue(output.value);
						}
					}

					std::string sign{};
					if (output.value[0] == '1') {

						sign = "-";
						output.value = UTILS::BinaryFlip(output.value);
					}

					output.value = sign + std::to_string(UTILS::intBinToDec(output.value).numerator);

					return output;
				}

			}
			namespace TwosComplement {

				bin_string_parser DecToRaw(BUILT_INS::value input, uint16_t bit_size, bool display_IR) {

					bin_string_parser output;

					output.flag = input.flag;
					if (output.flag != 0) return output;

					uint64_t integer_value{ BUILT_INS::INP_HANDLING::ToDec(input.WHOLE_return_val).numerator };
					input.WHOLE_return_val = UTILS::intDecToBin(integer_value);

					output = BinToRaw(input, bit_size, static_cast<std::string>(int_formats[3]));

					if (output.flag == F_TRUNCATION) {

						if (bit_size == 0) {

							DIAGNOSTICS::TruncationWarning(64);

						}

						else {

							DIAGNOSTICS::TruncationWarning(bit_size);

						}

						output.intermediate_value =  output.value;

						//-0 is ignored because TWOS_COMP doesnt have signed zeros
						if (output.intermediate_value[0] == '1' && BUILT_INS::INP_HANDLING::INP_EDIT::LeadingZeroRemoval(output.value) != "0") {

							
							output.intermediate_value = UTILS::BinaryFlip(output.intermediate_value);
							output.intermediate_value = std::to_string(UTILS::intBinToDec(output.intermediate_value).numerator + 1);
							output.intermediate_value.insert(0, 1, '-');
							
						}

						else {

							//Here the number is positive, hence Two's Complement transformation is irrelevant
							output.intermediate_value = std::to_string(UTILS::intBinToDec(output.intermediate_value).numerator);
						}
						
					
					}

					if (display_IR && output.intermediate_value.length() != 0) {
						
						
						DIAGNOSTICS::IntermediateValue(output.intermediate_value);
					}

					return output;


				}

				bin_string_parser RawToDec(BUILT_INS::value input, uint16_t bit_size) {

					bin_string_parser output;

					output.flag = input.flag;
					if (output.flag != 0) return output;


					//MValue handling
					if (input.has_explicit_plus == true || input.sign == 1 || UTILS::isRawBin(input.WHOLE_return_val, bit_size_presets) == false) {

						input.WHOLE_return_val = BUILT_INS::INP_HANDLING::INP_EDIT::LeadingZeroRemoval(input.WHOLE_return_val);
						input.WHOLE_return_val = std::to_string(UTILS::intBinToDec(input.WHOLE_return_val).numerator);

						output = DecToRaw(input, bit_size, false);
						DIAGNOSTICS::IntermediateValue(output.value);
					}

					//Rvalue handling
					else {

						output.value = input.WHOLE_return_val;

						if (bit_size != 0 && bit_size != output.value.length()) {

							//Preserving the sign as a leading bit
							if (bit_size > output.value.length() && output.value[0] == '1') {


								//Because the input is negative, we should append '1's instead of '0's
								output.value.insert(0, bit_size - output.value.length(), '1');
							}

							else {

								output = BitResizer(output.value, false, bit_size);

								if (output.flag == F_TRUNCATION) {


									if (bit_size == 0) {

										DIAGNOSTICS::TruncationWarning(64);
										DIAGNOSTICS::IntermediateValue(output.value);
									}

									else {

										DIAGNOSTICS::TruncationWarning(bit_size);
									}
								}

							}

							DIAGNOSTICS::IntermediateValue(output.value);
						}
					}

					std::string sign{};
					if (output.value[0] == '1') {

						sign = "-";
						output.value = UTILS::BinaryFlip(output.value);
						output.value = UTILS::BinAdd(output.value, "1");
						
					}

					output.value = sign + std::to_string(UTILS::intBinToDec(output.value).numerator);

					return output;
				
				}

			}

			

		}

		
	}

	
	

}

