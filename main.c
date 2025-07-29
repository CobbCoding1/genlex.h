#define GENLEX_IMPLEMENTATION
#include "genlex.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "USAGE: %s <filename>\n", argv[0]);
    exit(1);
  }
  Lexer *lexer = genlex_init_lexer(argv[1]);

  if (lexer == NULL) {
    fprintf(stderr, "ERROR: Could not init lexer\n");
    exit(1);
  }

  GenlexErr err = genlex_lex(lexer);
  if (err != GENLEX_ERR_NONE) {
    fprintf(stderr, "ERROR: Could not lex file: %s\n", genlex_err_str[err]);
    exit(1);
  }

  genlex_print_tokens(lexer);

  genlex_exit_lexer(lexer);
  return 0;
}
