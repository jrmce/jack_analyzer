#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_STRING_LENGTH   128

// Keywords
#define CLASS               "class"
#define CONSTRUCTOR         "constructor"
#define FUNCTION            "function"
#define METHOD              "method"
#define FIELD               "field"
#define STATIC              "static"
#define VAR                 "var"
#define INT                 "int"
#define CHAR                "char"
#define BOOLEAN             "boolean"
#define VOID                "void"
#define J_TRUE              "true"
#define J_FALSE             "false"
#define J_NULL              "null"
#define THIS                "this"
#define LET                 "let"
#define DO                  "do"
#define IF                  "if"
#define ELSE                "else"
#define WHILE               "while"
#define RETURN              "return"

// Memory Segments
#define SEG_CONSTANT        "constant"
#define SEG_LOCAL           "local"
#define SEG_ARGUMENT        "argument"
#define SEG_FIELD           "this"
#define SEG_STATIC          "static"
#define SEG_POINTER         "pointer"
#define SEG_TEMP            "temp"
#define SEG_THAT            "that"

// System calls
#define SYS_MULTIPLY        "Math.multiply"
#define SYS_DIVIDE          "Math.divide"
#define SYS_ALLOC           "Memory.alloc"
#define SYS_DEALLOC         "Memory.deAlloc"
#define SYS_STRING_NEW      "String.new"
#define SYS_STRING_APPEND   "String.appendChar"

// VM Operations
#define OP_ADD              "add"
#define OP_SUB              "sub"
#define OP_AND              "and"
#define OP_OR               "or"
#define OP_LT               "lt"
#define OP_GT               "gt"
#define OP_EQ               "eq"
#define OP_NEG              "neg"
#define OP_NOT              "not"

// Token symbols
#define LEFT_BRACE          '{'
#define RIGHT_BRACE         '}'
#define LEFT_PAREN          '('
#define RIGHT_PAREN         ')'
#define LEFT_BRACKET        '['
#define RIGHT_BRACKET       ']'
#define PERIOD              '.'
#define COMMA               ','
#define SEMICOLON           ';'
#define PLUS                '+'
#define MINUS               '-'
#define ASTERISK            '*'
#define SLASH               '/'
#define AMPERSAND           '&'
#define PIPE                '|'
#define LESS_THAN           '<'
#define GREATER_THAN        '>'
#define EQUAL               '='
#define TILDE               '~'
#define UNDERSCORE          '_'

#endif
