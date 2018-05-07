#ifndef _MY_ARCH_GEN_H_
#define _MY_ARCH_GEN_H_

#include "ir/ir.h"
#include "my-arch-config.h"

namespace MyArchGen {
const cstring MY_ARCH_TABLE = "my_arch_table";

class MyArchOptions : public CompilerOptions {
 public:
    cstring outputFile = nullptr;
    MyArchOptions() {
      registerOption("-o", "outfile",
              [this](const char* arg) { outputFile = arg; return true; },
              "Write output to outfile");
     }
};

class CodeGen {
public:
    CodeGen() = default;
    void gen(const IR::P4Program* p4Program, MyArchOptions options);

private:
    int getMyArchTableId(const IR::P4Table* table);
    MyArchConfig::TableConfig* genTableConfig(const IR::P4Table* tables);
    MyArchConfig::ActionConfig* genActionConfig(const IR::P4Action* actions);
    cstring convertMatchToStr(const IR::Expression* exp);
    cstring convertMatchTypeToStr(const IR::PathExpression* path);
    void writeMyArchBinaryToFile(std::vector<const MyArchConfig::TableConfig*>* tableConfigs,
                                 std::vector<const MyArchConfig::ActionConfig*>* actionConfigs,
                                 MyArchOptions options);
};

}

#endif
