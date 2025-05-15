#ifndef LLRACKET_PARSER_PARSER_H
#define LLRACKET_PARSER_PARSER_H

#include "llracket/AST/AST.h"
#include "llracket/Basic/Diagnostic.h"
#include "llracket/Lexer/Lexer.h"
#include <vector>

namespace llracket {

class Parser {
  Lexer &Lex;
  Token Tok;
  DiagnosticsEngine &Diags;

  // {Received Token, Expected Token}
  std::vector<std::pair<tok::TokenKind, tok::TokenKind>> UnexpectedTokens;

  void error(tok::TokenKind Kind) {
    UnexpectedTokens.push_back({Tok.getKind(), Kind});
  }

  void advance() { Lex.next(Tok); }
  void advance(unsigned N) {
    for (unsigned I = 0; I < N; I++) {
      if (Tok.getKind() == tok::eof) {
        break;
      }
      advance();
    }
  }

  bool expect(tok::TokenKind Kind) {
    if (Tok.getKind() != Kind) {
      error(Kind);
      return false;
    }
    return true;
  }

  bool consume(tok::TokenKind Kind) {
    if (!expect(Kind))
      return false;
    advance();
    return true;
  }

  Expr *parseExpr();

public:
  Parser(Lexer &Lex, DiagnosticsEngine &Diags) : Lex(Lex), Diags(Diags) {
    advance();
  }

  AST *parse();

  template <class... Tokens> void skipUntil(Tokens... Toks) {
    std::unordered_set<tok::TokenKind> Skipset = {tok::eof, Toks...};
    while (true) {
      if (Skipset.count(Tok.getKind())) {
        break;
      }
      advance();
    }
  }
};
} // namespace llracket
#endif
