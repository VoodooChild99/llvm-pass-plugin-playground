#include "HelloWorld.h"

#if LLVM_VERSION_MAJOR >= 11
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#else
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#endif
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#if LLVM_VERSION_MAJOR >= 11
PreservedAnalyses HelloWorld::run(Module &M, ModuleAnalysisManager &MAM) {
#else
bool HelloWorld::runOnModule(Module &M) {
#endif
  for (auto &F : M) {
    outs() << F.getName() << "\n";
  }
#if LLVM_VERSION_MAJOR >= 11
  return PreservedAnalyses::all();
#else
  return true;
#endif
}

#if LLVM_VERSION_MAJOR >= 11
extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "HelloWorld", "v0.1",
    [](PassBuilder &PB) {
#if LLVM_VERSION_MAJOR <= 13
      using OptimizationLevel = typename PassBuilder::OptimizationLevel;
#endif
      PB.registerOptimizerLastEPCallback(
        [](ModulePassManager &MPM, OptimizationLevel OL) {
          MPM.addPass(HelloWorld());
        }
      );
    }
  };
}
#else
char HelloWorld::ID = 0;
static void registerHelloWorldPass(const PassManagerBuilder &,
                                   legacy::PassManagerBase &PM) {
  PM.add(new HelloWorld());
}

static RegisterStandardPasses RegisterHelloWorldPass(
    PassManagerBuilder::EP_OptimizerLast, registerHelloWorldPass);
#endif