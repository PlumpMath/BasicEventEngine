/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_STRING
#define BEE_UTIL_STRING 1

// String handling functions

#include <algorithm>
#include <regex>

#include "string.hpp" // Include the function definitions

/*
* chr() - Convert the given character code to a string
* @c: the character code to convert
*/
std::string chr(int c) {
	std::string str = "0";
	str[0] = c;
	return str;
}
/*
* ord() - Convert the given character to a character code
* @c: the character to convert
*/
int ord(char c) {
	return static_cast<int>(c);
}
/*
* ord() - Convert the given string's first character into a character code
* @s: the string to operate on
*/
int ord(const std::string& s) {
	return s[0];
}
/*
* chra() - Convert the given array of character data into a string
* @array: the array to convert
*/
std::string chra(std::vector<Uint8> array) {
	std::string str = ""; // Initialize an empty string to store the data in
	for (auto& e : array) { // Iterate over the array and append each element as a character
		str.append(chr(e));
	}
	return str; // Return the string on success
}
/*
* chra() - Convert the given array of character data into a string
* @size: the size of the array
* @carray: the array to convert
*/
std::string chra(size_t size, Uint8* carray) {
	std::string str = ""; // Initialize an empty string to store the data in
	for (size_t i=0; i<size; i++) { // Iterate over the array and append each element as a character
		str.append(chr(carray[i]));
	}
	return str; // Return the string on success
}
/*
* orda() - Convert the given string into an array of character data
* @s: the string to convert
*/
std::vector<Uint8> orda(const std::string& s) {
	std::vector<Uint8> v;
	for (size_t i=0; i<s.length(); i++) { // Iterate over the string and add each character to the array
		v.push_back(s[i]);
	}
	return v; // Return the array on success
}

/*
* string_lower() - Change the characters in the string so that they are all lowercase
* @str: the string to operate on
*/
std::string string_lower(const std::string& str) {
	std::string s = ""; // Declare a new string of the same size as the given string
	s.resize(str.length());
	std::transform(str.begin(), str.end(), s.begin(), ::tolower); // Transform the given string to lowercase
	return s;
}
/*
* string_upper() - Change the characters in the string so that they are all uppercase
* @str: the string to operate on
*/
std::string string_upper(const std::string& str) {
	std::string s = ""; // Declare a new string of the same size as the given string
	s.resize(str.length());
	std::transform(str.begin(), str.end(), s.begin(), ::toupper); // Transform the given string to uppercase
	return s;
}
/*
* string_upper() - Change the characters in the string so that they are titlecase, i.e. all lowercase except the beginning of words
* @str: the string to operate on
*/
std::string string_title(const std::string& str) {
	std::string s = "";
	s.reserve(str.length());

	char previous_c = ' ';
	for (auto c : str) {
		if (previous_c == ' ') {
			s += ::toupper(c);
		} else {
			s += ::tolower(c);
		}

		previous_c = c;
	}

	return s;
}
/*
* string_letters() - Filter the characters in the string so that only alphabetical characters are returned
* @str: the string to operate on
*/
std::string string_letters(const std::string& str) {
	return std::regex_replace(str, std::regex("[^[:alpha:]]"), std::string(""));
}
/*
* string_letters() - Filter the characters in the string so that only digits are returned
* @str: the string to operate on
*/
std::string string_digits(const std::string& str) {
	return std::regex_replace(str, std::regex("[^[:digit:]]"), std::string(""));
}
/*
* string_letters() - Filter the characters in the string so that only alphanumeric characters are returned
* @str: the string to operate on
*/
std::string string_lettersdigits(const std::string& str) {
	return std::regex_replace(str, std::regex("[^[:alnum:]]"), std::string(""));
}

