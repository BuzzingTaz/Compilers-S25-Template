#ifndef LLRACKET_BASIC_DIAGNOSTIC_H
#define LLRACKET_BASIC_DIAGNOSTIC_H

#include "llracket/Basic/LLVM.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/FormatVariadic.h"


namespace llracket {

namespace diag {
enum {
#define DIAG(ID, Level, Msg) ID,
#include "llracket/Basic/Diagnostic.def"
};
} // namespace diag

class DiagnosticsEngine {
  static const char *getDiagnosticText(unsigned DiagID);
  static SourceMgr::DiagKind getDiagnosticKind(unsigned DiagID);
  SourceMgr &SrcMgr;
  unsigned NumErrors;

public:
  DiagnosticsEngine(SourceMgr &SrcMgr) : SrcMgr(SrcMgr), NumErrors(0) {}

  unsigned numErrors() { return NumErrors; }
  template <typename... Args>
  void report(SMLoc Loc, unsigned DiagID, Args &&...Arguments) {
    std::string Msg = llvm::formatv(getDiagnosticText(DiagID), Arguments...).str();
    SourceMgr::DiagKind Kind = getDiagnosticKind(DiagID);
    SrcMgr.PrintMessage(Loc, Kind, Msg);
    NumErrors += (Kind == SourceMgr::DK_Error);
  }
};
} // namespace llracket

#endif
