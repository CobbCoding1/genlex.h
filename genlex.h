#ifndef GENLEX_H
#define GENLEX_H
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_START_CAPACITY 32

#define ASSERT(cond, ...)                                                      \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fprintf(stderr, "%s:%d: ASSERTION FAILED: ", __FILE__, __LINE__);        \
      fprintf(stderr, __VA_ARGS__);                                            \
      fprintf(stderr, "\n");                                                   \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

#define DA_APPEND(da, item)                                                    \
  do {                                                                         \
    if ((da)->count >= (da)->capacity) {                                       \
      (da)->capacity =                                                         \
          (da)->capacity == 0 ? DATA_START_CAPACITY : (da)->capacity * 2;      \
      void *new = calloc(((da)->capacity + 1), sizeof(*(da)->data));           \
      ASSERT(new, "outta ram");                                                \
      memcpy(new, (da)->data, (da)->count);                                    \
      free((da)->data);                                                        \
      (da)->data = new;                                                        \
    }                                                                          \
    (da)->data[(da)->count++] = (item);                                        \
  } while (0)

typedef enum {
  GENLEX_ERR_NONE,
  GENLEX_ERR_UNKNOWN_TOKEN,
} GenlexErr;

typedef enum {
  TOKEN_IDENT = 0,
  TOKEN_INT_LIT,
  TOKEN_FLOAT_LIT,
  TOKEN_CHAR_LIT,
  TOKEN_STR_LIT,
  TOKEN_S_STR_LIT,
  TOKEN_SEMI,
  TOKEN_COLON,
  TOKEN_QUESTION,
  TOKEN_DOT,
  TOKEN_COMMA,
  TOKEN_EQ,
  TOKEN_EQ_EQ,
  TOKEN_PLUS,
  TOKEN_PLUS_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_MOD,
  TOKEN_NOT,
  TOKEN_OR,
  TOKEN_XOR,
  TOKEN_AND,
  TOKEN_BOR,
  TOKEN_BAND,
  TOKEN_BNOT,
  TOKEN_PLUS_EQ,
  TOKEN_MINUS_EQ,
  TOKEN_STAR_EQ,
  TOKEN_SLASH_EQ,
  TOKEN_MOD_EQ,
  TOKEN_B_SLASH,
  TOKEN_NOT_EQ,
  TOKEN_BNOT_EQ,
  TOKEN_OR_EQ,
  TOKEN_XOR_EQ,
  TOKEN_AND_EQ,
  TOKEN_L_EQ,
  TOKEN_G_EQ,
  TOKEN_HASH,
  TOKEN_O_PAREN,
  TOKEN_C_PAREN,
  TOKEN_O_CURL,
  TOKEN_C_CURL,
  TOKEN_O_BRACE,
  TOKEN_C_BRACE,
  TOKEN_L_THAN,
  TOKEN_G_THAN,
  TOKEN_SH_L,
  TOKEN_SH_R,
  TOKEN_NONE,
} TokenType;

char *genlex_err_str[] = {
    "GENLEX_ERR_NONE",
    "GENLEX_ERR_UNKNOWN_TOKEN",
};

char *genlex_type_str[TOKEN_NONE] = {
    "TOKEN_IDENT",    "TOKEN_INT_LIT",   "TOKEN_FLOAT_LIT", "TOKEN_CHAR_LIT",
    "TOKEN_STR_LIT",  "TOKEN_S_STR_LIT", "TOKEN_SEMI",      "TOKEN_COLON",
    "TOKEN_QUESTION", "TOKEN_DOT",       "TOKEN_COMMA",     "TOKEN_EQ",
    "TOKEN_EQ_EQ",    "TOKEN_PLUS",      "TOKEN_PLUS_PLUS", "TOKEN_MINUS",
    "TOKEN_STAR",     "TOKEN_SLASH",     "TOKEN_MOD",       "TOKEN_B_SLASH",
    "TOKEN_NOT",      "TOKEN_OR",        "TOKEN_XOR",       "TOKEN_AND",
    "TOKEN_BOR",      "TOKEN_BAND",      "TOKEN_BNOT",      "TOKEN_PLUS_EQ",
    "TOKEN_MINUS_EQ", "TOKEN_STAR_EQ",   "TOKEN_SLASH_EQ",  "TOKEN_MOD_EQ",
    "TOKEN_NOT_EQ",   "TOKEN_BNOT_EQ",   "TOKEN_OR_EQ",     "TOKEN_XOR_EQ",
    "TOKEN_AND_EQ",   "TOKEN_L_EQ",      "TOKEN_G_EQ",      "TOKEN_HASH",
    "TOKEN_O_PAREN",  "TOKEN_C_PAREN",   "TOKEN_O_CURL",    "TOKEN_C_CURL",
    "TOKEN_O_BRACE",  "TOKEN_C_BRACE",   "TOKEN_L_THAN",    "TOKEN_G_THAN",
    "TOKEN_SH_L",     "TOKEN_SH_R",
};