/*
* split() - Split a string by a given delimiter and return the data as a map
* @input: the string to operate on
* @delimiter: the character to use to split the string
* @should_respect_quotes: whether quotes should split the string
*/
std::map<int,std::string> split(const std::string& input, char delimiter, bool should_respect_quotes) {
	std::map<int,std::string> output; // Declare a map to store the split strings

	size_t token_start = 0; // Store the position of the beginning of each token
	for (size_t i=0; i<input.length(); ++i) { // Iterate over each character in the string
		char c = input[i]; // Get the current character

		if (c == delimiter) { // If the character is a delimiter, store a substring in the map
			output.emplace(output.size(), input.substr(token_start, i-token_start));
			token_start = i+1; // Begin the next token after the delimiter
		} else if ((c == '"')&&(should_respect_quotes)) { // If the character is a quote, handle it separately
			if ((i>0)&&(input[i-1] == '\\')) {
				continue;
			}

			++i; // Increment past the first quote
			while (i<input.length()) { // Iterate over the string until the next quote or string end is reached
				if ((input[i] == '"')&&(input[i-1] != '\\')) {
					break;
				}

				++i;
			}
		}
	}
	if (token_start < input.length()) {  // Add the last token to the map if it exists
		output.emplace(output.size(), input.substr(token_start, input.length()-token_start));
	}

	return output; // Return the vector on success
}
/*
* split() - Split a string by a given delimiter and return the data as a map
* ! If the function is called without specifying how to act with quotes, simply call it without treating them specially
* @input: the string to operate on
* @delimiter: the character to use to split the string
*/
std::map<int,std::string> split(const std::string& input, char delimiter) {
	return split(input, delimiter, false);
}
/*
* handle_newlines() - Split a string by newlines and return the data as a map
* @input: the string to operate on
*/
std::map<int,std::string> handle_newlines(const std::string& input) {
	return split(input, '\n');
}
/*
* splitv() - Split a string by a given delimiter and return the data as a vector
* @input: the string to operate on
* @delimiter: the character to use to split the string
* @should_respect_containers: whether quotes should split the string
*/
std::vector<std::string> splitv(const std::string& input, char delimiter, bool should_respect_containers) {
	std::vector<std::string> output; // Declare a map to store the split strings
	std::string cont_start = "\"[{";
	std::string cont_end = "\"]}";
	std::vector<size_t> containers;

	size_t token_start = 0; // Store the position of the beginning of each token
	for (size_t i=0; i<input.length(); ++i) { // Iterate over each character in the string
		char c = input[i]; // Get the current character

		if (c == delimiter) { // If the character is a delimiter, store a substring in the map
			output.emplace_back(input.substr(token_start, i-token_start));
			token_start = i+1; // Begin the next token after the delimiter
		} else if ((cont_start.find(c) != std::string::npos)&&(should_respect_containers)) { // If the character is a container, handle it separately
			if ((i>0)&&(input[i-1] == '\\')) { // Skip escaped containers
				continue;
			}

			containers.push_back(cont_start.find(c)); // Store the container index

			while (++i < input.length()) { // Iterate over the string until the container or string end is reached
				if ((cont_end.find(input[i]) == containers.back())&&(input[i-1] != '\\')) {
					containers.pop_back();
					if (containers.empty()) {
						break;
					}
				} else if (cont_start.find(input[i]) != std::string::npos) { // Nest the containers if another is found
					containers.push_back(cont_start.find(input[i]));
				}
			}
		}
	}
	if (token_start < input.length()) {  // Add the last token to the map if it exists
		output.emplace_back(input.substr(token_start, input.length()-token_start));
	}

	return output; // Return the vector on success
}
/*
* join() - Join a map of strings by the given delimiter and return a string
* @input: the map to join together
* @delimiter: the character to join the string with
*/
std::string join(const std::map<int,std::string>& input, char delimiter) {
	std::string output = ""; // Declare a string to store each line from the map
	for (auto& l : input) { // Iterate over the map
		output += l.second; // Add each line to the output
		output += delimiter; // Append the delimiter for each line
	}
	if (output.size() > 0) {
		output.pop_back(); // Remove the last delimiter
	}
	return output; // Return the string on success
}
/*
* join() - Join a vector of strings by the given delimiter and return a string
* @input: the vector to join together
* @delimiter: the character to join the string with
*/
std::string joinv(const std::vector<std::string>& input, char delimiter) {
	std::string output = ""; // Declare a string to store each line from the vector
	for (auto& l : input) { // Iterate over the vector
		output += l; // Add each line to the output
		output += delimiter; // Append the delimiter for each line
	}
	if (output.size() > 0) {
		output.pop_back(); // Remove the last delimiter
	}
	return output; // Return the string on success
}


