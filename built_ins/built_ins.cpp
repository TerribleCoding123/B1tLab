#include <iostream>
#include <string>
#include <cstdint>
#include <vector>

#include "built_ins.h"
#include "view.h"

namespace BuiltIns {
	
	uint64_t power(uint64_t base, uint64_t exponent) {
		/*This is a simple power unsgined integer based power function
		with an absolute limit of UINT64_MAX
		*/

		
		uint64_t output{ 1 };

		for (int i = 0; i < exponent; i++) {
			output *= base;
		}

		return output;
	}

	Value decimalDivision(uint64_t numerator, uint64_t denominator, uint16_t fraction_precision) {
		/*This is string-based exact (no rounding error for fracitons) function for decimal division. It can output
		periodic fractions as well as full decimal fractions up to a given fraction_precision amount of digits */
		//This function becomes handy when we want to output the final result of our integer arithmetic calculations!
		
		Value output;
		
		//value_buffer is needed in order to check whether is a number periodic in a given fractional_precision or no
		std::vector<uint64_t> value_buffer{};

		//Ensures that the period part of a fraction is inserted into FRAC_period_val only once
		//Note! It also serves a purpose of an indicator of whether does the number contain period or no
		bool is_inserted{ false };

		//To account for division by zero
		if (denominator == 0) {
			
			
			output = Value{};
			output.flag = f_invalid_input;
			return output;
		}

		
		if (numerator == 0) {
			
			output = Value{};
			output.whole_part = '0';
			//Technically 0/5 is a fraction (numerator < denominator), hence we also append zero to the FRAC_digit_val
			output.frac_part = '0';
			return output;
		}
		
		//if the numerator == 0 (the remainder after the division = 0) the division has ended
		while (numerator != 0) {

			//We terminate calculations if the fraction_precision is exceeded, so the fractional part is non-infinite
			if (output.frac_part.length() >= fraction_precision) {
				
				output.flag = f_precision_loss;

				break;

			};

			if (numerator >= denominator) {
				
				//If at the current point of execution we are still in the whole part of the number -> append WHOLE_return_val
				//This statement is needed to calculate whole digits of an output
				if (output.is_a_frac == false) output.whole_part.append(std::to_string(numerator / denominator));

				else if (output.is_a_frac == true) {

					//Simply calculates the next fractional digit
					output.frac_part.append(std::to_string(numerator / denominator));

					//We increment the current_digit in order to properly count at what digit of a fractional part we currently are
					
				}
			}

			else if (numerator < denominator) {

				//If the numerator is less than the denimator, the division on line 81 will never occur
				//Thats why we have to manually add zero form these types of inputs
				if (output.whole_part.length() == 0) output.whole_part += '0';

				//if_a_float becomes true because we divide the number the numerator of which is less than the denominator
				output.is_a_frac = true;

				//The value_buffer.size() check is needed in order to avoid loop subscription on empty vectors (which will result in error)
				//is_inserted check is needed, so period digits of a fractional part are inserted into FRAC_period_val once, and not multiple times
				//(same logic with FRAC_nonperiod_val insertion)
				if (value_buffer.size() != 0 && is_inserted == false) {

					for (uint64_t index{ 0 }; index < value_buffer.size(); index++) {
						
						//Looks for any equal remainders, if found then it means in a given fraction_precision the number
						//appears periodic.
						if (value_buffer[index] == numerator) {
							
							//The characters up to the matching remainder (exclusive) are inserted into FRAC_nonperiod_val
							output.frac_nonperiod_part = output.frac_part.substr(0, index);
							
							//The characters from the matching remainder index up to the end of FRAC_digit_val are inserted into FRAC_period_val
							output.frac_period_part = output.frac_part.substr(index, output.frac_part.length() - index);
						
							output.frac_part = output.frac_nonperiod_part + '(' + output.frac_period_part + ')';

							return output;
						}
					}

				}

				//We feed the value_buffer vector with the current remainder (numerator) after iterating, because this exludes
				//the current remainder being pushed into the vector (which causes problems because the statement if(value_buffer[index] == numerator))
				//will always be true
				value_buffer.push_back(numerator);

				

				//numerator's value is increased tenfold, so the integer only division will always work 
				//(note that without this statement any fraction calculations will result in zero, because integer part of it is zero!)
				numerator *= 10;
				
				if (numerator < denominator) output.frac_part += '0';

				//Reset the whole loop until the numerator > denominator, so integer division will output meaningful results
				continue;
			}
			
			//We reset the numerator to the remainder, so we can continue dividing  
			numerator = numerator % denominator;

		}
		

		return output;

	}

