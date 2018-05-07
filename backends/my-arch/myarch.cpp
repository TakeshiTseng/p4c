
#include <fstream>
#include <iostream>

#include "control-plane/p4RuntimeSerializer.h"
#include "ir/ir.h"
#include "ir/json_loader.h"
#include "lib/log.h"
#include "lib/error.h"
#include "lib/exceptions.h"
#include "lib/gc.h"
#include "lib/crash.h"
#include "lib/nullstream.h"
#include "frontends/common/applyOptionsPragmas.h"
#include "frontends/common/parseInput.h"
#include "frontends/p4/evaluator/evaluator.h"
#include "frontends/p4/frontend.h"
#include "frontends/p4/toP4/toP4.h"
#include "midend.h"
#include "my-arch-gen.h"

using MyArchContext = P4CContextWithOptions<MyArchGen::MyArchOptions>;

int main(int argc, char *const argv[]) {
    setup_gc_logging();
    setup_signals();

    AutoCompileContext autoMyArchContext(new MyArchContext);
    auto& options = MyArchContext::get().options();
    options.langVersion = CompilerOptions::FrontendVersion::P4_16;
    options.compilerVersion = "0.0.1";

    if (options.process(argc, argv) != nullptr)
        options.setInputFile();
    if (::errorCount() > 0)
        return 1;

    auto program = P4::parseP4File(options);
    auto hook = options.getDebugHook();

    if (program != nullptr && ::errorCount() == 0) {
        P4::P4COptionPragmaParser optionsPragmaParser;
        program->apply(P4::ApplyOptionsPragmas(optionsPragmaParser));


        try {
            P4::FrontEnd fe;
            fe.addDebugHook(hook);
            program = fe.run(options, program);
        } catch (const Util::P4CExceptionBase &bug) {
            std::cerr << bug.what() << std::endl;
            return 1;
        }

        if (program != nullptr && ::errorCount() == 0) {
            P4::serializeP4RuntimeIfRequired(program, options);
            MyArch::MidEnd midEnd(options);
            midEnd.addDebugHook(hook);
            const IR::ToplevelBlock *top = nullptr;
            try {
                top = midEnd.process(program);
            } catch (const Util::P4CExceptionBase &bug) {
                std::cerr << bug.what() << std::endl;
                return 1;
            }

            if (top == nullptr) {
                std::cout << "Can't find top level block" << std::endl;
                return 1;
            }

            // Generate binaries for my architecture
            auto gen = new MyArchGen::CodeGen();
            gen->gen(program, options);

            if (options.dumpJsonFile)
                JSONGenerator(*openFile(options.dumpJsonFile, true), true) << program << std::endl;
        }
    }

    if (Log::verbose())
        std::cerr << "Done." << std::endl;
    return ::errorCount() > 0;
}
