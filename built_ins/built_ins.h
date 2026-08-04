#ifndef BUILT_INS_10_29_2025
#define BUILT_INS_10_29_2025

//NOTE! CHANGING THIS VALUE TO SOMETHING MORE OR LESS WILL RESULT IN UNDEFINED BEHAVIOR!
#define MAX_BIT_VALUE 64

//NUMBER SYSTEMS FOR A CONVERTER (ARRANGE IN THE GROWTH ORDER)
/*
NOTE!Any number system that will be implemented should be represented
in a growing manner where each next digit should be greater than the previous but 
less than the next (e.g. 0,1,2,3...). Not only that, but their ASCII representations
should also be arranged in the same code point growing order!
*/
#define DECIMAL_SYS "0123456789" 
#define BINARY_SYS "01"

//LIMITS
#define MAX_64DEC_NUM "18446744073709551615"
#define MAX_32DEC_NUM ""
#define MAX_16DEC_NUM ""
#define MAX_8DEC_NUM "255"


#define MAX_64BIN_NUM "1111111111111111111111111111111111111111111111111111111111111111"
#define MAX_32BIN_NUM "11111111111111111111111111111111"
#define MAX_16BIN_NUM "1111111111111111"
#define MAX_8BIN_NUM "11111111"
#define MAX_FRAC_DIGITS 16


//ERROR FLAGS AND THEIR MEANING
#define F_INVALID_INP 1 //Invalid (for a given number system) input
#define F_OUT_OF_BOUNDS 5       //Input exceeds the bounds (Only true for integer part)
#define F_SIGNIFICAND_OVERFLOW 6   //Digits of fractional part exceed MAX_FRAC_DIGITS
#define F_IMPRECISE_OUTPUT 7 //Indicates that during division output was truncated to fit the fractional digit limit 
#define F_EXIT 8 //Exit flag reserved specially for 'exit' command
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>

namespace BUILT_INS {
	//NOTE! None using special characaters for function arguments will result in wrong or even undefined behavior

	//All default keywords
	const inline std::vector<std::string_view> commands{"exit" };



	//struct for handling command inputs by user (e.g. 'SDEC_TO_BIN 54 --resize_to_16') 
	struct user_input {

		uint16_t flag{};

		//Needed mainly for diagnostics
		std::string unparsed_input{};
		std::vector<std::string> tokens{};
		std::string invalid_token{};
	};

	//The main  struct for user argument parsing
	struct value {
	    
		//Whole part of input
		std::string WHOLE_return_val{};

		//Fractional part of input
		std::string FRAC_digit_val{}; 

		
		std::string FRAC_nonperiod_val{}; //Digits outside of the periodic notation
		std::string FRAC_period_val{}; //Digits inside the periodic notation 
		bool is_a_frac{ false };

		bool sign{ 0 };
		//Useful for handling inputs like +5, where the '+' sign is written explicitly 
		bool has_explicit_plus{ 0 };
		
		uint16_t flag{ 0 };
	};

	//The main output struct for internal conversion integer-based functions
	struct int_parser {

		bool sign{ 0 };

		uint64_t numerator{};
		//To prevent division by zero issues 
		uint64_t denominator{ 1 };
		uint16_t flag{};
	};

	//Power function with unsigned integer only support
	uint64_t Power(uint64_t base, uint64_t exponent);

	//String based division function, that doesn't utilize floating point arithmetics. 
	value DecimalDivision(uint64_t numerator, uint64_t denominator, uint16_t precision);

	namespace INP_HANDLING {

		//Initial parsing of the whole input before CommandCheck
		user_input GetParsedInput(user_input input);

		std::string removeComments(std::string_view unparsed_string);

		//Splits an input string into a vector containing tokens.
		std::vector<std::string> Tokenizer(std::string unparsed_string, std::vector<std::string> vector);

		//Gets the first token from the input, returns it and deletes it from teh input_vector
		std::string GetFirstToken(std::vector<std::string>& input_vector);

		//By selecting the number for the argument we fetch a specific word out of the input line. 
		//If argument exceeds the amount of words in an input_line, the function returns an empty string.
		std::string inputFetch(std::string input_line, uint16_t argument);

		namespace INP_EDIT {
			
			//Converts a wide character into a '?' in order to prevent crashes
			void utf_filter(std::string& input, int& cursor);