	namespace InpHandling {

		UserInput getParsedInput(UserInput input) {

			input.unparsed_input = removeComments(input.unparsed_input);
			

			input.unparsed_input = BuiltIns::InpHandling::InpEdit::replaceNewline(input.unparsed_input);
			input.unparsed_input = BuiltIns::InpHandling::InpEdit::trailingSpaceRemoval(input.unparsed_input);



			//Old values are constantly overwritten
			input.tokens = BuiltIns::InpHandling::tokenizer(input.unparsed_input, input.tokens);
			return input;
		}

		std::string inputFetch(std::string input_line, uint16_t argument) {

			uint16_t word_counter{};
			bool is_fetched{};
			std::string output_string{};

			for (uint64_t index{ 0 }; index < input_line.length(); index++) {

				if (input_line[index] != ' ') {

					//Checking whether if is_fetched == false makes sure that we recognize a word by a single non-white
					//space character, We don't need to take into account any other non-white space characters that come
					//right after it. This way, the word_counter logic remains consistent
					if (is_fetched == false) {
						word_counter++;
						is_fetched = true;
					}
					
					if (word_counter == argument) {

						output_string += input_line[index];
					}

				}	

				if (input_line[index] == ' ') {

					is_fetched = false;
				}
				
			}
		
			return output_string;
		}

		std::string removeComments(std::string_view unparsed_string) {

			std::string output_string{};

			bool is_a_comment{};

			for (size_t index{ 0 }; index < unparsed_string.length(); index++) {

				if (unparsed_string[index] == ';') {
					
					is_a_comment = true;
				}


				if (is_a_comment) {

					//Comment delimeters are the newline or the end of a string! 
					if (unparsed_string[index] == '\n' || index == unparsed_string.length() - 1) {
						
						is_a_comment = false;
					}

					continue;
				}

				output_string += unparsed_string[index];
			}

			return output_string;
		}

		std::vector<std::string> tokenizer(std::string unparsed_string, std::vector<std::string> vector) {

			uint16_t index{ 1 };
			std::string token{};
			
			while (BuiltIns::InpHandling::inputFetch(unparsed_string, index).length() != 0) {

				token = BuiltIns::InpHandling::inputFetch(unparsed_string, index); 
				vector.push_back(token);
				index++;

			}

			return vector;
		}

		std::string getFirstToken(std::vector<std::string>& input_vector) {

			std::string output_token{};

			if (input_vector.size() == 0) {
				
				return output_token;
			}

			output_token = input_vector[0];
			
			input_vector.erase(input_vector.begin());
			
			return output_token;
		}

		namespace InpEdit {

			void inputFilter(std::string& input, [[maybe_unused]] int& cursor) {

				std::string output{};
				//because UTF-16, or UTF-32 characters are stored as multiple UTF-8 bytes in input, while converting, we only need a single '?'
				bool is_called_once{ true };

				

				for (signed char i : input) {

					if (i > 0 && i < 128) {

						output += i;
						is_called_once = true;

					}

					else {

						
						if (is_called_once == true) {

							output += '?';
							is_called_once = false;
						}
					}

				}

				input = output;

			}

			std::string removeSuffix(std::string input) {
				//Assumes suffix is a single character

				if (input.length() == 0) {
					
					return "";
				}
				
				input.erase(input.length() - 1, 1);

				return input;
			}

			std::string spaceRemoval(std::string input) {
				/*Copies all of the contents of input into output_string, only skipping the ' ' character*/


				std::string output_string{};

				for (char i : input) {

					if (i != ' ') {
						
						output_string += i;

					}
				}

				return output_string;
			}

			std::string replaceNewline(std::string input) {

				std::string output_str{};

				for (char i : input) {
					
					if (i != '\n') {
						output_str += i;
					}

					else {

						output_str += ' ';
					}
				}

				return output_str;
			}

			std::string trailingSpaceRemoval(std::string input) {

				while (input.length() != 0) {

					if (input.back() == ' ') {
						
						input.pop_back();
					}

					else {
						 
						break;
					}


				}

				return input;
			}

			std::string leadingZeroRemoval(std::string input) {
				/*Removes all of the leading zeros from the input string via .erase method, until no left
				if string becomes or initially was  zero length, the function will output '0'*/

				while (input[0] == '0') {

					input.erase(0, 1);
				}

				if (input.length() == 0) input += '0';

				return input;
			}

