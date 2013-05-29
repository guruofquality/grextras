// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/clang_block.hpp>

using namespace grextras;

ClangBlockParams::ClangBlockParams(void)
{
    //NOP
}

#ifdef HAVE_CLANG

#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/DiagnosticOptions.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>

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
static llvm::Module *call_clang(const ClangBlockParams &params)
{
    //write source to tmp file
    char source_file[L_tmpnam];
    std::tmpnam(source_file);
    std::ofstream source_fstream(source_file);
    source_fstream << params.source;

    //inject the c wrapper
    source_fstream << boost::format(
        "\n"
        "extern \"C\" {\n"
        "void *%s(void){return %s();}"
        "}\n"
    ) % extern_c_fcn_name(params.name) % params.name;
    source_fstream.close();

    //begin command setup
    std::vector<const char *> args;

    //inject source
    args.push_back("-c");
    args.push_back("-x");
    args.push_back("c++");
    args.push_back(source_file);

    //inject args...
    BOOST_FOREACH(const std::string &flag, params.flags)
    {
        args.push_back(flag.c_str());
    }
    BOOST_FOREACH(const std::string &include, params.includes)
    {
        args.push_back("-I");
        args.push_back(include.c_str());
    }

    //http://fdiv.net/2012/08/15/compiling-code-clang-api

    // The compiler invocation needs a DiagnosticsEngine so it can report problems
    clang::TextDiagnosticPrinter *DiagClient = new clang::TextDiagnosticPrinter(llvm::errs(), clang::DiagnosticOptions());
    llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> DiagID(new clang::DiagnosticIDs());
    clang::DiagnosticsEngine Diags(DiagID, DiagClient);

    // Create the compiler invocation
    llvm::OwningPtr<clang::CompilerInvocation> CI(new clang::CompilerInvocation);
    clang::CompilerInvocation::CreateFromArgs(*CI, &args[0], &args[0] + args.size(), Diags);

    // Print the argument list, which the compiler invocation has extended
    printf("clang ");
    std::vector<std::string> argsFromInvocation;
    CI->toArgs(argsFromInvocation);
    for (std::vector<std::string>::iterator i = argsFromInvocation.begin(); i != argsFromInvocation.end(); ++i)
    {
        std::cout << (*i) << " ";
    }
    std::cout << std::endl;

    // Create the compiler instance
    clang::CompilerInstance Clang;
    Clang.setInvocation(CI.take());

    // Get ready to report problems
    Clang.createDiagnostics(args.size(), &args[0]);
    if (not Clang.hasDiagnostics()) throw std::runtime_error("ClangBlock::createDiagnostics");

    // Create an action and make the compiler instance carry it out
    clang::CodeGenAction *Act = new clang::EmitLLVMOnlyAction();
    if (not Clang.ExecuteAction(*Act)) throw std::runtime_error("ClangBlock::EmitLLVMOnlyAction");

    return Act->takeModule();
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
    llvm::InitializeNativeTarget();
    llvm::llvm_start_multithreaded();


    llvm::Module *module = call_clang(params);

    //create execution engine and function
    boost::shared_ptr<llvm::ExecutionEngine> ee(llvm::ExecutionEngine::create(module));
    const std::string c_function_name = extern_c_fcn_name(params.name);
    llvm::Function *func = ee->FindFunctionNamed(c_function_name.c_str());

    //call into the function
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
