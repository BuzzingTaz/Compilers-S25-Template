#ifndef LLRACKET_LEXER_TOKENKIND_H
#define LLRACKET_LEXER_TOKENKIND_H

#include "llracket/Basic/LLVM.h"
#include "llracket/Basic/TokenKinds.h"
#include <unordered_set>

namespace llracket {

class Token {
  friend class Lexer;

private:
  tok::TokenKind Kind;
  StringRef Text;

public:
  tok::TokenKind getKind() const { return Kind; }
  StringRef getText() const { return Text; }
  size_t getlength() const { return Text.size(); }

  bool is(tok::TokenKind K) const { return Kind == K; }
  bool is(std::unordered_set<tok::TokenKind> K) const { return K.count(Kind); }
  template <typename... Ts> bool isOneOf(Ts... Ks) const {
    return (... || is(Ks));
  }

  SMLoc getLocation() const { return SMLoc::getFromPointer(Text.data()); }

  StringRef getLiteral() {
    assert(is(tok::integer_literal) and "Not a literal");
    return Text;
  }
};
} // namespace llracket
#endif