			std::string trailingZeroRemoval(std::string input) {
				/*Checks if the last character of input string is zero, if so, it removes it until no trailing 
				zeros are left. If the string becomes or innitally was zero length, it appends zero */

				while (input.length() > 0) {
					
					if (input.back() == '0') {
						
						input.pop_back();
					}

					else {

						break;
					}
				}

				if (input.length() == 0) input += '0';

				return input;

			}
			
			std::string signAppender(std::string input, bool sign) {
				//If the sign is 1 (aka true aka negative), it appends the minus

				if (sign == 1) {
					input.insert(0, 1, '-');
				}

				return input;
			}

			std::string signRemoval(std::string input) {
				/*Removes the zero character of input if it is '+' or '-'*/


				if (input[0] == '+' || input[0] == '-') {

					input.erase(0, 1);
				}

				return input;
			}

			std::string periodNotationRemoval(std::string input) {
				/*for() loop copies all of the data from input to output_string, however if '(' or 
				')' are encountered, the loop skips the copy of these via the continue statement.*/
				 

				std::string output_string{};

				for (uint64_t index{}; index < input.length(); index++) {

					if (input[index] == '(' || input[index] == ')') {


						continue;
					}

					output_string += input[index];
				}

				return output_string;
			}

			std::string dotRemoval(std::string input) {

				std::string output_string{};

				for (char i : input) {


					if (i != '.') {

						output_string += i;
					}
				}


				return output_string;
			}

			std::string periodExpander(std::string input, uint64_t precision) {

				std::string output_string{};

				//Guard against the empty input to prevent subsequent division by zero ( cycle % input.length() )
				if (input.length() == 0) {
					return output_string;
				}

				uint64_t cycle{};

				while (cycle < precision) {

					output_string += input[cycle % input.length()];

					cycle++;

				}

				return output_string;
			}

			Value decimalSeparator(std::string input) {
				/*This function separates the input into two strings: 
				WHOLE_return_val and FRAC_return_val
				*/

				Value output;

				for (char i : input) {
					
					

					if (output.is_a_frac != true) {
						
						//The fraction separator checking statement is put inside if (is_whole) statement to 
						//make it only work when the first dot is encountered (so inputs like 5..4 will be parsed like: 5, .4)
						if (i == '.') {

							output.is_a_frac = true;

							continue;
						}

						output.whole_part += i;
					}

					else {
						output.frac_part += i;
					}
					
					

				}
				
				//To properly account for inputs like 5.
				if (output.is_a_frac == true && output.frac_part.length() == 0) {

					output.frac_part += '0';
				}

				return output;
			}

			Value periodSorter(std::string fractional_part) {

				Value output;

				//Formats the input, so empty spaces wont slip in 
				fractional_part = spaceRemoval(fractional_part);

				//If the input is zero length, we error
				if (fractional_part.length() == 0) {
					
					output.flag = f_invalid_input;
					return output;
				}
				

				//This variable indicates whether are we inside of period or outside of period at the point of execution
				bool is_in_period{ false };

				//This variable indicates whether is a given fraciton periodic or not (e.g. for inputs like 0.0(56) it will be set to true)
				bool is_a_periodic_fraction{ false };

				for (uint64_t index{}; index < fractional_part.length(); index++) {

					
					//if the digits are not in period, and the current character is '(', we enter period
					if (is_in_period == false && fractional_part[index] == '(') {
						
						is_a_periodic_fraction = true;
						is_in_period = true;
						
						//Skips the '(', to prevent any logic errors (because '(' or ')' are not inside or outside of period)
						continue;
					}

					//Error handler: if the digits are already in period and the current character is '(' again, we raise an error
					else if (is_in_period == true && fractional_part[index] == '(') {

						output = Value{};
						output.flag = f_invalid_input;
						return output;
					}

					//If the digits are in period and current character is ')', we leave the period
					if (is_in_period == true && fractional_part[index] == ')') {
						
						//Error handler for empty periods -- 0.5() is recognized as invalid input 
						if (fractional_part[index - 1] == '(') {

							output = Value{};
							output.flag = f_invalid_input;
							return output;
						}

						is_in_period = false;
						
						//Skips the ')', to prevent any logic errors (because '(' or ')' are not inside or outside of period)
						continue;
					}

					//Error handler: If the current digit is not in period and the character is ')' -> raise error.
					else if (is_in_period == false && fractional_part[index] == ')') {
						
						output = Value{};
						output.flag = f_invalid_input;
						return output;
					}

					
					//Copies the periodic parts of a string and non-periodic parts into their desired places
					if (is_in_period == false) {

						output.frac_nonperiod_part += fractional_part[index];
					}
					else {
						
						output.frac_period_part += fractional_part[index];
					}
					
				}

				//This snippet is used to account for mathematically invalid cases like 0.0(56)565, the trailing digits after
				//the period are pointless because they will never occur, hence for such cases we raise an error.
				//It also accounts for weird inputs like (543 or ( 
				if (is_a_periodic_fraction == true && fractional_part.back() != ')') {
					
					output = Value{};
					output.flag = f_invalid_input;
					return output;
				}

				return output;
			}

			

		}
		
