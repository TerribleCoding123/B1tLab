#pragma once

#define VIEW_5_8_2026


#include "replxx.hxx"
#include "binary.h"

#include <array>

namespace View {

	//colors
	constexpr inline std::string_view kReset { "\033[0m" };
	//Diagnosic messages
	constexpr inline std::array kErrorRed = {5,1,1};
	constexpr inline std::array kWarningYellow = { 5,4,1 };
	constexpr inline std::array kReturnPink = { 4,2,5 };

	//Style 
	constexpr inline  std::string_view kItalic{ "\033[3m" };
	constexpr inline  std::string_view kResetItalic{ "\033[23m" };

	//Highlighting
	constexpr inline std::array kDefaultYellow = { 5, 5, 3 };
	constexpr inline std::array kStandardYellow = { 5, 4, 3 };
	constexpr inline std::array kCommentGreen = { 1, 3, 1 };
	constexpr inline std::array kLiteralGreen = { 3, 4, 3 };
	constexpr inline std::array kCommandPurple = { 3, 3, 5 };
	constexpr inline std::array kFormatBlue = { 2, 4, 5 };
	constexpr inline int kLiteralWhite = 23;
	constexpr inline int kFlagGray = 14;
  	



	namespace Utils {
		
		replxx::Replxx::colors_t& keywordHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, std::string_view keyword, replxx::Replxx::Color desired_color);

		replxx::Replxx::colors_t& commentHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, replxx::Replxx::Color desired_color);
		
		replxx::Replxx::colors_t& mValueHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, replxx::Replxx::Color desired_color);

		std::string rgb666ToString(int r, int g, int b);

#ifdef BINARY_10_29_2025
		
		replxx::Replxx::colors_t& rValueHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, replxx::Replxx::Color desired_color);

#endif

		void inputHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer);

		void draw(std::string_view content);

		void clear();

		void update(std::string_view new_content);

		void setCursor(uint64_t pos_x, uint64_t pos_y);

		void textPaint(uint16_t red, uint16_t green, uint16_t blue);
	}

	
}


