/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_FILES_H
#define _BEE_UTIL_FILES_H 1

// File handling functions

#include <string>
#include <fstream>
#include <sys/stat.h>

bool file_exists(std::string fname) {
        std::fstream f (fname.c_str());
        return f.good();
}
int file_delete(std::string fname) {
        return remove(fname.c_str());
}
int file_rename(std::string old_fname, std::string new_fname) {
        return rename(old_fname.c_str(), new_fname.c_str());
}
int file_copy(std::string fname, std::string new_fname) {
        std::ifstream source (fname, std::ios::binary);
        std::ofstream dest (new_fname, std::ios::binary);

        dest << source.rdbuf();

        source.close();
        dest.close();

        return 0;
}
std::string file_get_contents(std::string fname) {
        std::ifstream input (fname);
        if (input.is_open()) {
                std::string s, tmp;
                while (!input.eof()) {
                        getline(input, tmp);
                        s += tmp + "\n";
                }
                input.close();
                return s;
        }
        return "";
}

bool directory_exists(std::string fname) {
        struct stat st;
        stat(fname.c_str(), &st);
        return S_ISDIR(st.st_mode);
}
int directory_create(std::string fname) {
        return bee_mkdir(fname.c_str(), 0755);
}
std::string directory_get_temp() {
        char t[] = "/tmp/bee-XXXXXX";
        return bee_mkdtemp(t);
}

std::string file_basename(std::string path) {
        return {
                std::find_if(path.rbegin(), path.rend(),
                        [] (char c) {
                                return c == '/' || c == '\\';
                        }
                ).base(),
                path.end()
        };
}
std::string file_dirname(std::string path) {
        return {
                path.begin(),
                std::find_if(path.rbegin(), path.rend(),
                        [] (char c) {
                                return c == '/' || c == '\\';
                        }
                ).base()
        };
}
std::string file_plainname(std::string path) {
        return {
                path.begin(),
                std::find_if(path.rbegin(), path.rend(),
                        [] (char c) {
                                return c == '.';
                        }
                ).base()-1
        };
}
std::string file_extname(std::string path) {
        return {
                std::find_if(path.rbegin(), path.rend(),
                        [] (char c) {
                                return c == '.';
                        }
                ).base()-1,
                path.end()
        };
}

#endif // _BEE_UTIL_FILES_H