/*
* ltrim() - Trim the string for whitespace on the left side
* @str: the string to operate on
*/
std::string ltrim(const std::string& str) {
	unsigned int i;
	for (i=0; i<str.length(); i++) { // Iterate over the given string until there is a non-space character
		if (!isspace(str[i])) {
			break;
		}
	}
	return str.substr(i); // Return the string beginning at the first non-space character
}
/*
* rtrim() - Trim the string for whitespace on the right side
* @str: the string to operate on
*/
std::string rtrim(const std::string& str) {
	int i;
	for (i=str.length()-1; i>=0; i--) { // Reverse iterate over the given string until there is a non-space character
		if (!isspace(str[i])) {
			break;
		}
	}
	return str.substr(0, i+1); // Return the string ending at the first non-space character
}
/*
* trim() - Trim the string for whitespace on both sides
* @str: the string to operate on
*/
std::string trim(const std::string& str) {
	return rtrim(ltrim(str));
}

/*
* stringtobool() - Convert the given string to a boolean value
* ! Currently everything besides "0" and "false" will evaluate to true
* @str: the string to convert
*/
bool stringtobool(const std::string& str) {
	std::string lstr = string_lower(trim(str)); // Trim the string and convert it to lowercase
	std::vector<std::string> false_values = {"0", "false"}; // Declare the values which shall evaluate to false
	for (auto& s : false_values) { // Iterate over the false values and compare them to the string
		if (lstr == s) {
			return false; // Return false if the string is one of the false values
		}
	}
	return true; // Return true if the string is not any of the false values
}
/*
* booltostring() - Convert the given boolean value to a string
* @b: the bool to convert
*/
std::string booltostring(bool b) {
	if (b) {
		return "true"; // Return "true" if true
	}
	return "false"; // Otherwise, return false
}

/*
* string_replace() - Replace all occurences of the given search string with the given replacement
* @str: the string to operate on
* @search: the string to search and replace
* @replacment: the string to replace the search
*/
std::string string_replace(const std::string& str, const std::string& search, const std::string& replacement) {
	std::string s (str); // Create a new string so that the given string is not modified
	size_t start_pos = 0; // Start at the beginning of the string
	while ((start_pos = s.find(search, start_pos)) != std::string::npos) { // Continue to search through the string for the given substring
		s.replace(start_pos, search.length(), replacement); // Replace the substring
		start_pos += replacement.length(); // Start from the end of the replacement
	}
	return s; // Return the modified string
}
/*
* string_escape() - Replace all occurences of the quotes, forward slashes, and back slashes with escaped versions
* @str: the string to operate on
*/
std::string string_escape(const std::string& str) {
	std::string s (str);
	s = string_replace(s, "\\", "\\\\"); // Escape back slashes
	s = string_replace(s, "\"", "\\\""); // Escape quotes
	//s = string_replace(s, "/", "\\/"); // Escape forward slashes
	return s;
}
/*
* string_unescape() - Replace all occurences of escaped quotes, forward slashes, and back slashes with unescaped versions
* @str: the string to operate on
*/
std::string string_unescape(const std::string& str) {
	std::string s (str);
	s = string_replace(s, "\\\\", "\\"); // Replace escaped back slashes
	s = string_replace(s, "\\\"", "\""); // Replace escaped quotes
	//s = string_replace(s, "\\/", "/"); // Replace escaped forward slashes
	return s;
}
/*
* string_repeat() - Repeat a given string the given number of times
* @amount: the number of times to repeat the string
* @str: the string to repeat
*/
std::string string_repeat(size_t amount, const std::string& str) {
	if (str.length() == 1) { // If the string is only a single character long
		return std::string(amount, str[0]); // Return the string initializer which implements single-character repetition
	}

	std::string s;
	for (size_t i=0; i<amount; i++) { // Continually append the string as needed
		s += str;
	}
	return s; // Return the repeated string
}

