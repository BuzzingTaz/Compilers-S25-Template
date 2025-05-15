#include "llracket/CodeGen/CodeGen.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

using namespace llracket;

namespace {
class ToIRVisitor : public ASTVisitor {
  llvm::Module *M;
  llvm::IRBuilder<> Builder;
  llvm::Type *VoidTy;
  llvm::Type *Int32Ty;
  llvm::PointerType *PtrTy;
  llvm::Constant *Int32Zero;
  llvm::Value *V;
  StringMap<llvm::Value *> nameMap;

public:
  ToIRVisitor(llvm::Module *M) : M(M), Builder(M->getContext()) {
    VoidTy = llvm::Type::getVoidTy(M->getContext());
    Int32Ty = llvm::Type::getInt32Ty(M->getContext());
    PtrTy = llvm::PointerType::getUnqual(M->getContext());
    Int32Zero = llvm::ConstantInt::get(Int32Ty, 0, true);
  }

  void run(AST *Tree) {
    llvm::FunctionType *MainFty =
        llvm::FunctionType::get(Int32Ty, {Int32Ty, PtrTy}, false);
    llvm::Function *MainFn = llvm::Function::Create(
        MainFty, llvm::GlobalValue::ExternalLinkage, "main", M);
    llvm::BasicBlock *BB =
        llvm::BasicBlock::Create(M->getContext(), "entry", MainFn);
    Builder.SetInsertPoint(BB);
    Tree->accept(*this);

    llvm::FunctionType *WriteFnTy =
        llvm::FunctionType::get(VoidTy, {Int32Ty}, false);
    llvm::Function *WriteFn = llvm::Function::Create(
        WriteFnTy, llvm::GlobalValue::ExternalLinkage, "write_int", M);
    Builder.CreateCall(WriteFnTy, WriteFn, {V});
    Builder.CreateRet(Int32Zero);
  }

  virtual void visit(Program &Node) override { Node.getExpr()->accept(*this); };

  virtual void visit(Expr &Node) override {
    if (llvm::isa<Prim>(Node)) {
      Node.accept(*this);
      return;
    }
    if (llvm::isa<Int>(Node)) {
      Node.accept(*this);
      return;
    }
  };

  virtual void visit(Prim &Node) override {
    if (Node.getOp() == tok::read) {
      llvm::Function *ReadFn;
      if ((ReadFn = M->getFunction("read_int")) == nullptr) {
        llvm::FunctionType *ReadFty =
            llvm::FunctionType::get(Int32Ty, {PtrTy}, false);
        ReadFn = llvm::Function::Create(
            ReadFty, llvm::GlobalValue::ExternalLinkage, "read_int", M);
      }
      llvm::AllocaInst *ReadInput =
          Builder.CreateAlloca(PtrTy, nullptr, "read_input");
      V = Builder.CreateCall(ReadFn, {ReadInput});
      return;
    }
    if (Node.getOp() == tok::minus) {
      if (Node.getE1() and !Node.getE2()) {
        Node.getE1()->accept(*this);
        V = Builder.CreateNSWNeg(V);
        return;
      }
    }
    if (Node.getOp() == tok::plus || Node.getOp() == tok::minus) {
      Node.getE1()->accept(*this);
      llvm::Value *E1 = V;
      Node.getE2()->accept(*this);
      llvm::Value *E2 = V;
      if (Node.getOp() == tok::plus) {
        V = Builder.CreateNSWAdd(E1, E2);
      } else {
        V = Builder.CreateNSWSub(E1, E2);
      }
      return;
    }
  };

  virtual void visit(Int &Node) override {
    int Intval;
    Node.getValue().getAsInteger(10, Intval);
    V = llvm::ConstantInt::get(Int32Ty, Intval, true);
  };
};
}; // namespace

void CodeGen::compile(AST *Tree) {
  ToIRVisitor ToIR(M);
  ToIR.run(Tree);
}
