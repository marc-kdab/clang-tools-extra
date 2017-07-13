set(LLVM_LINK_COMPONENTS support)

add_clang_library(clangTidyQtModule
  QtTidyModule.cpp
  ModernizeQSharedPointerCreateCheck.cpp

  LINK_LIBS
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangTidy
  clangTidyUtils
  )
