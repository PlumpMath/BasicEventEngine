/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_MESSAGEBOX_H
#define BEE_UTIL_MESSAGEBOX_H 1

#include <string>

int show_message(const std::string&, const std::string&, const std::string&, const std::string&);
bool show_question(const std::string&);
int show_message(const std::string&);
int show_warning(const std::string&);
int show_error(const std::string&, bool);
int show_error(const std::string&);

#endif // BEE_UTIL_MESSAGEBOX_H
