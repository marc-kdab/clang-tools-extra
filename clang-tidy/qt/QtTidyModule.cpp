//===------- BoostTidyModule.cpp - clang-tidy -----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "../ClangTidy.h"
#include "../ClangTidyModule.h"
#include "../ClangTidyModuleRegistry.h"

#include "ModernizeQSharedPointerCreateCheck.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace qt {

class QtModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<modernize::QSharedPointerCreateCheck>("qt-modernize-qsharedpointer-create");
  }
};

// Register the QtModule using this statically initialized variable.
static ClangTidyModuleRegistry::Add<QtModule> X("qt-module", "Add Qt checks.");

} // namespace boost

// This anchor is used to force the linker to link in the generated object file
// and thus register the QtModule.
volatile int QtModuleAnchorSource = 0;

} // namespace tidy
} // namespace clang
