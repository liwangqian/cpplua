#ifndef CPPLUA_KEYWORD_H
#define CPPLUA_KEYWORD_H

#include "def.h"

CPPLUA_NS_BEGIN

enum keyword_t {
    NOT_A_KW = 0,
    KW_DO = 1,
    KW_IF,
    KW_IN,
    KW_OR,
    KW_AND,
    KW_END,
    KW_FOR,
    KW_NOT,
    KW_ELSE,
    KW_THEN,
    KW_GOTO,
    KW_BREAK,
    KW_LOCAL,
    KW_UNTIL,
    KW_WHILE,
    KW_ELSEIF,
    KW_REPEAT,
    KW_RETURN,
    KW_FUNCTION,
    KW_MAX
};

bool is_keyword(const string_t &val);

keyword_t get_keyword_t(const string_t &val);

CPPLUA_NS_END

#endif
