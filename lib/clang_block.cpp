// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/clang_block.hpp>

#ifdef HAVE_CLANG

#include <llvm/LLVMContext.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/system_error.h>

#include <boost/foreach.hpp>
#include <iostream>
#include <cstdio>
#include <fstream>

using namespace grextras;

ClangBlockParams::ClangBlockParams(void)
{
    //NOP
}

//clang -emit-llvm -c /home/jblum/Desktop/foo.cpp -o /home/jblum/Desktop/foo.bc -I /opt/usr/gras/include/


static std::string call_clang(const ClangBlockParams &params)
{
    //make up bitcode file path
    char bitcode_file[L_tmpnam];
    std::tmpnam(bitcode_file);

    //write source to tmp file
    char source_file[L_tmpnam];
    std::tmpnam(source_file);
    std::ofstream source_fstream(source_file);
    source_fstream << params.code;
    source_fstream.close();

    //begin command setup
    std::vector<std::string> cmd;
    cmd.push_back("clang");
    cmd.push_back("-emit-llvm");

    //inject source
    cmd.push_back("-c");
    cmd.push_back("-x");
    cmd.push_back("c++");
    cmd.push_back(source_file);

    //inject output
    cmd.push_back("-o");
    cmd.push_back(bitcode_file);

    //inject args...
    BOOST_FOREACH(const std::string &flag, params.flags)
    {
        cmd.push_back(flag.c_str());
    }
    BOOST_FOREACH(const std::string &include_dir, params.include_dirs)
    {
        cmd.push_back("-I");
        cmd.push_back(include_dir.c_str());
    }
    BOOST_FOREACH(const std::string &library_dir, params.library_dirs)
    {
        cmd.push_back("-L");
        cmd.push_back(library_dir.c_str());
    }
    BOOST_FOREACH(const std::string &library, params.libraries)
    {
        cmd.push_back("-l");
        cmd.push_back(library.c_str());
    }

    std::cout << __LINE__ << std::endl;
    std::string command;
    BOOST_FOREACH(const std::string &c, cmd)
    {
        command += c + " ";
    }
    std::cout << command << std::endl;
    const int ret = system(command.c_str());
    if (ret != 0)
    {
        throw std::runtime_error("ClangBlock: error system exec clang");
    }

    //readback bitcode for result
    std::cout << __LINE__ << std::endl;
    std::ifstream bitcode_fstream(bitcode_file);
    return std::string((std::istreambuf_iterator<char>(bitcode_fstream)), std::istreambuf_iterator<char>());
}

boost::shared_ptr<gras::Block> ClangBlock::make(const ClangBlockParams &params)
{

    const std::string bitcode = call_clang(params);

    std::cout << __LINE__ << std::endl;
    llvm::InitializeNativeTarget();
    std::cout << __LINE__ << std::endl;
    llvm::llvm_start_multithreaded();
    std::cout << __LINE__ << std::endl;
    llvm::LLVMContext context;
    std::cout << __LINE__ << std::endl;
    std::string error;
    //llvm::OwningPtr< llvm::MemoryBuffer > buffer;
    std::cout << __LINE__ << std::endl;
    llvm::OwningPtr< llvm::MemoryBuffer> buffer(llvm::MemoryBuffer::getMemBuffer(bitcode));
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

#else

boost::shared_ptr<gras::Block> ClangBlock::make(const ClangBlockParams &)
{
    return boost::shared_ptr<gras::Block>();
}

#endif //HAVE_CLANG
