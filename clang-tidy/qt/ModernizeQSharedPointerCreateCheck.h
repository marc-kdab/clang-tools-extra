//===--- MakeSharedCheck.h - clang-tidy--------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MODERNIZE_MAKE_SHARED_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MODERNIZE_MAKE_SHARED_H

#include "../modernize/MakeSmartPtrCheck.h"

namespace clang {
namespace tidy {
namespace qt {
namespace modernize {

/// Replace the pattern:
/// \code
///   QSharedPointer<type>(new type(args...))
/// \endcode
///
/// With the safer version:
/// \code
///   QSharedPointer<type>::create(args...)
/// \endcode
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/qt-modernize-qsharedpointer-create.html
class QSharedPointerCreateCheck : public tidy::modernize::MakeSmartPtrCheck {
public:
  explicit QSharedPointerCreateCheck(StringRef Name, ClangTidyContext *Context);

protected:
  SmartPtrTypeMatcher getSmartPointerTypeMatcher() const override;
};

} // namespace modernize
} // namespace qt
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MODERNIZE_MAKE_SHARED_H
