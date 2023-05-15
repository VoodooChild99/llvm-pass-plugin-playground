#ifndef __LOOP_IND_VAR_BOUND_H__
#define __LOOP_IND_VAR_BOUND_H__

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

namespace {

class LoopIndVarBound : public llvm::PassInfoMixin<LoopIndVarBound> {
public:
  LoopIndVarBound();
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);
};

}

#endif