		namespace InpCheck {

			bool isUnsigned(std::string input) {

				if (input.length() == 0) {
					
					return false;
				}

				if (input.back() == 'u' || input.back() == 'U') {

					return true;
				}

				return false;
			}

			bool isNegative(std::string input, bool include_neg_zero) {
			//Note that empty strings are treated as 0, so "" and "-" are treated as  0 and -0 respectively 

				if (input[0] == '-') {

					input.erase(0, 1);

					for (char i : input) {

						//If all of the remaining characters of input were non-zero, this means that the input is negative 
						//For instance input like '-000000' is considered non-negative, whereas '-0000005' is negative
						if (i != '0') {

							return true;
						}
						
						//If the number is indeed -0, we can check the condition of the flag, and then return the appropriate
						//value
						else if (include_neg_zero == true) {
								
							return true;
						}
					}
					
					

					//If the input is only '-', we still return false, because it is treated as -0 (which is not negative)
					return false;
				}
			
				
				return false;

			}

			bool isNumber(std::string input, std::string num_sys) {
				/*This function takes each digit of input and checks its validity by comparing
				it with each number system's digit.*/


				bool is_a_number{ false };

				for (char input_digit : input) {
					

					is_a_number = false;

					for (char real_digit : num_sys) {

						
						if (input_digit == real_digit) {

							is_a_number = true;

							//We use break so  is_a_number in future iterations won't be set to false
							break;
						}

					}

					//If after comparing the given digit with all of the appropriate digit is_a_number is still false
					//This means that the input is not an appropriate number and we exit with false
					if (is_a_number == false) {

						return is_a_number;
					}
				}


				return is_a_number;
			} 

			bool isAbsGreater(std::string input, std::string threshold) {
				//This function works by comparing each input string's character's ASCII code point
				//With the treshold string's character's ASCII code point 
				
				//This function uses string based parameters and compares only the values of their characters, not only to eliminate
				//the problem of arbitrary numeral system comparison, but also to eliminate the issue of argument overflow or wrap-around
				//(which most likely will cause unexpected behavior)

				//Removes empty spaces, leading zeros and signs
				input = InpEdit::leadingZeroRemoval(InpEdit::signRemoval(InpEdit::spaceRemoval(input)));
				threshold = InpEdit::leadingZeroRemoval(InpEdit::signRemoval(InpEdit::spaceRemoval(threshold)));
				
				if (input.length() > threshold.length()) {

					return true;
				}

				else if (input.length() < threshold.length()) {

					return false;
				}

				
				else if (input.length() == threshold.length()) {

					for (uint64_t i{ 0 }; i < threshold.length(); i++) {

						//Comparing ASCII values 
						if (input[i] > threshold[i]) {

							return true;
						}
					}

					return false;
				}

				return false;



			}

			bool isFraction(std::string input) {
				
				for (char i : input) {
					
					if (i == '.')
						return true;
				}
				
				return false;
			}

			bool isEqual(std::string input, std::string threshold) {
				
				return input == threshold;
			}

			bool isGreaterThan(std::string input, std::string threshold) {

				//Signs
				bool inp_sign{ isNegative(input) };
				bool trs_sign{ isNegative(threshold) };


				//case 0: both inputs equal to each other
				if (isEqual(input, threshold) == true) {
					return false;
				}

				//case1: input is negative, treshold is positive
				if (inp_sign > trs_sign) {
					return false;
				}

				//case 2: input is positive, treshold is negative
				else if (inp_sign < trs_sign) {
					return true;
				}

				//case 3: input is positive, threshold is positive (magnitude comparison)
				else if (inp_sign == 0 && trs_sign == 0) {

					return isAbsGreater(input, threshold);
				}

				//case 4: input is negative, threshold is negative (magnitude comparison)
				else if (inp_sign == 1 && trs_sign == 1) {
					
					return !isAbsGreater(input, threshold);
				}
				

				return false;
				
			}

