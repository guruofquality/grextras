// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/clang_block.hpp>

using namespace grextras;

ClangBlockParams::ClangBlockParams(void)
{
    //NOP
}

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
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <fstream>

static std::string extern_c_fcn_name(const std::string &name)
{
    return name + "__extern_c";
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
        "void *%s(void){return %s();}"
        "}\n"
    ) % extern_c_fcn_name(params.name) % params.name;
    source_fstream.close();

    //begin command setup
    std::vector<std::string> cmd;
    cmd.push_back(BOOST_STRINGIZE(CLANG_EXECUTABLE));
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

    //format command string
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
    std::ifstream bitcode_fstream(bitcode_file);
    return std::string((std::istreambuf_iterator<char>(bitcode_fstream)), std::istreambuf_iterator<char>());
}

struct WeakContainerClangBlock : gras::WeakContainer
{
    boost::shared_ptr<const void> lock(void)
    {
        return weak_self.lock();
    }
    boost::weak_ptr<const void> weak_self;
    boost::shared_ptr<llvm::MemoryBuffer> buffer;
    boost::shared_ptr<llvm::Module> module;
    boost::shared_ptr<llvm::ExecutionEngine> ee;
    boost::shared_ptr<llvm::Function> func;
};

boost::shared_ptr<gras::Block> ClangBlock::make(const ClangBlockParams &params)
{
    const std::string bitcode = call_clang(params);

    llvm::InitializeNativeTarget();
    llvm::llvm_start_multithreaded();
    llvm::LLVMContext context;

    //create a memory buffer from the bitcode
    boost::shared_ptr<llvm::MemoryBuffer> buffer(llvm::MemoryBuffer::getMemBuffer(bitcode));

    //parse the bitcode into a module
    std::string error;
    boost::shared_ptr<llvm::Module> module(llvm::ParseBitcodeFile(buffer.get(), context, &error));
    if (not error.empty()) throw std::runtime_error("ClangBlock: ParseBitcodeFile " + error);

    //create execution engine and function
    boost::shared_ptr<llvm::ExecutionEngine> ee(llvm::ExecutionEngine::create(module.get()));
    const std::string c_function_name = extern_c_fcn_name(params.name);
    boost::shared_ptr<llvm::Function> func(ee->FindFunctionNamed(c_function_name.c_str()));

    //call into the function
    typedef void * (*PFN)();
    PFN pfn = reinterpret_cast<PFN>(ee->getPointerToFunction(func.get()));
    void *block = pfn();

    //inject container to hold onto block and execution unit
    boost::shared_ptr<gras::Block> block_sptr(reinterpret_cast<gras::Block *>(block));
    WeakContainerClangBlock *weak_container = new WeakContainerClangBlock();
    weak_container->weak_self = block_sptr;
    weak_container->buffer = buffer;
    weak_container->module = module;
    weak_container->ee = ee;
    weak_container->func = func;
    block_sptr->set_container(weak_container);
    return block_sptr;
}

#else

boost::shared_ptr<gras::Block> ClangBlock::make(const ClangBlockParams &)
{
    return boost::shared_ptr<gras::Block>();
}

#endif //HAVE_CLANG