/*
* string_tabulate() - Tabulate a table of strings such that the columns have at least one space between them
* ! Note that all sub-vectors should have the same number of elements
* @table: the table to tabulate
*/
std::string string_tabulate(const std::vector<std::vector<std::string>>& table) {
	std::vector<size_t> column_width; // Create a vector which measures the width of each existing column
	for (size_t i=0; i<table[0].size(); i++) { // Iterate over the columns of the first row
		column_width.push_back(0); // Push an initial width for every column
	}

	for (auto& r : table) { // Iterate over the table rows
		for (size_t i=0; i<r.size(); i++) { // Iterate over the table columns
			if (r[i].length() > column_width[i]) { // If the length of this value is greater than the column width
				column_width[i] = r[i].length(); // Expand the column width to fit it
			}
		}
	}

	std::string str;
	for (auto& r : table) { // Iterate over the table rows
		for (size_t i=0; i<r.size(); i++) { // Iterate over the table columns
			str += r[i] + string_repeat(column_width[i]-r[i].length()+1, " "); // Append the columns to the tabulation
		}
		str += "\n"; // Separate each row with a new line
	}

	return str; // Return the tabulation
}

/*
* clipboard_has_text() - Return whether there is currently text in the clipboard
*/
bool clipboard_has_text() {
	return (SDL_HasClipboardText() == SDL_TRUE) ? true : false;
}
/*
* clipboard_get_text() - Return the text that is currently in the clipboard
*/
std::string clipboard_get_text() {
	char* cstr = SDL_GetClipboardText(); // Get the clipboard text and convert it to a string
	std::string s (cstr);
	free(cstr);
	return s; // Return the string on success
}
/*
* clipboard_set_text() - Copy the given text to the clipboard
* ! See https://wiki.libsdl.org/SDL_SetClipboardText for details
* @str: the string to copy
*/
int clipboard_set_text(const std::string& str) {
	return SDL_SetClipboardText(str.c_str());
}

/*
* is_str_floating() - Return whether the string is a floating point number
* @str: the string to evaluate
*/
bool is_str_floating(const std::string& str) {
	std::string ns = str;
	if (ns.empty()) {
		return false;
	}

	// Remove the number's sign if it has one
	if ((ns[0] == '-')||(ns[0] == '+')) {
		ns = ns.substr(1);
	}

	// Remove any scientific notation
	static const std::vector<std::string> exps = {"e", "E"};
	for (auto& e : exps) {
		if (ns.find(e) != std::string::npos) {
			size_t exponent = ns.find(e);
			if (!is_str_integer(ns.substr(exponent+1))) { // If the exponent is not an integer, it is not valid scientific notation
				return false;
			}

			ns.erase(exponent);
			break; // After removing the notation, continue below
		}
	}

	// Remove a single decimal point
	if (ns.find(".") != std::string::npos) {
		ns.erase(ns.find("."));
	}

	// Remove all digits
	static const std::vector<std::string> digits = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
	for (auto& d : digits) {
		ns = string_replace(ns, d, "");
	}

	if (!ns.empty()) {
		return false;
	}
	return true;
}
/*
* is_str_integer() - Return whether the string is an integer
* @str: the string to evaluate
*/
bool is_str_integer(const std::string& str) {
	std::string ns = str;
	if (ns.empty()) {
		return false;
	}

	// Remove the number's sign if it has one
	if ((ns[0] == '-')||(ns[0] == '+')) {
		ns = ns.substr(1);
	}

	// Remove all digits
	static const std::vector<std::string> digits = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
	for (auto& d : digits) {
		ns = string_replace(ns, d, "");
	}

	if (!ns.empty()) {
		return false;
	}
	return true;
}

#endif // BEE_UTIL_STRING
