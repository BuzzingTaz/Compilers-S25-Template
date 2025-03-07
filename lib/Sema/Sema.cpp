#include "llracket/Sema/Sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace {
class ProgramCheck : public ASTVisitor {
  llvm::StringSet<> Scope;
  bool HasError;

  enum ErrorType { Twice, Not };
  void error(ErrorType ET, llvm::StringRef V) {
    llvm::errs() << "Variable " << V << " " << (ET == Twice ? "already" : "not")
                 << "declared\n";
    HasError = true;
  }

public:
  ProgramCheck() : HasError(false) {}

  bool hasError() { return HasError; }

  virtual void visit(Program &Node) override {
    if (Node.getExpr())
      Node.getExpr()->accept(*this);
    else
      HasError = true;
  };

  virtual void visit(Expr &Node) override {
    if (llvm::isa<Prim>(Node)) {
      auto &PrimNode = llvm::cast<Prim>(Node);
      if (PrimNode.getOp() == tok::kw_READ)
        return;
      if (PrimNode.getOp() == tok::minus) {
        if (PrimNode.getE1() and !PrimNode.getE2())
          PrimNode.getE1()->accept(*this);
        return;
      }
      if (PrimNode.getOp() == tok::plus || PrimNode.getOp() == tok::minus) {
        PrimNode.getE1()->accept(*this);
        PrimNode.getE2()->accept(*this);
        return;
      }
    }
    HasError = true;
  }
};
} // namespace

bool Sema::semantic(AST *Tree) {
  if (!Tree)
    return false;
  ProgramCheck Check;
  Tree->accept(Check);
  return !Check.hasError();
}
