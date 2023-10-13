#include <algorithm>
#include <clocale>
#include <functional>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>
#include <climits>
#include <optional>
#include "Token.h"

#define WHILE_FIND_TOKEN(tok) \
	if (std::find(disallowed_toks.begin(), disallowed_toks.end(), #tok) == disallowed_toks.end()) { \
		tok_it = _ltoks.begin(); \
		while (true) { \
			tok_it = remove_constness(_ltoks, find_tok(_ltoks, tok, tok_it)); \
			if (tok_it != _ltoks.end()) {
 
#define WHILE_FIND_TOKEN_END \
			} else { \
				break; \
			} \
			tok_it++; \
		} \
	} \

#define WHILE_US_FIND_TOKEN(tok) \
	if (std::find(disallowed_toks.begin(), disallowed_toks.end(), #tok) == disallowed_toks.end()) { \
		tok_it = _us_ltoks.begin(); \
		while (true) { \
			tok_it = remove_constness(_us_ltoks, find_tok(_us_ltoks, tok, tok_it)); \
			if (tok_it != _us_ltoks.end()) {

#define WHILE_FIND_TOKENS(toks) \
	if (std::find(disallowed_toks.begin(), disallowed_toks.end(), #toks) == disallowed_toks.end()) { \
		tok_it = _ltoks.begin(); \
		while (true) { \
			tok_it = remove_constness(_ltoks, find_first_tok(_ltoks, toks, tok_it)); \
			if (tok_it != _ltoks.end()) {

#define WHILE_US_FIND_TOKENS(toks) \
	if (std::find(disallowed_toks.begin(), disallowed_toks.end(), #toks) == disallowed_toks.end()) { \
		tok_it = _us_ltoks.begin(); \
		while (true) { \
			tok_it = remove_constness(_us_ltoks, find_first_tok(_us_ltoks, toks, tok_it)); \
			if (tok_it != _us_ltoks.end()) {

using TokIt = std::vector<std::string>::iterator;

std::vector<std::string> _ltoks;
std::vector<std::string> _us_ltoks;

std::vector<std::string> out;

struct Translation {
	std::string before;
	std::string after;
};
std::vector<Translation> tag_translations = {
	{ "rhtml", "html" }
};

using uchar = unsigned char;

const char* const ws = " \t\n\r\f\v";

size_t line_number = 0;

namespace clog {
	template <typename T>
	void out(const T &str) noexcept {
		std::cout << str << std::endl;
	}
	void warn(const std::string &str, bool print_line = true) noexcept {
		std::cerr << (print_line ? "LINE: " + std::to_string(line_number) : "") << "WARNING: " << str << std::endl;
	}
	void error(const std::string &str, bool print_line = true) noexcept {
		std::cerr << (print_line ? "LINE: " + std::to_string(line_number) : "") << " ERROR: " << str << std::endl;
	}
	void note(const std::string &str) noexcept {
		std::cout << "NOTE: " << str << std::endl;
	}
}

// trim from end of string (right)
std::string rtrim(std::string s, const char* t = ws) {
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

// trim from beginning of string (left)
std::string ltrim(std::string s, const char* t = ws) {
	s.erase(0, s.find_first_not_of(t));
	return s;
}

// trim from both ends of string (right then left)
std::string trim(std::string s, const char* t = ws) {
	if (!s.empty())
		return ltrim(rtrim(s, t), t);
	else
		return "";
}

std::vector<std::string> split(const std::string &str) { // IT WORKS!! WOW!!
	std::vector<std::string> ret;
	std::vector<bool> ret_is_token;

	ret.push_back(str);
	ret_is_token.push_back(false);

	for (const Token &token : tokens) {
		int j = 0;
		for (int i = 0; i < ret.size(); i++) {
			if (!ret_is_token[i]) {
				std::string chunk = ret[i];
				size_t token_index = chunk.find(token.token);
				if (token_index != std::string::npos) {
					ret.erase(ret.begin()+i);
					ret_is_token.erase(ret_is_token.begin()+i);

					std::string sub = chunk.substr(0, token_index);
					if (!sub.empty()) {
						ret.insert(ret.begin()+i++, chunk.substr(0, token_index));
						ret_is_token.insert(ret_is_token.begin()+(i-1), false);
					}

					ret.insert(ret.begin()+i++, token.token);
					ret_is_token.insert(ret_is_token.begin()+(i-1), true);

					int begin = token_index+token.token.size();
					sub = chunk.substr(begin);
					if (!sub.empty()) {
						ret.insert(ret.begin()+i, sub);
						ret_is_token.insert(ret_is_token.begin()+i, false);
					}
				}
			}
			i = j;
			j++;
		}
	}
	
	return ret;
}

template <typename Container, typename ConstIterator>
typename Container::iterator remove_constness(Container& c, ConstIterator it) {
    return c.erase(it, it);
}

std::vector<std::string> unspaced(const std::vector<std::string> &vec) {
	std::vector<std::string> ret;
	for (const std::string &str : vec) {
		if (str != " ") ret.push_back(str);
	}
	return ret;
}

std::vector<std::string>::const_iterator find_tok(const std::vector<std::string> &ltoks, const std::string &str,
				const std::vector<std::string>::const_iterator &begin) {
	return std::find(begin, ltoks.end(), str);
}

std::vector<std::string>::const_iterator find_first_tok(const std::vector<std::string> &ltoks, const std::vector<std::string> &toks_to_find,
				const std::vector<std::string>::const_iterator &begin) {
	for (const std::string &tok : toks_to_find) {
		if (auto it = std::find(begin, ltoks.end(), tok); it != ltoks.end()) return it;
	}

	return ltoks.end();
}

template <typename T>
size_t from_it(const std::vector<T> &vec, const typename std::vector<T>::const_iterator &it) {
	return std::distance(vec.begin(), it);
}

template <typename T>
size_t index_of(const std::vector<T> &vec, const T &val) {
	return from_it(vec, std::find(vec.begin(), vec.end(), val));
}

template <typename T>
size_t index_of_last(const std::vector<T> &vec, const T &val) {
	return from_it(vec, std::find(vec.rbegin(), vec.rend(), val).base());
}

std::vector<std::string> replace_toks(std::vector<std::string> toks, size_t begin, size_t end, const std::string &str) {
	toks.erase(toks.begin()+begin, toks.begin()+end+1);
	toks.insert(toks.begin()+begin, str);
	
	return toks;
}

std::vector<std::string> replace_toks(const std::vector<std::string> &toks, std::vector<std::string>::const_iterator begin,
		std::vector<std::string>::const_iterator end, const std::string &str) {
	return replace_toks(toks, from_it(toks, begin), from_it(toks, end), str);
}

std::vector<std::string> replace_tok(std::vector<std::string> toks, size_t i, const std::string &str) {
	toks[i] = str;
	return toks;
}

std::vector<std::string> replace_tok(const std::vector<std::string> &toks, std::vector<std::string>::const_iterator it, const std::string &str) {
	return replace_tok(std::vector<std::string>(toks), from_it(toks, it), str);
}

void commit(const std::vector<std::string> &new_vec) {
	_ltoks = new_vec;
	if (find_tok(new_vec, " ", new_vec.begin()) ==	new_vec.end()) {
		_us_ltoks = new_vec;
	} else {
		_us_ltoks = unspaced(_ltoks);
	}
}


std::string combine_toks(const std::vector<std::string>::const_iterator &begin, const std::vector<std::string>::const_iterator &end) {
	std::string ret = "";
	for (auto it = begin; it != end; it++) {
		ret += *it;
	}
	return ret;
}

std::string get_string_literal(const std::vector<std::string> &toks, TokIt index, bool with_quotes = true) { // Not actually used but I'm keeping it
	std::string ret = "";
	for (index++; *index != "\""; index++) {
		ret += *index;
	}
	if (with_quotes) {
		ret = toks[0] + ret;
		ret += *index;
	}

	return ret;
}

bool is_number(const std::string &s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

namespace tok_funcs {
	void open_curly_brace(TokIt tok_it) {
		out.push_back('<' + *(tok_it-1) + '>');
	}
	void close_curly_brace(TokIt tok_it) {
		out.push_back("</" + *(tok_it+1) + '>');
	}
	void quote(TokIt tok_it, bool &in_quotes, TokIt &quote) {
		in_quotes = !in_quotes;
		if (in_quotes) {
			quote = tok_it;
		} else {
			out.push_back(combine_toks(quote+1, tok_it));
		}
	}
};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		clog::error("Must input file to compile.", 0);
		return 0;
	}
	
	std::ifstream read;
	read.open(argv[1], std::ifstream::in);
	std::ofstream write;
	write.open("rhtmlout.html", std::ofstream::out | std::ios::trunc);
	
	std::vector<std::string> disallowed_toks = { };
	
	bool in_quotes = false;
	TokIt quote;

	TokIt tok_it;

	std::string l;
	while (std::getline(read, l)) {
		line_number++;
		l = trim(l);
		if (!l.empty()) {
			_ltoks = split(l);
			_us_ltoks = unspaced(_ltoks);
			WHILE_US_FIND_TOKEN("{") {
				tok_funcs::open_curly_brace(tok_it);
			} WHILE_FIND_TOKEN_END
			WHILE_US_FIND_TOKEN("}") {
				tok_funcs::close_curly_brace(tok_it);
			} WHILE_FIND_TOKEN_END
			WHILE_US_FIND_TOKEN("\"") {
				tok_funcs::quote(tok_it, in_quotes, quote);
			} WHILE_FIND_TOKEN_END

			disallowed_toks.clear();
		}
		out.push_back("\n");
	}

	for (const std::string &str : out) {
		write << str;
	}
	
	read.close();
	write.close();

	return 0;
}

