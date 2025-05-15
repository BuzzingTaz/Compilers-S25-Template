#include "llracket/Lexer/Lexer.h"

using namespace llracket;

namespace charinfo {
LLVM_READNONE inline static bool isWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\f' || c == '\v' || c == '\r' ||
         c == '\n';
}
LLVM_READNONE inline static bool isDigit(char c) {
  return c >= '0' && c <= '9';
}
LLVM_READNONE inline static bool isLetter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

LLVM_READNONE inline static bool isAlphanumeric(char c) {
  return isLetter(c) || isDigit(c);
}

LLVM_READNONE inline static bool isAlphanumeric_(char c) {
  return isAlphanumeric(c) || c == '_';
}
} // namespace charinfo

void Lexer::next(Token &token) {
  while (*BufferPtr && charinfo::isWhitespace(*BufferPtr))
    ++BufferPtr;

  if (!*BufferPtr) {
    token.Kind = tok::eof;
    return;
  }

  if (charinfo::isDigit(*BufferPtr)) {
    const char *End = BufferPtr + 1;
    while (charinfo::isDigit(*End))
      ++End;
    formToken(token, End, tok::integer_literal);
    return;
  }
  if (charinfo::isLetter(*BufferPtr)) {
    const char *End = BufferPtr + 1;
    while (charinfo::isAlphanumeric_(*End))
      ++End;

    llvm::StringRef Text(BufferPtr, End - BufferPtr);
    if (Text == "read") {
      formToken(token, End, tok::read);
      return;
    }
    formToken(token, End, tok::unknown);
    return;
  }

  switch (*BufferPtr) {
#define CASE(ch, tok)                                                          \
  case ch:                                                                     \
    formToken(token, BufferPtr + 1, tok);                           \
    break;

    CASE('+', tok::plus);
    CASE('-', tok::minus);
    CASE('(', tok::l_paren);
    CASE(')', tok::r_paren);
#undef CASE

  default:
    Diags.report(getLoc(), diag::err_unknown_token, *BufferPtr);
    formToken(token, BufferPtr + 1, tok::unknown);
    break;
  }
  return;
}

void Lexer::formToken(Token &Tok, const char *TokEnd, tok::TokenKind Kind) {
  Tok.Kind = Kind;
  Tok.Text = StringRef(BufferPtr, TokEnd - BufferPtr);
  BufferPtr = TokEnd;
}
