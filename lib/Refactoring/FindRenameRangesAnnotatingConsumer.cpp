//===----------------------------------------------------------------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2023 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#include "RefactoringActions.h"

using namespace swift::refactoring;

static StringRef tag(RefactoringRangeKind Kind) {
  switch (Kind) {
  case RefactoringRangeKind::BaseName:
    return "base";
  case RefactoringRangeKind::KeywordBaseName:
    return "keywordBase";
  case RefactoringRangeKind::ParameterName:
    return "param";
  case RefactoringRangeKind::NoncollapsibleParameterName:
    return "noncollapsibleparam";
  case RefactoringRangeKind::DeclArgumentLabel:
    return "arglabel";
  case RefactoringRangeKind::CallArgumentLabel:
    return "callarg";
  case RefactoringRangeKind::CallArgumentColon:
    return "callcolon";
  case RefactoringRangeKind::CallArgumentCombined:
    return "callcombo";
  case RefactoringRangeKind::SelectorArgumentLabel:
    return "sel";
  }
  llvm_unreachable("unhandled kind");
}

swift::ide::FindRenameRangesAnnotatingConsumer::
    FindRenameRangesAnnotatingConsumer(SourceManager &SM, unsigned BufferId,
                                       raw_ostream &OS)
    : pRewriter(new SourceEditOutputConsumer(SM, BufferId, OS)) {}

void swift::ide::FindRenameRangesAnnotatingConsumer::accept(
    SourceManager &SM, RegionType RegionType,
    ArrayRef<RenameRangeDetail> Ranges) {
  if (RegionType == RegionType::Mismatch || RegionType == RegionType::Unmatched)
    return;
  for (const auto &Range : Ranges) {
    std::string NewText;
    llvm::raw_string_ostream OS(NewText);
    StringRef Tag = tag(Range.RangeKind);
    OS << "<" << Tag;
    if (Range.Index.has_value())
      OS << " index=" << *Range.Index;
    OS << ">" << Range.Range.str() << "</" << Tag << ">";
    Replacement Repl{/*Path=*/{}, Range.Range, /*BufferName=*/{}, OS.str(),
                     /*RegionsWorthNote=*/{}};
    pRewriter->accept(SM, Repl);
  }
}
