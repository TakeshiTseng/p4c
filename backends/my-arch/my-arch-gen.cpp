#include <cstring>
#include "my-arch-gen.h"


namespace MyArchGen {

void CodeGen::gen(const IR::P4Program* program, MyArchOptions options) {
    auto main = program->getDeclByName("main")->to<IR::Declaration_Instance>();
    // first argument of main package is ingress pipeline
    auto arg0 = main->arguments->at(0);
    auto argExp = arg0->expression->to<IR::ConstructorCallExpression>();
    auto argExpType = argExp->type->to<IR::Type_Control>();
    auto ingressPipelineName = argExpType->name;

    auto ingressPipelineDec = program
                                  ->getDeclByName(ingressPipelineName)
                                  ->to<IR::P4Control>();
    auto pipelineDecs = ingressPipelineDec->getDeclarations();

    auto actions = new std::vector<const IR::P4Action *>();
    auto tables = new std::vector<const IR::P4Table *>();
    auto unsupported = new std::vector<const IR::IDeclaration *>();

    for (auto dec : *pipelineDecs) {
        if (dec->is<IR::P4Action>()) {
            actions->push_back(dec->to<IR::P4Action>());
        } else if (dec->is<IR::P4Table>()) {
            tables->push_back(dec->to<IR::P4Table>());
        } else {
            unsupported->push_back(dec);
        }
    }

    auto myArchTables = new std::vector<const MyArchConfig::TableConfig*>();
    auto myArchActions = new std::vector<const MyArchConfig::ActionConfig*>();
    for (auto table : *tables) {
        const MyArchConfig::TableConfig* tableConfig = genTableConfig(table);
        if (tableConfig != nullptr) {
            myArchTables->push_back(tableConfig);
        }
    }

    for (auto action : *actions) {
        const MyArchConfig::ActionConfig* actionConfig = genActionConfig(action);
        if (actionConfig != nullptr) {
            myArchActions->push_back(actionConfig);
        }
    }

    writeMyArchBinaryToFile(myArchTables, myArchActions, options);
}

int CodeGen::getMyArchTableId(const IR::P4Table* table) {
    auto ants = table->getAnnotations();
    auto archTableIdAnno = ants->getSingle(MY_ARCH_TABLE);
    if (archTableIdAnno == nullptr) {
        return -1;
    }

    auto expr = archTableIdAnno->expr.at(0);
    if (expr->is<IR::Constant>()) {
        auto constant = expr->to<IR::Constant>();
        return constant->asInt();
    }
    return -1;
}
MyArchConfig::TableConfig* CodeGen::genTableConfig(const IR::P4Table* table) {
    auto tableKey = table->getKey();
    int tableId = getMyArchTableId(table);
    if (tableId == -1) {
        return nullptr;
    }
    auto result = new MyArchConfig::TableConfig(tableId);

    for (auto ke : tableKey->keyElements) {
        cstring matchStr = convertMatchToStr(ke->expression);
        cstring matchTypeStr = convertMatchTypeToStr(ke->matchType);
        result->pushMatch(matchStr);
        result->pushMatchType(matchTypeStr);
    }
    return result;
}
MyArchConfig::ActionConfig* CodeGen::genActionConfig(const IR::P4Action* action) {
    cstring actionName = action->name;
    auto result = new MyArchConfig::ActionConfig(actionName);
    auto paramList = action->parameters;

    for (auto dec : *paramList->getDeclarations()) {
        if (dec->is<IR::Parameter>()) {
            auto param = dec->to<IR::Parameter>();
            result->pushParameter(param->name);
        }
    }

    auto actionBody = action->body;
    auto bodyComps = actionBody->components;

    for (auto comp : bodyComps) {
        if (comp->is<IR::AssignmentStatement>()) {
            auto assigStat = comp->to<IR::AssignmentStatement>();
            std::ostringstream actStat;
            actStat << "ASSIGN " << assigStat->left->toString() << " " << assigStat->right->toString();
            result->pushAction(actStat.str());
        } else if (comp->is<IR::MethodCallStatement>()) {
            auto mthCallStat = comp->to<IR::MethodCallStatement>();
            auto method = mthCallStat->methodCall->method;

            std::ostringstream mthStat;
            mthStat << "EXTERN " << method->toString();
            result->pushAction(mthStat.str());
        } else if (comp->is<IR::EmptyStatement>()){
            result->pushAction("NOP");
        } else {
            std::cout << "Unsupported statement " << comp << std::endl;
        }
    }

    return result;
}

cstring CodeGen::convertMatchToStr(const IR::Expression* exp) {
    return exp->toString();
}

cstring CodeGen::convertMatchTypeToStr(const IR::PathExpression* path) {
    return path->toString();
}

void CodeGen::writeMyArchBinaryToFile(std::vector<const MyArchConfig::TableConfig*>* tableConfigs,
                             std::vector<const MyArchConfig::ActionConfig*>* actionConfigs,
                             MyArchOptions options) {
    if (options.outputFile.isNullOrEmpty()) {
        return;
    }

    std::ofstream fileStream;
    fileStream.open(options.outputFile);
    for (auto tblCfg : *tableConfigs) {
        fileStream << "TBL " << tblCfg->tableId << " BEGIN" << std::endl;
        for (size_t i = 0; i < tblCfg->matches->size(); i++) {
            fileStream << "MATCH " << tblCfg->matches->at(i) << " " << tblCfg->matchTypes->at(i) << std::endl;
        }
        fileStream << "TBL " << tblCfg->tableId << " END" << std::endl;
    }
    fileStream << std::endl;
    for (auto actCfg : *actionConfigs) {
        fileStream << "ACT " << actCfg->actionName << " BEGIN" << std::endl;
        for (auto param : *actCfg->params) {
            fileStream << "PARAM " << param << std::endl;
        }
        for (auto stat : *actCfg->actionCalls) {
            fileStream << "STAT " << stat << std::endl;
        }
        fileStream << "ACT " << actCfg->actionName << " END" << std::endl;
    }
    fileStream << std::endl;
    fileStream.flush();
    fileStream.close();
}
}
