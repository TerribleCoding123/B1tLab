#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

#include "replxx.hxx"

#include "view.h"
#include "built_ins.h"
#include "binary.h"



namespace View {



    namespace Window {

    }
    

    namespace Utils {

        replxx::Replxx::colors_t& keywordHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, std::string_view keyword, replxx::Replxx::Color desired_color) {

            size_t index{};
            size_t offset{};

            //std::string ignore_keywords
           
            while (true) {

                index = input_buffer.find(keyword, index);

                if (index == input_buffer.npos) {

                    break;
                }
                
              
                   
                //Checking if the previous character is any of the unprintabel ASCII characters or if it is a comment
                if (index != 0 &&  input_buffer[index - 1] > 32 && input_buffer[index -1 ] != ';') {
                
                    index++;
                    continue;
                }
                
                //Checking if the previous character is any of the unprintabel ASCII characters or if it is a comment
                if (index + keyword.length() < input_buffer.length() && input_buffer[index + keyword.length()] > 32 && input_buffer[index + keyword.length()] != ';') {

                    index++;
                    continue;
                }

                //Coloring
                while (offset < keyword.length()) {

                    color_buffer[index + offset] = desired_color;
                    offset++;
                    
                }
                

                index++;
                offset = 0;
                    
                

            }

             return color_buffer;
        }

        replxx::Replxx::colors_t& commentHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, replxx::Replxx::Color desired_color) {
            //Comments are triggered by ;

            bool is_a_comment{ false };

            for (size_t index{ 0 }; index < input_buffer.length(); index++) {

                if (input_buffer[index] == ';') {
                    
                    is_a_comment = true;
                }

                if (is_a_comment) {

                    color_buffer[index] = desired_color;


                    if (input_buffer[index] == '\n' || index == input_buffer.length() - 1) {

                        is_a_comment = false;
                        
                    }

                    
                }
            }
            return color_buffer;
        }

        replxx::Replxx::colors_t& mValueHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, replxx::Replxx::Color desired_color) {

           
            std::string casted_char{};

            //Suffix logic
            bool is_a_last_digit{ false };

            for (size_t index{ 0 }; index < input_buffer.length(); index++) {

                casted_char = input_buffer[index];

                if (BuiltIns::InpHandling::InpCheck::isNumber(casted_char, BuiltIns::kDecimalSys) == true) {

                    color_buffer[index] = desired_color;
                    
                    is_a_last_digit = true;
                }
                
                else {
                    
                   
                    if ((input_buffer[index] == 'u' || input_buffer[index] == 'U') && is_a_last_digit == true) {
                       
                        
                        color_buffer[index] = desired_color;
                    }

                    is_a_last_digit = false;
                }
               
            }

            return color_buffer;
            
        }

        std::string rgb666ToString(int r, int g, int b) {

            if ((r >= 0 && r <= 5) && (g >= 0 && g <= 5) && (b >= 0 && b <= 5)) {

                int index{ 16 + 36 * r + 6 * g + b };
                return "\033[38;5;" + std::to_string(index) + "m";

            }

            return "";
        }

#ifdef BINARY_10_29_2025
        replxx::Replxx::colors_t& rValueHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, replxx::Replxx::Color desired_color) {

            
            std::vector<std::string> tokens{};
            tokens = BuiltIns::InpHandling::tokenizer(input_buffer, tokens);
           
            size_t index{};
            size_t offset{};
            
            //rValue can only be invoked in SBIN_TO_DEC conversion!
            bool is_rValue_possible{ false };


            for (std::string token : tokens) {
                
                token = BuiltIns::InpHandling::InpEdit::replaceNewline(token);
                token = BuiltIns::InpHandling::InpEdit::spaceRemoval(token);

                if (token == "SBIN_TO_DEC") {

                    is_rValue_possible = true;
                }

                if (BinaryConversion::Utils::isRawBin(token, BinaryConversion::Format::Int::bit_size_presets) == true && BuiltIns::InpHandling::InpCheck::isNumber(token, BuiltIns::kBinarySys) == true && is_rValue_possible) {

                    index = input_buffer.find(token, index);

                    if (index != input_buffer.npos) {

                        while (offset < token.length()) {
                            
                            color_buffer[index + offset] = desired_color;
                            offset++;
                        }
                        
                        offset = 0;
                        //Ensures that even if there are two identical rValues, both will be highlighted
                        index++;
                        is_rValue_possible = false;

                    }
                    
                    else {
                        
                        //No more rValues detected
                        return color_buffer;
                    }
                    
                }
                
               
            
            }
            

            return color_buffer;
        }

