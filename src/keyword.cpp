#include "keyword.h"
#include <unordered_map>

CPPLUA_NS_BEGIN

static std::unordered_map<string_t, const keyword_t> g_keywords{
    {"do",       KW_DO},
    {"if",       KW_IF},
    {"in",       KW_IN},
    {"or",       KW_OR},
    {"and",      KW_AND},
    {"end",      KW_END},
    {"for",      KW_FOR},
    {"not",      KW_NOT},
    {"else",     KW_ELSE},
    {"then",     KW_THEN},
    {"goto",     KW_GOTO},
    {"break",    KW_BREAK},
    {"local",    KW_LOCAL},
    {"until",    KW_UNTIL},
    {"while",    KW_WHILE},
    {"elseif",   KW_ELSEIF},
    {"repeat",   KW_REPEAT},
    {"return",   KW_RETURN},
    {"function", KW_FUNCTION}
};

bool is_keyword(const string_t &val)
{
    return g_keywords.find(val) != g_keywords.end();
}

keyword_t get_keyword_t(const string_t &val)
{
    return g_keywords.at(val);
}

CPPLUA_NS_END
