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
#include <llvm/Module.h>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <cstdio>
#include <fstream>

using namespace grextras;

ClangBlockParams::ClangBlockParams(void)
{
    //NOP
}

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
    //inject the c wrapper
    source_fstream << boost::format(
        "\n"
        "extern \"C\" {\n"
        "void *__%s__(void){return %s();}"
        "}\n"
    ) % params.name % params.name;
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

    llvm::InitializeNativeTarget();
    llvm::llvm_start_multithreaded();

    //create a memory buffer from the bitcode
    llvm::OwningPtr<llvm::MemoryBuffer> buffer(llvm::MemoryBuffer::getMemBuffer(bitcode));

    //parse the bitcode into a module
    std::string error;
    llvm::LLVMContext context;
    llvm::OwningPtr<llvm::Module> m(llvm::ParseBitcodeFile(buffer.get(), context, &error));
    if (not error.empty()) throw std::runtime_error("ClangBlock: ParseBitcodeFile " + error);

    //create execution engine and function
    llvm::OwningPtr<llvm::ExecutionEngine> ee(llvm::ExecutionEngine::create(m.get()));
    const std::string c_function_name = "__" + params.name + "__";
    llvm::OwningPtr<llvm::Function> func(ee->FindFunctionNamed(c_function_name.c_str()));

    //call into the function
    typedef void * (*PFN)();
    PFN pfn = reinterpret_cast<PFN>(ee->getPointerToFunction(func.get()));
    std::cout << __LINE__ << std::endl;
    void *block = pfn();
    std::cout << __LINE__ << std::endl;

    std::cout << reinterpret_cast<gras::Block *>(block)->to_string() << std::endl;
    //TODO needs to hold a container w/ this llvm containers

    return boost::shared_ptr<gras::Block>(reinterpret_cast<gras::Block *>(block));
}

#else

boost::shared_ptr<gras::Block> ClangBlock::make(const ClangBlockParams &)
{
    return boost::shared_ptr<gras::Block>();
}

#endif //HAVE_CLANG