			bool isLessThan(std::string input, std::string threshold) {
				
				//If they inputs are still equal in length, we return false
				if (isEqual(input, threshold) == true) {
					return false;
				}
				
				//Its just the reverse of IsGreaterThan (I mean...come on its logical!)
				return !isGreaterThan(input, threshold);
			}

			bool isPeriodic(std::string input) {
				//Simply checks whether does an input contain the '(' character or no

				for (char i : input) {
					
					if (i == '(') {
						
						return true;
					}
				}

				return false;
			}
			
			uint64_t leadingZeroCounter(std::string input) {
				
				uint64_t leading_zeros{};
				
				for (char i : input) {
					
					if (i == '0') leading_zeros++;
					
					else break;
				}

				return leading_zeros;
			}
		}

		Value valueParser(std::string input, std::string num_sys, std::string min_int_num, std::string max_int_num, uint64_t max_frac_digit_amount, bool remove_leading_zeroes, bool remove_trailing_zeros, bool include_neg_zero) {

			Value output;

			
			//Formating + sign check
			input = InpEdit::spaceRemoval(input);

			if (input[0] == '+') {

				output.has_explicit_plus = true;
			}
			output.sign = InpCheck::isNegative(input, include_neg_zero);
			input = InpEdit::signRemoval(input);

			//'.' is treated as 0.0 which is wrong
			if (input.length() == 0 || input == ".") {
			
				output = Value{};
				output.flag = f_invalid_input;
				return output;
			}


			output.whole_part = InpEdit::decimalSeparator(input).whole_part;

			if (remove_leading_zeroes) {
				output.whole_part = InpEdit::leadingZeroRemoval(output.whole_part);
			}

			//For performance and maintability reasons, we format the input removing both the
			//Decimal separator and the period notation and only then check the remaining digits for their validity
			if (InpCheck::isNumber(output.whole_part, num_sys) == false) {

				//Reset the output and set the error flag
				output = Value{};
				output.flag = f_invalid_input;

				return output;
			}


			if (InpCheck::isFraction(input) == true) {



				output.is_a_frac = true;

				
				output.frac_part = InpEdit::decimalSeparator(input).frac_part;
				
				

				//For inputs like '5.' or '0.'
				if (output.frac_part.length() == 0) {
					output.frac_part += '0';
				}

				

				//Fractional part and integer part are checked separately because integer part does not contain period
				//whereas fractional part does. Without the split either (6).5 becomes valid, or period notation won't be valid whatsoever
				if (InpCheck::isNumber(InpEdit::periodNotationRemoval(output.frac_part), num_sys) == false) {

					//Reset the output and set the error flag
					output = Value{};
					output.flag = f_invalid_input;

					return output;
				}

				//Note that here we dont need to check whether do we have zeros after the period or no, we just remove them
				//Because writing something like 5.(36)0000 albeit being redundant, is sitll valid!
				if (remove_trailing_zeros) {
					
					
					output.frac_part = InpEdit::trailingZeroRemoval(output.frac_part);

				}

				//For performance and maintabilitiy  reasons, we check the length of a fractional part prior
				//to any operations
				if (InpEdit::periodNotationRemoval(output.frac_part).length() > max_frac_digit_amount) {

					output = Value{};
					output.flag = f_mantissa_overflow;
					return output;
				}


				if (InpCheck::isPeriodic(output.frac_part) == true) {
					
					//Error for wrong period notation
					if (InpEdit::periodSorter(output.frac_part).flag == f_invalid_input) {

						output = Value{};
						output.flag = f_invalid_input;
						return output;
					}


					//Secondary split of FRAC_return_val into nonperiodic part and periodic
					output.frac_nonperiod_part = InpEdit::periodSorter(output.frac_part).frac_nonperiod_part;
					output.frac_period_part = InpEdit::periodSorter(output.frac_part).frac_period_part;
				}
				
				

				
				if (InpCheck::isPeriodic(output.frac_part) == false) {
					output.frac_nonperiod_part = output.frac_part;
				}
				
				
			}

			//Checks the limit
			//Built-in limit ignorance
			if (min_int_num != "ignore" || max_int_num != "ignore") {

				
				std::string leading_minus{};
				
				//We only need minus acount because +5 = 5 by default
				if (output.sign == 1) {
					
					leading_minus += '-';
				}

				if (InpCheck::isLessThan(leading_minus +  output.whole_part, min_int_num) == true || InpCheck::isGreaterThan( leading_minus + output.whole_part, max_int_num) == true) {
					
					output = Value{};
					output.flag = f_out_of_bounds;
					return output;
				}
			}

			return output;

		}

