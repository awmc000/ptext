#include <ctype.h>
#include <lexer.h>
#include <stdio.h>
#include <string.h>

static struct flags fl = {0};

static const char *keywords[] = {
    "auto",          "break",
    "case",          "char",
    "const",         "continue",
    "default",       "do",
    "double",        "else",
    "enum",          "extern",
    "float",         "for",
    "goto",          "if",
    "int",           "long",
    "register",      "return",
    "short",         "signed",
    "sizeof",        "static",
    "struct",        "switch",
    "typedef",       "union",
    "unsigned",      "void",
    "volatile",      "while",
    "alignas",       "alignof",
    "and",           "and_eq",
    "asm",           "atomic_cancel",
    "atomic_commit", "atomic_noexcept",
    "bitand",        "bitor",
    "bool",          "catch",
    "char16_t",      "char32_t",
    "char8_t",       "class",
    "co_await",      "co_return",
    "co_yield",      "compl",
    "concept",       "const_cast",
    "consteval",     "constexpr",
    "constinit",     "decltype",
    "delete",        "dynamic_cast",
    "explicit",      "export",
    "false",         "friend",
    "inline",        "mutable",
    "namespace",     "new",
    "noexcept",      "not",
    "not_eq",        "nullptr",
    "operator",      "or",
    "or_eq",         "private",
    "protected",     "public",
    "reflexpr",      "reinterpret_cast",
    "requires",      "static_assert",
    "static_cast",   "synchronized",
    "template",      "this",
    "thread_local",  "throw",
    "true",          "try",
    "typeid",        "typename",
    "using",         "virtual",
    "wchar_t",       "xor",
    "xor_eq",
};

Single_Tokens single_tokens[] = {
    {.kind = TOKEN_SEMICOLON, .text = ";"},
    {.kind = TOKEN_OPEN_CURLY, .text = "}"},
    {.kind = TOKEN_CLOSE_CURLY, .text = "{"},
    {.kind = TOKEN_OPEN_PAREN, .text = "("},
    {.kind = TOKEN_CLOSE_PAREN, .text = ")"},
    {.kind = TOKEN_SEMICOLON, .text = ";"},
};

#define KEYWORDS_COUNT (sizeof(keywords) / sizeof(keywords[0]))

static int isSymbol(char x) { return isalnum(x) || x == '_'; }
static int isSymbolStart(char x) { return isalpha(x) || x == '_'; }

int trimLeft(Lexer *l) {
  int num = 0;
  while (isspace(l->content[l->cursor])) {
    l->cursor++;
    num++;
  }
  return num;
}

Token getNextToken(Lexer *l) {
  Token t = {0};
  int firstloc = l->cursor;
  int spaces = trimLeft(l);
  t.text = &l->content[l->cursor];
  if (l->cursor >= l->contentlen) {
    t.kind = TOKEN_END;
    return t;
  }

  if (l->content[l->cursor] == '#') {
    t.kind = TOKEN_PREPROC;
    t.textlen = l->contentlen;
    l->cursor = l->contentlen;
    return t;
  }
  size_t i;
  for (i = 0; i < sizeof(single_tokens) / sizeof(single_tokens[0]); ++i) {
    if (l->content[l->cursor] == single_tokens[i].text[0]) {
      t.kind = single_tokens[i].kind;
      t.textlen = 1 + spaces;
      t.text = &l->content[firstloc];
      l->cursor++;
      return t;
    }
  }
  if (isdigit(l->content[l->cursor])) {
    t.kind = TOKEN_DIGIT;
    while (isdigit(l->content[l->cursor]) && l->cursor < l->contentlen) {
      t.textlen++;
      l->cursor++;
    }
    t.textlen += spaces;
    t.text = &l->content[firstloc];
    return t;
  }

  if (isSymbolStart(l->content[l->cursor])) {
    t.kind = TOKEN_SYMBOL;
    while (isSymbol(l->content[l->cursor]) && l->cursor < l->contentlen) {
      t.textlen++;
      l->cursor++;
    }
    for (i = 0; i < KEYWORDS_COUNT; ++i) {
      size_t keyword_len = strlen(keywords[i]);
      if (keyword_len == (t.textlen) &&
          memcmp(keywords[i], t.text, keyword_len) == 0) {
        t.kind = TOKEN_KEYWORD;
        break;
      }
    }
    t.textlen += spaces;
    t.text = &l->content[firstloc];
    return t;
  }
  t.kind = TOKEN_INVALID;
  t.textlen = 1;
  t.textlen += spaces;
  t.text = &l->content[firstloc];
  l->cursor++;
  return t;
}