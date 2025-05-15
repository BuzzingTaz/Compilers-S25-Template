#include "llracket/Parser/Parser.h"
#include "llracket/AST/AST.h"
#include "llracket/Lexer/Token.h"
#include "llvm/Support/Casting.h"

using namespace llracket;

AST *Parser::parse() {
  Program *P = new Program(parseExpr());
  AST *Res = llvm::dyn_cast<AST>(P);
  expect(tok::eof);
  return Res;
}

Expr *Parser::parseExpr() {
  auto ErrorHandler = [this]() {
    Diags.report(Tok.getLocation(), diag::err_unexpected_token, Tok.getText());
    skipUntil(tok::r_paren);
    return nullptr;
  };

  if (Tok.is(tok::integer_literal)) {
    Int *Ret = new Int(Tok.getText());
    advance();
    return Ret;
  }

  if (!consume(tok::l_paren))
    return ErrorHandler();

  if (Tok.is(tok::read)) {
    advance();
    if (!consume(tok::r_paren))
      return ErrorHandler();
    return new Prim(tok::read);
  }

  if (Tok.is(tok::plus)) {
    advance();
    Expr *E1 = parseExpr();
    Expr *E2 = parseExpr();
    if (!consume(tok::r_paren))
      return ErrorHandler();
    return new Prim(tok::plus, E1, E2);
  }
  if (Tok.is(tok::minus)) {
    advance();

    Expr *E1 = parseExpr();

    if (Tok.is(tok::r_paren)) {
      advance();
      return new Prim(tok::minus, E1);
    }

    Expr *E2 = parseExpr();
    if (!consume(tok::r_paren))
      return ErrorHandler();
    return new Prim(tok::minus, E1, E2);
  }
  return ErrorHandler();
}