		IntParser toDec(std::string input) {

			IntParser output;
		

			//Checks whether is the input written in decimal
			if (BuiltIns::InpHandling::InpCheck::isNumber(input, kDecimalSys) == false) {
				

				output = IntParser{};
				output.flag = f_invalid_input;
				return output;
			}

			//Checks whether does the input exceed the max value, because using overly
			//large string inputs that exceed UINT64_MAX will result in wrap-around
			if (BuiltIns::InpHandling::InpCheck::isAbsGreater(input, kMaxDec) == true) {

				output = IntParser{};
				output.flag = f_out_of_bounds;
				return output;
			}

			uint64_t return_value{};
			uint64_t length{ input.length() };

			//The return_value is calculated by properly adding digits with different placement in the number.
			//Consider the input "1234": The program, based on the significance of the digit, will add up numbers like this:
			// 1 * 10^3 + 2 * 10^2 + 3 * 10^1 + 4 * 10^0
			for (char i : input) {

				length--;
				// i - '0' converts the ASCII character into a proper integer digit by ASCII index subtraction 
				// For instance, let i be '1'. Its ASCII value is 31, and the ASCII value of '0' is 30, hence 31 - 30 = 1
				return_value += (i - '0') * power(10, length);
			}

			output.numerator = return_value;

			return output;
		}

		IntParser toRational(std::string fractional_input) {
			/*This is a decimal fraction to ordinary fraction converter. It supports both repeating and non-repeating
			* sequences. Let's see how it works! 
			* 
			* Let's start with the following fractional input input 183(679) (full form: 0.183(679)).
			* We can expand this to the following sequene: 0.18(679) = 0.18679679679679679679679679679679679... .
			* Knowing, that the order of a digit in fraction corresonds to the exponent of 10 in the denominator when 
			* represented as a standard fraction (e.g 0.36, 3 is first after the decimal separator, 6 is second, hence, 
			* 0.36 = 3/10^(1) + 6/10^(2)), we can rewrite our example input like this: 0.18679679679679679679679679... = 
			* 1/10^(1) + 8/10^(2) + 6/10^(3) + 7/10^(4) + 9/10^(5)... . We can merge a single digit numerator fractions into 
			* bigger chunks to separate between non-periodic digits and once, ecnlosed in period notation, here, we get:
			* 0.18679679679679679679679679... = 18/10^(2) + 679/10^(5) + 679/10^(8) + 679/10^(11) + 679/10^(14)... . To get 
			* a make things, let's use a table where, at the bottom we have a fracional digit, and at the top corresponding index
			* 
 			* Index:   | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | ... |
			* Digit:   | 1 | 8 | 6 | 7 | 9 | 6 | 7 | 9 | 6 | 7  | 9  | 6  | 7  | 9  | 6  | 7  | 9  | 6  | 7  | 9  | ... |
			* 
			* We can see a clear picture, that in order to group digits into chuncks, we need to divide them by the 10 raised
			* to index of the least significand digit. So, to get 0.18 in fraction, for instance, we need to divide 18 by 10 
			* raised to the index of 8 (which is two), hence, getting us 18/10^(2). 
			* 
			* Let's view our periodic decimal fraction, as an infinite sum of fractions again:
			* 0.18679679679... = 18/10^(2) + 679/10^(5) + 679/10^(8) + 679/10^(11) + 679/10^(14)...
			* We can see that each next fraction's denominator's exponent in the periodic fractions is raised by a constant value 
			* (which is the amount of digits in period). Exponents are arranged into an arithmetic progressions, where the start 
			* is the amount of digits that are not in period, and the commond difference, is the amount of digits that are in the period. 
			* Now, we can rewrite our previos fractional sequence, with the help of sigma notation: 
			* 0.18679679679... = 18/10^(2)  + (sigma)(lower: n = 1)(upper: inf)(679/10^(2 + 3n)), where 
			* 2 (in the 679/10^(2 + 3n) part) is the amount of digits, that are not in period, while 3 is the amount of digits that are in period.
			* 
			* Let's write a more general form for it: 
			* Let B be digits that are not in period (so for 0.36, B = 36; 0.01 B = 1),
			* Let C be digits that are in periodic the part (so for 0.(36), C = 36; 0. )
			* Let k be the amount of digits in B (leading zeros included, so for B = 001, k = 3)
			* Let m be the amount of digits in C (leading zeros included, so for C = 01, m = 2)
			* 
			* Then, our fraction of the form 0.B(C) = B*10^(m)/10^(k+m) + (sigma)(lower: n = 1)(upper: inf)(C/10^(k+mn)),
			* when fully expanded into a finite series, it will give us the following: 
			* 0.B(C) = ( B * ( 10^(m) - 1 ) + C )/( 10^(k) * (10^(m) - 1 )).
			* 
			*/

			Value parsed_input;
			IntParser output;

			
			parsed_input = valueParser("0." + fractional_input, kDecimalSys, "0", kMaxDec);
											
			//Error Handling							
			if (parsed_input.flag != 0) {

				output = IntParser{};
				output.flag = parsed_input.flag;
				
				return output;
			}
				
			uint64_t nonperiod_digits{ BuiltIns::InpHandling::toDec(parsed_input.frac_nonperiod_part).numerator };
			uint64_t period_digits{ BuiltIns::InpHandling::toDec(parsed_input.frac_period_part).numerator };

			uint64_t nonperiod_length{ parsed_input.frac_nonperiod_part.length() };
			uint64_t period_length{ parsed_input.frac_period_part.length() };
	
			//To prevent division by zero
			if (parsed_input.frac_period_part.length() == 0) {
				
				period_length++;
			}

			output.numerator = nonperiod_digits * (BuiltIns::power(10, period_length) - 1) + period_digits;
			output.denominator = BuiltIns::power(10, nonperiod_length) * (BuiltIns::power(10, period_length) - 1);
			


			return output;
		}

	}

