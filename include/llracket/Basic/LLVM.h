#ifndef LLRACKET_BASIC_LLVM_H
#define LLRACKET_BASIC_LLVM_H

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

namespace llracket {
using llvm::cast;
using llvm::cast_or_null;
using llvm::dyn_cast;
using llvm::dyn_cast_or_null;
using llvm::isa;

using llvm::raw_ostream;
using llvm::SMLoc;
using llvm::SourceMgr;
using llvm::StringMap;
using llvm::StringRef;
} // namespace llracket

#endif
