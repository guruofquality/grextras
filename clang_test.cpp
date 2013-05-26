#include <string>
#include <memory>
#include <iostream>

#include <llvm/LLVMContext.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
//#include <llvm/ModuleProvider.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/system_error.h>

#include <iostream>

using namespace std;
using namespace llvm;

int main()
{
    std::cout << __LINE__ << std::endl;
    InitializeNativeTarget();
    std::cout << __LINE__ << std::endl;
    llvm_start_multithreaded();
    std::cout << __LINE__ << std::endl;
    LLVMContext context;
    std::cout << __LINE__ << std::endl;
    string error;
    OwningPtr< MemoryBuffer > buffer;
    std::cout << __LINE__ << std::endl;
    MemoryBuffer::getFile("/home/jblum/Desktop/foo.bc", buffer);
    std::cout << __LINE__ << std::endl;
    Module *m = ParseBitcodeFile(buffer.get(), context, &error);
    std::cout << __LINE__ << std::endl;
    ExecutionEngine *ee = ExecutionEngine::create(m);
    std::cout << __LINE__ << std::endl;

    Function* func = ee->FindFunctionNamed("main");
    std::cout << __LINE__ << std::endl;

    typedef void (*PFN)();
    PFN pfn = reinterpret_cast<PFN>(ee->getPointerToFunction(func));
    pfn();
    delete ee;
}