			//Removes literal suffixes from a stirng
			std::string RemoveSuffix(std::string input);

			//Removes all empty spaces from a string.
			std::string SpaceRemoval(std::string input);

			//Removes all trailing spaces from a string
			std::string TrailingSpaceRemoval(std::string input);

			//Removes all newlines 
			std::string ReplaceNewline(std::string input);

			//Strips all leading '0' characters from the input string. If the input string is empty,
			//or if all characters are removed during this process, the function ensures the result is "0".
			std::string LeadingZeroRemoval(std::string input);
			
			//Strips all trailing '0' characters from the input string. If the input string is empty,
			//or if all characters are removed during this process, the function ensures the result is "0".
			std::string TrailingZeroRemoval(std::string input);
			
			//Appends the sign before the first character of the input string, based on whether the bool sign is 0 or 1. 
			std::string SignAppender(std::string input, bool sign);
			
			//Removes the sign of input, so (+5 becomes 5, 5 stays 5, -5 becomes 5 )
			std::string SignRemoval(std::string input);
			
			//Removes the brackets from input (e.g. 54665(54) becomes 5466554 )
			std::string PeriodNotationRemoval(std::string input);

			//Removes the decimal separator from the input
			std::string DotRemoval(std::string input); 

			//Expands the digits in period into their full form (e.g. input 53 becomes 5353535353...)
			std::string PeriodExpander(std::string input, uint64_t precision);

			//Separates the input to an integer part and fractional part
			value DecimalSeparator(std::string input);
			
			//Sorts the fractional part of a number in either non-period notation or period notation (e.g. if fractional part is 369(555), the non-period notation will store 369, whereas period will store 555 )
			value PeriodSorter(std::string fractinoal_part);

			


		}

		namespace INP_CHECK {
			
			//Checks whether if the input is signed or unsigned (checks the literal suffix)
			bool IsUnsigned(std::string input);

			//Checks whether input is positive or negative ('+' = 0, '-' = 1) 
			bool IsANegative(std::string input, bool include_neg_zero = false);
			
			//Checks whether an input is an appropriate number for a given number system 
			bool IsANumber(std::string input, std::string num_sys);
			
			//Compares the absolute values of two whole inputs
			bool IsAbsGreater(std::string input, std::string threshold);
			
			//Checks whether does the input exceed the threshold or no. Supports only whole numbers
			//Both for the input and for the max_num
			bool IsGreaterThan(std::string input, std::string max_num);

			//Compares whether are two strings completely identical
			bool IsEqual(std::string input, std::string threshold);

			//Checks whether is the input smaller than the threshold
			bool IsLessThan(std::string input, std::string threshold);

			//Checks whether does an input contain floating point or not
			bool IsAFraction(std::string input);

			//Checks whether does a fractional input contain period or no
			bool IsPeriodic(std::string input);

			//Counts the number of leading zero digits in a given number
			uint64_t LeadingZeroCounter(std::string input);
		}

		//The parser of user input that checks the inputed number for its validity, whether does it exceed
		//limits, whats the sign of it and stores info about the number in properly
		value InpValueParser(std::string input, std::string num_sys, std::string min_int_num, std::string max_int_num, uint64_t max_frac_digit_amount = MAX_FRAC_DIGITS, bool remove_leading_zeroes = true, bool remove_trailing_zeros = true, bool include_neg_zero = false);

		//Converts any string based absolute whole input into an integer. 
		int_parser ToDec(std::string input);

		//Converts any string based fractional part to an ordinary fraction. (e.g. (3) -> 3/9)
		int_parser ToFraction(std::string fractional_input);
	}

	namespace DIAGNOSTICS {

		//General purpose error handler that will call a specific error based on the flag
		bool ErrorHandler(std::string input, uint16_t flag, std::string min_num, std::string max_num);

		//Any invalid input for a given converter function will result in this error
		void ConversionError(std::string input);

		//Any invalid command written in the console will result in this error 
		void InvalidCommandError(std::string input);

		//Any input that is greater or less than the value that the given function can support will result in this error
		void OverflowError(std::string min_num, std::string max_num);

		//Any input whose amount of fractional digits exceeds the given function's limit will result in this error
		void MantissaOverflowError(std::string max_frac_digit);

		

	}

}

#endif