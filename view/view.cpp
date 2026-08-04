#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

#include "replxx.hxx"

#include "view.h"
#include "built_ins.h"
#include "binary.h"



namespace VIEW {



    namespace WINDOW {

       

    }
    

    namespace UTILS {

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

        replxx::Replxx::colors_t& MvalueHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, replxx::Replxx::Color desired_color) {

           
            std::string casted_char{};

            //Suffix logic
            bool is_a_last_digit{ false };

            for (size_t index{ 0 }; index < input_buffer.length(); index++) {

                casted_char = input_buffer[index];

                if (BUILT_INS::INP_HANDLING::INP_CHECK::IsANumber(casted_char, DECIMAL_SYS) == true) {

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
        replxx::Replxx::colors_t& RvalueHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, replxx::Replxx::Color desired_color) {

            
            std::vector<std::string> tokens{};
            tokens = BUILT_INS::INP_HANDLING::Tokenizer(input_buffer, tokens);
           
            size_t index{};
            size_t offset{};
            
            //rValue can only be invoked in SBIN_TO_DEC conversion!
            bool is_rValue_possible{ false };


            for (std::string token : tokens) {
                
                token = BUILT_INS::INP_HANDLING::INP_EDIT::ReplaceNewline(token);
                token = BUILT_INS::INP_HANDLING::INP_EDIT::SpaceRemoval(token);

                if (token == "SBIN_TO_DEC") {

                    is_rValue_possible = true;
                }

                if (BINARY_CONVERSION::UTILS::isRawBin(token, BINARY_CONVERSION::FORMAT::INT::bit_size_presets) == true && BUILT_INS::INP_HANDLING::INP_CHECK::IsANumber(token, BINARY_SYS) == true && is_rValue_possible) {

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
            replxx::Replxx::Color mval_col = replxx::color::rgb666(mvalue_green[0], mvalue_green[1], mvalue_green[2]);
            color_buffer = VIEW::UTILS::MvalueHighlighter(input_buffer, color_buffer, mval_col);

#ifdef BINARY_10_29_2025        
            //rVal highlighting (overwrites mVal)
            replxx::Replxx::Color rval_col = replxx::color::grayscale(rvalue_white);
            color_buffer = VIEW::UTILS::RvalueHighlighter(input_buffer, color_buffer, rval_col);
#endif
            //Utility commands
            replxx::Replxx::Color command_col = replxx::color::rgb666(command_purple[0], command_purple[1], command_purple[2]);
            for (std::string_view keyword : BUILT_INS::commands) {

                color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, keyword, command_col);

            }

            //Conversion commands
            replxx::Replxx::Color mconversion_col = replxx::color::rgb666(mconv_yellow[0], mconv_yellow[1], mconv_yellow[2]);
            replxx::Replxx::Color sconversion_col = replxx::color::rgb666(sconv_yellow[0], sconv_yellow[1], sconv_yellow[2]);
            for (std::string_view keyword : BINARY_CONVERSION::conversion_commmands) {

                if (keyword == BINARY_CONVERSION::conversion_commmands[0] || keyword == BINARY_CONVERSION::conversion_commmands[1]) {

                    color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, keyword, mconversion_col);
                }

                else {

                    color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, keyword, sconversion_col);
                }
            }



            //Flags
            replxx::Replxx::Color mode_flag_col = replxx::color::grayscale(flag_gray);
            for (std::string_view keyword : BINARY_CONVERSION::STANDARD_CONVERSION::flag_mode) {

                color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, keyword, mode_flag_col);

            }
            for (std::string_view keyword : BINARY_CONVERSION::FORMAT::INT::resize_flags) {

                color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, keyword, mode_flag_col);
            }

            //Formats
            replxx::Replxx::Color format_col = replxx::color::rgb666(format_blue[0], format_blue[1], format_blue[2]);
            for (std::string_view keyword : BINARY_CONVERSION::int_formats) {

                //Unsigned shouldn't be recognized as a valid format to enter
                if (keyword == BINARY_CONVERSION::int_formats[0]) {

                    continue;
                }

                color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, keyword, format_col);

            }


            //color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, "SDEC_TO_BIN", red);
            //color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, "SBIN_TO_DEC", red);
            //color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, "DEC_TO_BIN", replxx::Replxx::Color::BRIGHTMAGENTA);
            //color_buffer = VIEW::UTILS::keywordHighlighter(input_buffer, color_buffer, "BIN_TO_DEC", replxx::Replxx::Color::BRIGHTMAGENTA);

            //Comments
            replxx::Replxx::Color comment_col = replxx::color::rgb666(comment_green[0], comment_green[1], comment_green[2]);
            color_buffer = VIEW::UTILS::commentHighlighter(input_buffer, color_buffer, comment_col);


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



