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
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/system_error.h>
#include <llvm/Module.h>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/thread/mutex.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <map>

/***********************************************************************
 * Map the C++ factory function to the generated extern C function
 **********************************************************************/
static std::string extern_c_fcn_name(const std::string &name)
{
    return name + "__extern_c";
}

/***********************************************************************
 * Helper function to call a clang compliation
 **********************************************************************/
static std::string call_clang(const ClangBlockParams &params)
{
    //make up bitcode file path
    char bitcode_file[L_tmpnam];
    std::tmpnam(bitcode_file);

    //write source to tmp file
    char source_file[L_tmpnam];
    std::tmpnam(source_file);
    std::ofstream source_fstream(source_file);
    source_fstream << params.source;

    //inject the c wrapper
    source_fstream << boost::format(
        "                                                               \n"
        "extern \"C\" void *%s(void)                                    \n"
        "{                                                              \n"
        "    return %s();                                               \n"
        "}                                                              \n"
        "                                                               \n"
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
    BOOST_FOREACH(const std::string &include, params.includes)
    {
        cmd.push_back("-I");
        cmd.push_back(include.c_str());
    }

    //format command string
    std::string command;
    BOOST_FOREACH(const std::string &c, cmd)
    {
        command += c + " ";
    }
    std::cout << "  " << command << std::endl;
    const int ret = system(command.c_str());
    if (ret != 0)
    {
        throw std::runtime_error("ClangBlock: error system exec clang");
    }

    //readback bitcode for result
    std::ifstream bitcode_fstream(bitcode_file);
    return std::string((std::istreambuf_iterator<char>(bitcode_fstream)), std::istreambuf_iterator<char>());
}

/***********************************************************************
 * Map blocks allocated by LLVM to the execution engine:
 * The special block deleter will also free the engine.
 **********************************************************************/
static std::map<void *, boost::shared_ptr<const void> > block_ptr_to_engine;

static boost::mutex engine_map_mutex;

static void delete_clang_block(void *block)
{
    boost::mutex::scoped_lock l(engine_map_mutex);
    delete reinterpret_cast<gras::Block *>(block);
    block_ptr_to_engine.at(block).reset();
    block_ptr_to_engine.erase(block);
}

/***********************************************************************
 * Lazy static initializer fpr LLVM context:
 * We should only need one per process.
 **********************************************************************/
static llvm::LLVMContext &get_context(void)
{
    static llvm::LLVMContext context;
    return context;
}

/***********************************************************************
 * The ClangBlock factory - turn params into a shared_ptr of a block
 **********************************************************************/
boost::shared_ptr<gras::Block> ClangBlock::make(const ClangBlockParams &params)
{
    std::cout << "ClangBlock: compile " << params.name << " into bitcode..." << std::endl;
    const std::string bitcode = call_clang(params);

    llvm::InitializeNativeTarget();
    llvm::llvm_start_multithreaded();
    std::string error;

    //create a memory buffer from the bitcode
    boost::shared_ptr<llvm::MemoryBuffer> buffer(llvm::MemoryBuffer::getMemBuffer(bitcode));

    //parse the bitcode into a module
    llvm::Module *module = llvm::ParseBitcodeFile(buffer.get(), get_context(), &error);
    if (not error.empty()) throw std::runtime_error("ClangBlock: ParseBitcodeFile " + error);

    //create execution engine
    boost::shared_ptr<llvm::ExecutionEngine> ee(llvm::ExecutionEngine::create(module, false, &error));
    if (not error.empty()) throw std::runtime_error("ClangBlock: ExecutionEngine " + error);

    //extract function
    const std::string c_function_name = extern_c_fcn_name(params.name);
    llvm::Function *func = ee->FindFunctionNamed(c_function_name.c_str());
    if (not func) throw std::runtime_error("ClangBlock: FindFunctionNamed " + params.name);

    //call into the function
    std::cout << "ClangBlock: execute " << params.name << " factory function..." << std::endl;
    typedef void * (*PFN)();
    PFN pfn = reinterpret_cast<PFN>(ee->getPointerToFunction(func));
    void *block = pfn();

    //inject block into shared container w/ special deleter
    boost::shared_ptr<gras::Block> block_sptr;
    block_sptr.reset(reinterpret_cast<gras::Block *>(block), &delete_clang_block);
    boost::mutex::scoped_lock l(engine_map_mutex);
    block_ptr_to_engine[block] = ee;
    return block_sptr;
}

#else

boost::shared_ptr<gras::Block> ClangBlock::make(const ClangBlockParams &)
{
    return boost::shared_ptr<gras::Block>();
}

#endif //HAVE_CLANG
