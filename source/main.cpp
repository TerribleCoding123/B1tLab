//External
#include <iostream>
#include <string>
#include <string_view>
#include <cstdint>
#include <vector>
#include "replxx.hxx"

//Internal
#include "built_ins.h" 
#include "view.h"

//Conversion
#include "binary.h" // For binary-conversion operations

/*
To do: 

Fix syntax highlighting with comments (SDEC_TO_BIN; should be lit up!)
*/

void commandCheck(BuiltIns::UserInput &input) {

	std::string command{BuiltIns::InpHandling::getFirstToken(input.tokens)};
	
	if (command == "exit") {

		input.flag = BuiltIns::f_exit;
	}

	

#ifdef BINARY_10_29_2025


	
	else if (command == "DEC_TO_BIN") {
		
		
		std::string pink{ View::Utils::rgb666ToString(View::kReturnPink[0],View::kReturnPink[1],View::kReturnPink[2]) };
		std::cout << pink << BinaryConversion::DefaultConversion::decToBin(input.tokens) << View::kReset << "\n\n";
		
	}

	else if (command == "BIN_TO_DEC") {

		
		std::string pink{ View::Utils::rgb666ToString(View::kReturnPink[0],View::kReturnPink[1],View::kReturnPink[2]) };
		std::cout << pink << BinaryConversion::DefaultConversion::binToDec(input.tokens) << View::kReset << "\n\n";

	}

	else if (command == "SDEC_TO_BIN") {
		
		std::string output{ BinaryConversion::StandardConversion::standardConverter(input.tokens, BinaryConversion::dec_to_bin) };
		
		std::string pink{ View::Utils::rgb666ToString(View::kReturnPink[0],View::kReturnPink[1],View::kReturnPink[2]) };
		std::cout <<  pink <<  output << View::kReset << "\n\n";
	}
	
	else if (command == "SBIN_TO_DEC") {
		
		std::string output{ BinaryConversion::StandardConversion::standardConverter(input.tokens, BinaryConversion::bin_to_dec) };

		std::string pink{ View::Utils::rgb666ToString(View::kReturnPink[0],View::kReturnPink[1],View::kReturnPink[2]) };
		std::cout << pink << output << View::kReset << "\n\n";

	}
	
#endif

	else {
		
		input.flag = BuiltIns::f_invalid_input;
		input.invalid_token = command;
	}

	return;
	
}

int main(){


	BuiltIns::UserInput input;

	replxx::Replxx terminal;
	
	terminal.set_modify_callback(&BuiltIns::InpHandling::InpEdit::inputFilter);
	terminal.set_highlighter_callback(&View::Utils::inputHighlighter);
	
	
	
	while (input.flag != BuiltIns::f_exit) {


		if (input.flag == BuiltIns::f_invalid_input) {

			BuiltIns::Diagnostics::invalidCommandError(input.invalid_token);
			//flushing all invalid data
			input = BuiltIns::UserInput{};
		}

		//If the input vector is empty, only then we ask for more input
		if (input.tokens.empty()) {
			input.unparsed_input = terminal.input("");
			input = BuiltIns::InpHandling::getParsedInput(input);
		}
		
		//If user typed nothing, we shall still ask for input
		if (input.unparsed_input.length() == 0) continue;
		
		
		commandCheck(input);
		
	}

	return 0;
}