#endif

        void inputHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer) {

          
            //mVal higlighting
            replxx::Replxx::Color mval_col = replxx::color::rgb666(kLiteralGreen[0], kLiteralGreen[1], kLiteralGreen[2]);
            color_buffer = View::Utils::mValueHighlighter(input_buffer, color_buffer, mval_col);

#ifdef BINARY_10_29_2025        
            //rVal highlighting (overwrites mVal)
            replxx::Replxx::Color rval_col = replxx::color::grayscale(kLiteralWhite);
            color_buffer = View::Utils::rValueHighlighter(input_buffer, color_buffer, rval_col);
#endif
            //Utility commands
            replxx::Replxx::Color command_col = replxx::color::rgb666(kCommandPurple[0], kCommandPurple[1], kCommandPurple[2]);
            for (std::string_view keyword : BuiltIns::commands) {

                color_buffer = View::Utils::keywordHighlighter(input_buffer, color_buffer, keyword, command_col);

            }

            //Conversion commands
            replxx::Replxx::Color mconversion_col = replxx::color::rgb666(kDefaultYellow[0], kDefaultYellow[1], kDefaultYellow[2]);
            replxx::Replxx::Color sconversion_col = replxx::color::rgb666(kStandardYellow[0], kStandardYellow[1], kStandardYellow[2]);
            for (std::string_view keyword : BinaryConversion::conversion_commmands) {

                if (keyword == BinaryConversion::conversion_commmands[0] || keyword == BinaryConversion::conversion_commmands[1]) {

                    color_buffer = View::Utils::keywordHighlighter(input_buffer, color_buffer, keyword, mconversion_col);
                }

                else {

                    color_buffer = View::Utils::keywordHighlighter(input_buffer, color_buffer, keyword, sconversion_col);
                }
            }



            //Flags
            replxx::Replxx::Color mode_flag_col = replxx::color::grayscale(kFlagGray);
            for (std::string_view keyword : BinaryConversion::StandardConversion::flag_mode) {

                color_buffer = View::Utils::keywordHighlighter(input_buffer, color_buffer, keyword, mode_flag_col);

            }
            for (std::string_view keyword : BinaryConversion::Format::Int::resize_flags) {

                color_buffer = View::Utils::keywordHighlighter(input_buffer, color_buffer, keyword, mode_flag_col);
            }

            //Formats
            replxx::Replxx::Color format_col = replxx::color::rgb666(kFormatBlue[0], kFormatBlue[1], kFormatBlue[2]);
            for (std::string_view keyword : BinaryConversion::int_formats) {

                //Unsigned shouldn't be recognized as a valid format to enter
                if (keyword == BinaryConversion::int_formats[0]) {

                    continue;
                }

                color_buffer = View::Utils::keywordHighlighter(input_buffer, color_buffer, keyword, format_col);

            }


            //color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, "SDEC_TO_BIN", red);
            //color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, "SBIN_TO_DEC", red);
            //color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, "DEC_TO_BIN", replxx::Replxx::Color::BRIGHTMAGENTA);
            //color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, "BIN_TO_DEC", replxx::Replxx::Color::BRIGHTMAGENTA);

            //Comments
            replxx::Replxx::Color comment_col = replxx::color::rgb666(kCommentGreen[0], kCommentGreen[1], kCommentGreen[2]);
            color_buffer = View::Utils::commentHighlighter(input_buffer, color_buffer, comment_col);


        }

        

        void clear() {

            std::cout << "\033c" << std::flush;
        }

        void draw(std::string_view content) {

            std::cout << content << '\n';
        }

        void update(std::string_view new_content) {
            
            clear();
            draw(new_content);
        }

        void setCursor(uint64_t pos_x, uint64_t pos_y) {

            std::cout << "\033[" << pos_y << ";" << pos_x << "H";
        }

        void textPaint(uint16_t red, uint16_t green, uint16_t blue) {

            std::cout << "\x1b[38;2;" << red << ";" << green << ";" << blue << "m";
        }
    }


}



