#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

#include "LoopIndVarBound.h"

using namespace llvm;

LoopIndVarBound::LoopIndVarBound() {}

PreservedAnalyses LoopIndVarBound::run(Module &M,
                                       ModuleAnalysisManager &MAM) {

  auto &FAM = MAM.getResult<FunctionAnalysisManagerModuleProxy>(M).getManager();
  std::string out_str;

  for (auto &F : M) {
    if (F.isDeclaration() || F.isDebugInfoForProfiling()) {
      continue;
    }
    auto &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
    auto &LI = FAM.getResult<LoopAnalysis>(F);

    raw_string_ostream sos(out_str);
    for (auto L : LI) {
      auto Header = L->getHeader();
      PHINode *PHI = nullptr;
      for (auto &I : *Header) {
        if (isa<PHINode>(&I)) {
          PHI = cast<PHINode>(&I);
          break;
        }
      }
      if (!PHI) {
        continue;
      }

      auto SCEV = SE.getSCEV(PHI);
      if (!SCEV) {
        continue;
      }

      if (!SE.containsAddRecurrence(SCEV)) {
        continue;
      }

      sos << "PHI: ";
      PHI->print(sos);

      if (PHI->hasMetadata(LLVMContext::MD_dbg)) {
        sos << ", at: ";
        PHI->getDebugLoc().print(sos);
      }

      sos << "\n";

      sos << "SCEV: ";
      SCEV->print(sos);
      sos << "\n";

      for (auto LB : L->blocks()) {
        for (auto &II : *LB) {
          if (CmpInst *CI = dyn_cast<CmpInst>(&II)) {
            if (CI->getOperand(0) == PHI || CI->getOperand(1) == PHI) {
              if (L->isLoopExiting(LB)) {
                sos << "exiting cond: ";
                CI->print(sos);
                sos << "\n";
              }
            }
          }
        }
      }

      // auto IV = L->getInductionVariable(SE);
      // if (IV) {
      //   sos << "IV: ";
      //   IV->print(sos);
      //   sos << "\n";
      // }
      // auto Bound = L->getBounds(SE);
      // if (!Bound) {
      //   continue;
      // }
    
      // sos << "IV Initial Value: ";
      // Bound->getInitialIVValue().print(sos);
      // sos << "\n";
      // if (Bound->getStepValue()) {
      //   sos << "Step Value: ";
      //   Bound->getStepValue()->print(sos);
      //   sos << "\n";
      // }
      // sos << "IV Final Value: ";
      // Bound->getFinalIVValue().print(sos);
      // sos << "\n";
      // sos << "Predicate: " << Bound->getCanonicalPredicate() << "\n-----------------------\n";
    }

    if (out_str.empty()) {
      continue;
    }
    std::string func_str;
    raw_string_ostream fos(func_str);
    fos << "###############################################\n";
    F.print(fos);
    fos << "\n-----------------------\n";
    errs() << func_str + out_str << "\n";
    out_str.clear();
  }
  
  return PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "LoopIndVarBound", "v0.1",
    [](PassBuilder &PB) {
      PB.registerOptimizerLastEPCallback(
        [](ModulePassManager &MPM, OptimizationLevel OL) {
          MPM.addPass(LoopIndVarBound());
        }
      );
    }
  };
}