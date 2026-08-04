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

void CommandCheck(BUILT_INS::user_input &input) {

	std::string command{BUILT_INS::INP_HANDLING::GetFirstToken(input.tokens)};
	
	if (command == "exit") {

		input.flag = F_EXIT;
	}

	

#ifdef BINARY_10_29_2025


	
	else if (command == "DEC_TO_BIN") {
		
		
		std::string pink{ VIEW::UTILS::rgb666ToString(VIEW::return_pink[0],VIEW::return_pink[1],VIEW::return_pink[2]) };
		std::cout << pink << BINARY_CONVERSION::DEFAULT_CONVERSION::DEC_TO_BIN(input.tokens) << VIEW::reset << "\n\n";
		
	}

	else if (command == "BIN_TO_DEC") {

		
		std::string pink{ VIEW::UTILS::rgb666ToString(VIEW::return_pink[0],VIEW::return_pink[1],VIEW::return_pink[2]) };
		std::cout << pink << BINARY_CONVERSION::DEFAULT_CONVERSION::BIN_TO_DEC(input.tokens) << VIEW::reset << "\n\n";

	}

	else if (command == "SDEC_TO_BIN") {
		
		std::string output{ BINARY_CONVERSION::STANDARD_CONVERSION::STANDARD_CONVERTER(input.tokens, DecToBin) };
		
		std::string pink{ VIEW::UTILS::rgb666ToString(VIEW::return_pink[0],VIEW::return_pink[1],VIEW::return_pink[2]) };
		std::cout <<  pink <<  output << VIEW::reset << "\n\n";
	}
	
	else if (command == "SBIN_TO_DEC") {
		
		std::string output{ BINARY_CONVERSION::STANDARD_CONVERSION::STANDARD_CONVERTER(input.tokens, BinToDec) };

		std::string pink{ VIEW::UTILS::rgb666ToString(VIEW::return_pink[0],VIEW::return_pink[1],VIEW::return_pink[2]) };
		std::cout << pink << output << VIEW::reset << "\n\n";

	}
	
#endif

	else {
		
		input.flag = F_INVALID_INP;
		input.invalid_token = command;
	}

	return;
	
}

int main(){

	
	
	BUILT_INS::user_input input;

	replxx::Replxx terminal;
	
	terminal.set_modify_callback(&BUILT_INS::INP_HANDLING::INP_EDIT::utf_filter);
	terminal.set_highlighter_callback(&VIEW::UTILS::inputHighlighter);
	
	
	
	while (input.flag != F_EXIT) {


		if (input.flag == F_INVALID_INP) {

			BUILT_INS::DIAGNOSTICS::InvalidCommandError(input.invalid_token);
			//flushing all invalid data
			input = BUILT_INS::user_input{};
		}

		//If the input vector is empty, only then we ask for more input
		if (input.tokens.empty()) {
			input.unparsed_input = terminal.input("");
			input = BUILT_INS::INP_HANDLING::GetParsedInput(input);
		}
		
		//If user typed nothing, we shall still ask for input
		if (input.unparsed_input.length() == 0) continue;
		
		
		CommandCheck(input);
		
	}

	return 0;
}