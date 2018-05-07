#ifndef _MY_ARCH_CONFIG_H_
#define _MY_ARCH_CONFIG_H_

namespace MyArchConfig {
    class TableConfig {
    public:
        int tableId;
        std::vector<cstring>* matches;
        std::vector<cstring>* matchTypes;

        TableConfig(int tableId) {
            this->tableId = tableId;
            this->matches = new std::vector<cstring>();
            this->matchTypes = new std::vector<cstring>();
        }

        void pushMatch(cstring match) {
            matches->push_back(match);
        }

        void pushMatchType(cstring matchType) {
            matchTypes->push_back(matchType);
        }
    };

    class ActionConfig {
    public:
        cstring actionName;
        std::vector<cstring>* params;
        std::vector<cstring>* actionCalls;
        ActionConfig(cstring actionName) {
            this->actionName = actionName;
            this->params = new std::vector<cstring>();
            this->actionCalls = new std::vector<cstring>();
        }

        void pushParameter(cstring param) {
            params->push_back(param);
        }

        void pushAction(cstring action) {
            actionCalls->push_back(action);
        }
    };
}

#endif
