#include "LLRacket.h"
#include "llracket/Basic/Diagnostic.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/SourceMgr.h>

static llvm::cl::opt<std::string> Input(llvm::cl::Positional,
                                        llvm::cl::desc("<input file>"),
                                        llvm::cl::Required);

int main(int argc_, const char **argv_) {
  llvm::InitLLVM X(argc_, argv_);
  llvm::cl::ParseCommandLineOptions(argc_, argv_, "LLRacket compiler\n");

  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> FileOrErr =
      llvm::MemoryBuffer::getFile(Input);

  if (std::error_code BufferError = FileOrErr.getError()) {
    llvm::errs() << "Error reading " << Input << ": " << BufferError.message()
                 << "\n";
    return 1;
  }

  // Source manager class to manage source buffers
  llvm::SourceMgr *SrcMgr = new llvm::SourceMgr();
  DiagnosticsEngine Diags(*SrcMgr);
  LLRacket Compiler(SrcMgr, Diags);
  Compiler.getSourceMgr()->AddNewSourceBuffer(std::move(*FileOrErr),
                                              llvm::SMLoc());

  Compiler.exec();

  return 0;
}
