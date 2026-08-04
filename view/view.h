#ifndef VIEW_7_15_2026 
#define VIEW_7_15_2026

#include "replxx.hxx"
#include "binary.h"
namespace VIEW {

	//colors
	inline std::string_view reset { "\033[0m" };
	//Diagnosic messages
	inline int error_red[3] = {5,1,1};
	inline int warning_yellow[3] = { 5,4,1 };
	inline int return_pink[3] = { 4,2,5 };

	inline std::string_view italic{ "\033[3m" };
	inline std::string_view reset_italic{ "\033[23m" };

	//Highlighting
	inline int mconv_yellow[3] = { 5, 5, 3 };
	inline int sconv_yellow[3] = { 5, 4, 3 };
	inline int comment_green[3] = { 1, 3, 1 };
	inline int mvalue_green[3] = { 3, 4, 3 };
	inline int command_purple[3] = { 3, 3, 5 };
	inline int format_blue[3] = { 2, 4, 5 };
	inline int rvalue_white = 23;
	inline int flag_gray = 14;
  	//Style 



	struct windowInfo {

		uint64_t aspectx{};
		uint64_t aspecty{};
		uint16_t flag{};
	};

	


	
	namespace UTILS {
		
		replxx::Replxx::colors_t& keywordHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, std::string_view keyword, replxx::Replxx::Color desired_color);

		replxx::Replxx::colors_t& commentHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, replxx::Replxx::Color desired_color);
		
		replxx::Replxx::colors_t& MvalueHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, replxx::Replxx::Color desired_color);

		std::string rgb666ToString(int r, int g, int b);

#ifdef BINARY_10_29_2025
		
		replxx::Replxx::colors_t& RvalueHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer, replxx::Replxx::Color desired_color);

#endif

		void inputHighlighter(std::string const& input_buffer, replxx::Replxx::colors_t& color_buffer);

		void draw(std::string_view content);

		void clear();

		void update(std::string_view new_content);

		void setCursor(uint64_t pos_x, uint64_t pos_y);

		void textPaint(uint16_t red, uint16_t green, uint16_t blue);
	}

	
}


#endif