typedef struct {
  TokenType type;
  char *value;
} Token;

typedef struct {
  Token *data;
  size_t count;
  size_t capacity;
} Tokens;

typedef struct {
  Tokens tokens;
  char *content;
  size_t index;
  char *filename;
} Lexer;

char *genlex_read_file(char *filename);
Token genlex_parse_num_lit(Lexer *l);
Token genlex_parse_id(Lexer *l);
Token genlex_parse_single_quotes(Lexer *l);
Token genlex_parse_string(Lexer *l);

Lexer *genlex_init_lexer(char *filename);
void genlex_exit_lexer(Lexer *lexer);

#endif

#ifdef GENLEX_IMPLEMENTATION

char *genlex_read_file(char *filename) {
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "ERROR: Could not open file: %s\n", filename);
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  size_t len = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *contents = malloc(sizeof(char) * len + 1);

  fread(contents, len, sizeof(char), f);
  contents[len] = '\0';

  fclose(f);

  return contents;
}

Token genlex_parse_num_lit(Lexer *l) {
  Token token = {0};
  token.type = TOKEN_INT_LIT;
  size_t start_index = l->index;
  char *start = &l->content[start_index];
  while (isdigit(l->content[l->index]) || l->content[l->index] == '.') {
    l->index++;
    if (l->content[l->index] == '.')
      token.type = TOKEN_FLOAT_LIT;
  }
  token.value = strndup(start, l->index - start_index);
  if (l->index > 0)
    l->index--;
  return token;
}

Token genlex_parse_id(Lexer *l) {
  Token token = {0};
  size_t start_index = l->index;
  char *start = &l->content[start_index];
  while (isalnum(l->content[l->index]) || l->content[l->index] == '_') {
    l->index++;
  }
  token.value = strndup(start, l->index - start_index);
  token.type = TOKEN_IDENT;
  if (l->index > 0)
    l->index--;
  return token;
}

Token genlex_parse_single_quotes(Lexer *l) {
  Token token = {0};

  size_t start_index = ++l->index;
  char *start = &l->content[start_index];
  while (l->content[l->index] != '\'') {
    // skip over the escape quote
    if (l->content[l->index] == '\\' &&
        (l->content[l->index + 1] == '\'' || l->content[l->index + 1] == '\\'))
      l->index++;

    l->index++;
    if (l->content[l->index] == '\0') {
      fprintf(stderr, "ERROR: Unclosed character/string literal\n");
      return (Token){0};
    }
  }

  token.value = strndup(start, l->index - start_index);
  token.type = strlen(token.value) > 1 ? TOKEN_S_STR_LIT : TOKEN_CHAR_LIT;

  return token;
}

Token genlex_parse_string(Lexer *l) {
  Token token = {0};

  size_t start_index = ++l->index;
  char *start = &l->content[start_index];
  while (l->content[l->index] != '"') {
    // skip over the escape quote
    if (l->content[l->index] == '\\' &&
        (l->content[l->index + 1] == '"' || l->content[l->index + 1] == '\\'))
      l->index++;
    l->index++;
    if (l->content[l->index] == '\0') {
      fprintf(stderr, "ERROR: Unclosed string literal\n");
      return (Token){0};
    }
  }

  token.value = strndup(start, l->index - start_index);
  token.type = TOKEN_STR_LIT;
  return token;
}

