set(LLVM_LINK_COMPONENTS Support)

add_llvm_executable(llracket LLRacket.cpp)

target_link_libraries(llracket PRIVATE llracketBasic llracketLexer
                                       llracketParser llracketSema)
