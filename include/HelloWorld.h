#ifndef __HELLO_WORLD_PASS_H__
#define __HELLO_WORLD_PASS_H__

#include "llvm/Config/llvm-config.h"

#if LLVM_VERSION_MAJOR >= 11
#include "llvm/IR/PassManager.h"
#else
#include "llvm/IR/LegacyPassManager.h"
#endif
#include "llvm/Pass.h"

namespace {

#if LLVM_VERSION_MAJOR >= 11
class HelloWorld : public llvm::PassInfoMixin<HelloWorld> {
public:
  HelloWorld() {};
#else
class HelloWorld : public llvm::ModulePass {
public:
  static char ID;
  HelloWorld() : llvm::ModulePass(ID) {}
#endif

#if LLVM_VERSION_MAJOR >= 11
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);
#else
  bool runOnModule(llvm::Module &M) override;
#endif
};

}

#endif