	namespace Diagnostics {

		bool errorHandler(std::string input, uint16_t flag, std::string min_num, std::string max_num) {

			if (flag == f_invalid_input) {
				conversionError(input);
				return true;
			}

			else if (flag == f_out_of_bounds) {
				overflowError(min_num, max_num);
				return true;
			}

			else if (flag == f_mantissa_overflow) {
				mantissaOverflowError(std::to_string(kMaxMantissa));
				return true;
			}

			return false;
		}

		void conversionError(std::string input) {
			
			std::string error_red{ View::Utils::rgb666ToString(View::kErrorRed[0], View::kErrorRed[1], View::kErrorRed[2]) };
			std::cout << View::kItalic << error_red << "'" << View::kReset << View::kResetItalic;
			std::cout << input;
			std::cout << View::kItalic << error_red << "' isn't recognized as a valid input. \a" << View::kReset << View::kResetItalic;
			
			
		}

		void invalidCommandError(std::string input) {

			std::string error_red{ View::Utils::rgb666ToString(View::kErrorRed[0], View::kErrorRed[1], View::kErrorRed[2]) };
			std::cout << View::kItalic << error_red << "The term '" << View::kReset << View::kResetItalic;
			std::cout << input;
			std::cout << View::kItalic << error_red << "' is not recognized as a valid command or a valid input. \a \n\n" << View::kReset << View::kResetItalic;
			
		}

		void overflowError(std::string min_num, std::string max_num) {
			
			std::cout << View::Utils::rgb666ToString(View::kErrorRed[0], View::kErrorRed[1], View::kErrorRed[2]);
			std::cout << View::kItalic;
			std::cout << "Input is out of bounds (convertable value should be greater or equal than " << min_num << " or less or equal than " << max_num << "). \a";
			std::cout << View::kReset << View::kResetItalic;
		
		}

		void mantissaOverflowError(std::string max_frac_digit){
			
			std::cout << View::Utils::rgb666ToString(View::kErrorRed[0], View::kErrorRed[1], View::kErrorRed[2]);
			std::cout << View::kItalic;
			std::cout << "Input has too many digits in fractional part (max. convertable amount is " << max_frac_digit << " digits). \a";
			std::cout << View::kResetItalic;
			std::cout << View::kReset;
		
		}

		
	}
}