GenlexErr genlex_lex(Lexer *lexer) {
  for (lexer->index = 0; lexer->content[lexer->index] != '\0'; lexer->index++) {
    Token token = {0};
    switch (lexer->content[lexer->index]) {
    case '&':
      if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_AND_EQ;
        lexer->index++;
      } else if (lexer->content[lexer->index + 1] == '&') {
        token.type = TOKEN_AND;
      } else
        token.type = TOKEN_BAND;
      break;
    case '!':
      if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_NOT_EQ;
        lexer->index++;
      } else
        token.type = TOKEN_NOT;
      break;
    case '~':
      if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_BNOT_EQ;
        lexer->index++;
      } else
        token.type = TOKEN_BNOT;
      break;
    case '|':
      if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_OR_EQ;
        lexer->index++;
      } else if (lexer->content[lexer->index + 1] == '|') {
        token.type = TOKEN_OR;
      } else
        token.type = TOKEN_BOR;
      break;
    case '^':
      if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_XOR_EQ;
        lexer->index++;
      } else
        token.type = TOKEN_XOR;
      break;
    case ' ':
      continue;
    case ';':
      token.type = TOKEN_SEMI;
      break;
    case ':':
      token.type = TOKEN_COLON;
      break;
    case '?':
      token.type = TOKEN_QUESTION;
      break;
    case '.':
      token.type = TOKEN_DOT;
      break;
    case ',':
      token.type = TOKEN_COMMA;
      break;
    case '+':
      if (lexer->content[lexer->index + 1] == '+') {
        token.type = TOKEN_PLUS_PLUS;
        lexer->index++;
      } else if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_PLUS_EQ;
        lexer->index++;
      } else
        token.type = TOKEN_PLUS;
      break;
    case '-':
      if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_MINUS_EQ;
        lexer->index++;
      } else
        token.type = TOKEN_MINUS;
      break;
    case '*':
      if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_STAR_EQ;
        lexer->index++;
      } else
        token.type = TOKEN_STAR;
      break;
    case '/':
      if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_SLASH_EQ;
        lexer->index++;
      } else
        token.type = TOKEN_SLASH;
      break;
    case '%':
      if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_MOD_EQ;
        lexer->index++;
      } else
        token.type = TOKEN_MOD;
      break;
    case '\\':
      token.type = TOKEN_B_SLASH;
      break;
    case '=':
      if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_EQ_EQ;
        lexer->index++;
      } else
        token.type = TOKEN_EQ;
      break;
    case '#':
      token.type = TOKEN_HASH;
      break;
    case '(':
      token.type = TOKEN_O_PAREN;
      break;
    case ')':
      token.type = TOKEN_C_PAREN;
      break;
    case '{':
      token.type = TOKEN_O_CURL;
      break;
    case '}':
      token.type = TOKEN_C_CURL;
      break;
    case '[':
      token.type = TOKEN_O_BRACE;
      break;
    case ']':
      token.type = TOKEN_C_BRACE;
      break;
    case '<':
      if (lexer->content[lexer->index + 1] == '<') {
        token.type = TOKEN_SH_L;
        lexer->index++;
      } else if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_L_EQ;
        lexer->index++;
      } else
        token.type = TOKEN_L_THAN;
      break;
    case '>':
      if (lexer->content[lexer->index + 1] == '>') {
        token.type = TOKEN_SH_R;
        lexer->index++;
      } else if (lexer->content[lexer->index + 1] == '=') {
        token.type = TOKEN_G_EQ;
        lexer->index++;
      } else
        token.type = TOKEN_G_THAN;
      break;
    case '\'':
      token = genlex_parse_single_quotes(lexer);
      break;
    case '"':
      token = genlex_parse_string(lexer);
      break;
    case '\n':
      continue;
    default:
      if (isdigit(lexer->content[lexer->index])) {
        token = genlex_parse_num_lit(lexer);
      } else if (isalpha(lexer->content[lexer->index]) ||
                 lexer->content[lexer->index] == '_') {
        token = genlex_parse_id(lexer);
      } else {
        return GENLEX_ERR_UNKNOWN_TOKEN;
      }
    }
    DA_APPEND(&lexer->tokens, token);
  }
  return GENLEX_ERR_NONE;
}

void genlex_print_tokens(Lexer *l) {
  for (size_t i = 0; i < l->tokens.count; i++) {
    Token token = l->tokens.data[i];
    printf("type: %s, value: %s\n", genlex_type_str[token.type], token.value);
  }
}

Lexer *genlex_init_lexer(char *filename) {
  Lexer *lexer = malloc(sizeof(Lexer));
  lexer->filename = filename;
  lexer->content = genlex_read_file(lexer->filename);
  if (lexer->content == NULL) {
    return NULL;
  }
  return lexer;
}

void genlex_exit_lexer(Lexer *lexer) {
  free(lexer->content);
  for (size_t i = 0; i < lexer->tokens.count; i++) {
    if (lexer->tokens.data[i].value != NULL)
      free(lexer->tokens.data[i].value);
  }
  free(lexer->tokens.data);
}

#endif
