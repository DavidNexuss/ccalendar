#pragma once
#define DEBUG
#ifdef DEBUG
#include <iostream>

#define LOG(...) std::cerr << __FILE__ << ":" << __LINE__ << "-> "; fprintf( stderr, __VA_ARGS__); std::cerr << std::endl;
#define EVAL(x) std::cerr << __FILE__ << ":" << __LINE__ << "-> " << # x " = " << (x) << std::endl
#define ERR(...) std::cerr << "\033[0;33m" << __FILE__ << ":" << __LINE__ << "-> "; fprintf( stderr, __VA_ARGS__); std::cerr << << "\033[0m" << std::endl;

#define TO_STRING_D(x) #x
#define TO_STRING(x) TO_STRING_D(x)
#define error_y_exit(msg,exit_status) { perror(__FILE__ ":" TO_STRING(__LINE__) "  " msg); exit(exit_status); }

#else
#define LOG(...)
#define EVAL(x)
#define ERR(...)
#define error_y_exit(msg,exit_status) { perror(msg); exit(1); }
#endif
