#ifndef LLRACKET_SEMA_SEMA_H
#define LLRACKET_SEMA_SEMA_H

#include "llracket/AST/AST.h"

namespace llracket {

class Sema {
public:
  bool semantic(AST *Tree);
};
} // namespace llracket
#endif
