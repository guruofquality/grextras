// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/clang_block.hpp>

#include <llvm/LLVMContext.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/system_error.h>

#include <iostream>

using namespace grextras;

ClangBlockParams::ClangBlockParams(void)
{
    //NOP
}

//clang -emit-llvm -c /home/jblum/Desktop/foo.cpp -o /home/jblum/Desktop/foo.bc -I /opt/usr/gras/include/


boost::shared_ptr<gras::Block> ClangBlock::make(const ClangBlockParams &params)
{
    std::cout << __LINE__ << std::endl;
    llvm::InitializeNativeTarget();
    std::cout << __LINE__ << std::endl;
    llvm::llvm_start_multithreaded();
    std::cout << __LINE__ << std::endl;
    llvm::LLVMContext context;
    std::cout << __LINE__ << std::endl;
    std::string error;
    llvm::OwningPtr< llvm::MemoryBuffer > buffer;
    std::cout << __LINE__ << std::endl;
    llvm::MemoryBuffer::getFile("/home/jblum/Desktop/foo.bc", buffer);
    std::cout << __LINE__ << std::endl;
    llvm::Module *m = llvm::ParseBitcodeFile(buffer.get(), context, &error);
    std::cout << __LINE__ << std::endl;
    llvm::ExecutionEngine *ee = llvm::ExecutionEngine::create(m);
    std::cout << __LINE__ << std::endl;

    llvm::Function* func = ee->FindFunctionNamed("main");
    std::cout << __LINE__ << std::endl;

    typedef int (*PFN)();
    PFN pfn = reinterpret_cast<PFN>(ee->getPointerToFunction(func));
    std::cout << pfn() << std::endl;
    delete ee;
}
