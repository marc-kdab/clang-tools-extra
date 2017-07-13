//===--- ModernizeQSharedPointerCreateCheck.cpp - clang-tidy----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ModernizeQSharedPointerCreateCheck.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace qt {
namespace modernize {

QSharedPointerCreateCheck::QSharedPointerCreateCheck(StringRef Name, ClangTidyContext *Context)
    : MakeSmartPtrCheck(Name, Context, "create", "QSharedPointer", CreateFunctionIsMember) {}

auto QSharedPointerCreateCheck::getSmartPointerTypeMatcher() const -> SmartPtrTypeMatcher {
  return qualType(hasDeclaration(classTemplateSpecializationDecl(
      hasName("::QSharedPointer"), templateArgumentCountIs(1),
      hasTemplateArgument(
          0, templateArgument(refersToType(qualType().bind(PointerType)))))));
}

} // namespace modernize
} // namespace qt
} // namespace tidy
} // namespace clang
