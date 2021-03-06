#include <spdlog/spdlog.h>
#include <helpers/Utility.hpp>
#include "tables/VariableTable.h"

VarMap *VariableTable::findVarMap(const std::string &file_path, const std::string &func_name) {
    auto f = this->findFunctionVarMap(file_path);
    if (f) {
        auto r = f->find(func_name);
        if (r != f->end()) {
            return &r->second;
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

SliceProfile *VariableTable::findSliceProfile(const std::string file_path, const std::string func_name,
                                              const std::string var_name) {
    auto var_map = this->findVarMap(file_path, func_name);
    if (var_map) {
        auto r = var_map->find(var_name);
        if (r != var_map->end()) {
            return &r->second;
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

FunctionVarMap *VariableTable::findFunctionVarMap(const std::string &file_path) {
    auto r = this->_file_function_var_map.find(file_path);
    if (r != this->_file_function_var_map.end()) {
        return &r->second;
    } else {
        nullptr;
    }
}

void VariableTable::dump_to_stdout() const {
    for (auto v : this->_file_function_var_map) {
        std::cout << v.first << std::endl;
        for (auto v2: v.second) {
            std::cout << "    " << v2.first << std::endl;
            for (auto v3: v2.second) {
                std::cout << "        " << v3.second.variableName << std::endl;
            }
        }
    }
}

void VariableTable::forEach(std::function<void(SliceProfile *)> f) {
    // ソートする
    std::map<std::string, FunctionVarMap> sorted_ffvMap
            (this->_file_function_var_map.begin(),
             this->_file_function_var_map.end());
    for (auto &ffvmIt: sorted_ffvMap) {
        // ソートする
        std::map<std::string, VarMap> sorted_fvMap
                (ffvmIt.second.begin(),
                 ffvmIt.second.end());
        for (auto fvmIt: sorted_fvMap) {
            // ソートする
            std::map<std::string, SliceProfile> sorted_vMap(
                    fvmIt.second.begin(),
                    fvmIt.second.end()
            );
            for (auto vmIt: sorted_vMap) {
                f(&vmIt.second);
            }
        }
    }
}

FunctionVarMap *VariableTable::addFile(std::string file_path) {
    auto pair = this->_file_function_var_map.insert(
            std::make_pair(file_path, FunctionVarMap())
    );
    return &pair.first->second;
}

VarMap *VariableTable::addFunction(const std::string file_path, const std::string func_name) {
    auto function_varmap = this->findFunctionVarMap(file_path);
    if (function_varmap) {
        auto insert_result = function_varmap->insert(std::make_pair(func_name, VarMap()));
        return &insert_result.first->second;
    } else {
        return nullptr;
    }
}

SliceProfile *VariableTable::findGlobalVariableSliceProfileByName(const std::string &global_var_name) {
    auto sp = varmapFind(global_var_name, &this->_global_var_map);
    if (sp) {
        return sp;
    } else {
        return nullptr;
    }
}

VarMap *VariableTable::getRawGlobalVariableTable() {
    return &this->_global_var_map;
}

void VariableTable::addGlobalVariable(std::pair<std::string, SliceProfile> pair) {
    spdlog::get("console")->debug("グローバル変数を追加します: {}", pair.first);
    this->_global_var_map.insert(pair);
}
