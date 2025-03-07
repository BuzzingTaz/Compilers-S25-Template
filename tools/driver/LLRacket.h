#ifndef RACKETLLVM_H
#define RACKETLLVM_H

#include "llracket/Basic/Diagnostic.h"
#include "llracket/Lexer/Lexer.h"
#include "llracket/Parser/Parser.h"
#include "llracket/Sema/Sema.h"
#include "llvm/ADT/StringRef.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <system_error>

class LLRacket {
  SourceMgr *SrcMgr;
  DiagnosticsEngine Diags;

public:
  LLRacket(SourceMgr *SrcMgr, DiagnosticsEngine Diags)
      : SrcMgr(SrcMgr), Diags(Diags) {
    moduleInit();
  };

  SourceMgr *getSourceMgr() { return SrcMgr; }

  void exec() {
    // Parse the program to AST
    Lexer Lex(*SrcMgr, Diags);
    Parser P(Lex);
    AST *Tree = P.parse();
    if (!Tree || Diags.numErrors()) {
      llvm::errs() << "Syntax error\n";
      return;
    }

    // Semantic analysis
    Sema S;
    if (!S.semantic(Tree)) {
      llvm::errs() << "Semantic error\n";
      return;
    }

    // Compile to LLVM IR

    Module->print(llvm::outs(), nullptr);
    // save to a .ll file
    saveModuleToFile("./out.ll");
    return;
  }

private:
  std::unique_ptr<llvm::LLVMContext> Ctx;
  std::unique_ptr<llvm::Module> Module;
  std::unique_ptr<llvm::IRBuilder<>> Builder;

  void moduleInit() {
    Ctx = std::make_unique<llvm::LLVMContext>();
    Module = std::make_unique<llvm::Module>("RacketLLVM", *Ctx);
  }

  void saveModuleToFile(llvm::StringRef FileName) {
    std::error_code ErrorCode;
    llvm::raw_fd_ostream OutLl(FileName, ErrorCode);
    Module->print(OutLl, nullptr);
  }
};
#endif
