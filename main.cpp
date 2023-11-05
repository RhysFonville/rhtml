#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
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

#define TOK(i) *(tok_it i)
#define RTOK(i) (tok_it i)

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
	template <typename T>
	void warn(const T &str, bool print_line = true) noexcept {
		std::cerr << (print_line ? "LINE: " + std::to_string(line_number) : "") << "WARNING: " << str << std::endl;
	}
	template <typename T>
	void error(const T &str, bool print_line = true) noexcept {
		std::cerr << (print_line ? "LINE: " + std::to_string(line_number) : "") << " ERROR: " << str << std::endl;
	}
	template <typename T>
	void note(const T &str) noexcept {
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
	
	//bool in_quotes = false;
	//for (int i = 0; i < ret.size(); i++) {
	//	if (ret[i] == "\"" && !in_quotes) in_quotes = false;
	//	else if (ret[i] == "\"" && in_quotes) in_quotes = true;
	//	else if (ret[i] == " " && !in_quotes) ret.erase(ret.begin()+i);
	//}
	
	return ret;
}

template <typename T>
std::ostream & operator<<(std::ostream &os, const std::vector<T> &v) {
    for (typename std::vector<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii) {
        os << " " << *ii;
    }
    return os;
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

std::string translate(const std::string &str) {
	for (const Translation &translation : tag_translations) {
		if (translation.before == str) return translation.after;
	}
	
	return str;
}

namespace tok_funcs {
	void open_curly_brace(TokIt tok_it, std::vector<std::string> &tag_tree) {
		std::string tag = translate(TOK(-1)); 
		out.push_back('<' + tag + '>');
		tag_tree.push_back(tag);
	}
	void close_curly_brace(TokIt tok_it, std::vector<std::string> &tag_tree) {
		std::string closing_tag = "</";
		if (RTOK(+1) == _us_ltoks.end() || TOK(+1) == ";") {
			closing_tag += translate(tag_tree.back());
		} else {
			closing_tag += translate(TOK(+1));
		}
		closing_tag += ">\n";
		out.push_back(closing_tag);

		tag_tree.pop_back();
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
		return 1;
	}
	std::string src_file = std::string(argv[1]);
	if (src_file.substr(src_file.size()-6) != ".rhtml" && src_file.substr(src_file.size()-5) != ".html") {
		clog::error("Source file must be .html or .rhtml.", false);
		return 1;
	}
	
	std::ifstream read;
	read.open(src_file, std::ifstream::in);
	std::ofstream write;
	write.open("rhtmlout.html", std::ofstream::out | std::ios::trunc);
	
	out.push_back("<!DOCTYPE html>\n");
	
	std::vector<std::string> disallowed_toks = { };
	
	bool in_quotes = false;
	TokIt quote;

	TokIt tok_it;
	
	std::vector<std::string> tag_tree;
	
	std::string l;
	while (std::getline(read, l)) {
		line_number++;
		l = trim(l);
		if (!l.empty()) {
			_ltoks = split(l);
			_us_ltoks = unspaced(_ltoks);
			
			//for (auto s : _us_ltoks) { std::cout << '\"' << s << "\", "; }
			//std::cout << std::endl;
			//for (auto s : _ltoks) { std::cout << '\"' << s << "\", "; }
			//std::cout << std::endl;

			auto stok_it = _ltoks.begin();
			for (tok_it = _us_ltoks.begin(); tok_it != _us_ltoks.end(); tok_it++) {
				if (*tok_it == "\"") {
					tok_funcs::quote(stok_it, in_quotes, quote);
				} else if (!in_quotes) {
					if (*tok_it == "{") {
						tok_funcs::open_curly_brace(tok_it, tag_tree);
					} else if (*tok_it == "}") {
						tok_funcs::close_curly_brace(tok_it, tag_tree);
					}
				}
				if (stok_it+1 != _ltoks.end()) {
					stok_it = std::find(stok_it+1, _ltoks.end(), TOK(+1));
				}
			}
			
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

