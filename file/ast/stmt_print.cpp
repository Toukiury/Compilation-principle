#include "ast/stmt_print.hpp"
#include <sstream>
#include <thread>
// 定义每个缩进层级的空格数量
#define SPACE_NUM 3
/* 用于语法调试的PRINT文件 */

/**
 * @brief 生成指定深度的缩进字符串
 * 
 * 该函数根据传入的深度参数，生成相应数量的空格字符串，用于打印时的缩进。
 * 
 * @param deep 缩进的深度，每一级深度对应SPACE_NUM个空格
 * @return 包含指定数量空格的字符串
 */
std::string deep_print(int deep) {
    // 创建一个长度为 deep * SPACE_NUM 的空格字符串
    std::string spaces(deep * SPACE_NUM, ' ');
    return spaces;
}

/**
 * @brief 生成指定长度的分隔线字符串
 * 
 * 该函数根据传入的长度参数，生成相应数量的横线字符串，可用于打印时的分隔。
 * 
 * @param length 分隔线的长度，每一级长度对应SPACE_NUM个横线
 * @return 包含指定数量横线的字符串
 */
std::string mark_print(int length){
    // 创建一个长度为 length * SPACE_NUM 的横线字符串
    std::string marks(length * SPACE_NUM, '-');
    return marks;
}

/**
 * @brief 将NumberStmt对象转换为字符串
 * 
 * 该函数根据NumberStmt对象的类型（实数、字符、无符号整数或整数），将其转换为相应的字符串表示。
 * 
 * @param stmt NumberStmt对象的引用
 * @param deep 打印时的缩进深度
 * @return 包含NumberStmt对象信息的字符串
 */
std::string number_stmt_str(const NumberStmt stmt, int deep) {
    std::stringstream res;
    // 打印缩进和[NumberStmt]标记
    res << deep_print(deep) << "|____[NumberStmt] ";
    if (stmt.is_real) {
        // 如果是实数类型，打印实数的值
        res << "real: " << stmt.real_val;
    } else if (stmt.is_char) {
        // 如果是字符类型，打印字符的值
        res << "char: " << stmt.char_val;
    } else if (stmt.is_unsigned) {
        // 如果是无符号整数类型，打印无符号整数的值
        res << "unsigned: " << stmt.uint_val;
    } else {
        // 如果是整数类型，打印整数的值
        res << "int: " << stmt.int_val;
    }
    return res.str();
}

/**
 * @brief 将ConstDeclStmt的键值对转换为字符串
 * 
 * 该函数将ConstDeclStmt中的键值对转换为字符串，键为变量名，值通过value_stmt_str函数转换。
 * 
 * @param pair ConstDeclStmt的键值对引用
 * @param deep 打印时的缩进深度
 * @return 包含键值对信息的字符串
 */
std::string kv_pair_str(const ConstDeclStmt::KvPair &pair, int deep) {
    printf("kv_pair\n");
    std::stringstream res;
    // 打印缩进、键和等号，然后调用value_stmt_str函数打印值
    res << deep_print(deep) << pair.first << " = " << value_stmt_str(pair.second.get(), 1);
    return res.str();
}

/**
 * @brief 将ConstDeclStmt对象转换为字符串
 * 
 * 该函数将ConstDeclStmt对象转换为字符串，包括其标记和所有键值对信息。
 * 
 * @param stmt ConstDeclStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含ConstDeclStmt对象信息的字符串
 */
std::string const_decl_stmt_str(const ConstDeclStmt* stmt,int deep) {
    if(stmt == nullptr){
        return "";
    }
    printf("const_decl\n");
    std::stringstream res;
    // 打印缩进和[ConstDeclStmt]标记
    res << deep_print(deep) << "|____[ConstDeclStmt]\n";
    
    // 遍历所有键值对，调用kv_pair_str函数打印每个键值对信息
    for(int i = 0; i < stmt->pairs.size(); i++){
        res << kv_pair_str(stmt->pairs[i], deep + 1);
    }
    
    return res.str();
}

/**
 * @brief 将RelExprStmt对象转换为字符串
 * 
 * 该函数将RelExprStmt对象转换为字符串，包括其标记、操作符和子表达式信息。
 * 
 * @param stmt RelExprStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含RelExprStmt对象信息的字符串
 */
std::string rel_expr_stmt_str(const RelExprStmt* stmt, int deep){
    if(stmt == nullptr){
        return "";
    }
    std::stringstream res;
    // 打印缩进和[RelExprStmt]标记
    res << deep_print(deep) << "|____[RelExprStmt] " << "\n";
    // 遍历所有项
    for(auto &term : stmt->terms){
        if (term.type != RelExprStmt::RelExprType::NULL_TYPE)
        {
            // 如果操作符类型不为空，打印操作符信息
            res << "\n" + deep_print(deep+1) << "===>[OP]: " + rel_op(term.type) << "\n";
        }
        // 递归调用add_expr_stmt_str函数打印子表达式信息
        res << add_expr_stmt_str(term.add_expr.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将AddExprStmt对象转换为字符串
 * 
 * 该函数将AddExprStmt对象转换为字符串，包括其标记、操作符和子表达式信息。
 * 
 * @param stmt AddExprStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含AddExprStmt对象信息的字符串
 */
std::string add_expr_stmt_str(const AddExprStmt* stmt, int deep){
    if(stmt == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[AddExprStmt]标记
    res << deep_print(deep) << "|____[AddExprStmt]\n";
    // 遍历所有项
    for(auto &term : stmt->terms){
        if (term.type != AddExprStmt::AddExprType::NULL_TYPE)
        {
            // 如果操作符类型不为空，打印操作符信息
            res << "\n" + deep_print(deep+1) << "===>[OP]: " + add_op(term.type) << "\n";
        }
        // 递归调用mul_expr_stmt_str函数打印子表达式信息
        res << mul_expr_stmt_str(term.mul_expr.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将MulExprStmt对象转换为字符串
 * 
 * 该函数将MulExprStmt对象转换为字符串，包括其标记、操作符和子表达式信息。
 * 
 * @param stmt MulExprStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含MulExprStmt对象信息的字符串
 */
std::string mul_expr_stmt_str(const MulExprStmt* stmt, int deep){
    if(stmt == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[MulExprStmt]标记
    res << deep_print(deep) << "|____[MulExprStmt]\n";
    // 遍历所有项
    for(auto &term : stmt->terms){
        if (term.type != MulExprStmt::MulExprType::NULL_TYPE)
        {
            // 如果操作符类型不为空，打印操作符信息
            res << "\n" + deep_print(deep+1) << "===>[OP]: " + mul_op(term.type) << "\n";
        }
        // 递归调用unary_expr_stmt_str函数打印子表达式信息
        res << unary_expr_stmt_str(term.unary_expr.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将UnaryExprStmt对象转换为字符串
 * 
 * 该函数将UnaryExprStmt对象转换为字符串，包括其标记、操作符和子表达式信息。
 * 
 * @param stmt UnaryExprStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含UnaryExprStmt对象信息的字符串
 */
std::string unary_expr_stmt_str(const UnaryExprStmt* stmt, int deep){
    if(stmt == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[UnaryExprStmt]标记
    res << deep_print(deep) << "|____[UnaryExprStmt]\n";
    // 遍历所有操作符类型
    for (auto &term : stmt->types)
    {
        if (term != UnaryExprStmt::UnaryExprType::NULL_TYPE)
        {
            // 如果操作符类型不为空，打印操作符信息
            res << "\n" + deep_print(deep+1) << "===>[OP]: " + unary_op(term) << "\n";
        }
        
    }
    // 递归调用primary_expr_stmt_str函数打印子表达式信息
    res << primary_expr_stmt_str(stmt->primary_expr.get(), deep + 1);
    return res.str();
};

/**
 * @brief 将PrimaryExprStmt对象转换为字符串
 * 
 * 该函数将PrimaryExprStmt对象转换为字符串，根据其类型（值或表达式）递归调用相应的函数。
 * 
 * @param stmt PrimaryExprStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含PrimaryExprStmt对象信息的字符串
 */
std::string primary_expr_stmt_str(const PrimaryExprStmt* stmt, int deep){
    if(stmt == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[PrimaryExprStmt]标记
    res << deep_print(deep) << "|____[PrimaryExprStmt]\n";
    if(stmt->type == PrimaryExprStmt::PrimaryExprType::Value){
        // 如果是值类型，递归调用value_stmt_str函数打印值信息
        res << value_stmt_str(stmt->value.get(), deep + 1);
    }else{
        // 如果是表达式类型，递归调用expr_stmt_str函数打印表达式信息
        res << expr_stmt_str(stmt->expr.get(), deep + 1);
    }
    res << deep_print(deep+1);
    return res.str();
};

/**
 * @brief 将ValueStmt对象转换为字符串
 * 
 * 该函数将ValueStmt对象转换为字符串，根据其类型（数字、字符串、左值或函数调用）递归调用相应的函数。
 * 
 * @param stmt ValueStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含ValueStmt对象信息的字符串
 */
std::string value_stmt_str(const ValueStmt* stmt, int deep){
    if(stmt == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[ValueStmt]标记
    res << deep_print(deep) << "|____[ValueStmt]\n";
    if(stmt->type == ValueStmt::ValueType::Number){
        // 如果是数字类型，递归调用number_stmt_str函数打印数字信息
        res << number_stmt_str(*stmt->number, deep + 1);
    }else if(stmt->type == ValueStmt::ValueType::Str){
        // 如果是字符串类型，递归调用str_stmt_str函数打印字符串信息
        res << str_stmt_str(stmt->str.get(), deep + 1);
    }else if(stmt->type == ValueStmt::ValueType::LVal){
        // 如果是左值类型，递归调用lval_stmt_str函数打印左值信息
        res << lval_stmt_str(stmt->lval.get(), deep + 1);
    }else if(stmt->type == ValueStmt::ValueType::FuncCall){
        // 如果是函数调用类型，递归调用func_call_stmt_str函数打印函数调用信息
        res << func_call_stmt_str(stmt->func_call.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将StrStmt对象转换为字符串
 * 
 * 该函数将StrStmt对象转换为字符串，包括其标记和字符串值。
 * 
 * @param stmt StrStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含StrStmt对象信息的字符串
 */
std::string str_stmt_str(const StrStmt* stmt, int deep){
    if(stmt == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进、[StrStmt]标记和字符串值
    res << deep_print(deep) << "|____[StrStmt] " + stmt->val;
    return res.str();
};

/**
 * @brief 将LValStmt对象转换为字符串
 * 
 * 该函数将LValStmt对象转换为字符串，包括其标记、标识符和数组索引信息。
 * 
 * @param stmt LValStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含LValStmt对象信息的字符串
 */
std::string lval_stmt_str(const LValStmt* stmt, int deep){
    if(stmt == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[LValStmt]标记
    res << deep_print(deep) << "|____[LValStmt] \n" ;
    // 打印标识符信息
    res << deep_print(deep+1) << "id: " + stmt->id + "\n";
    // 遍历所有数组索引，递归调用expr_stmt_str函数打印索引表达式信息
    for(auto &expr : stmt->array_index){
        res << expr_stmt_str(expr.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将FuncCallStmt对象转换为字符串
 * 
 * 该函数将FuncCallStmt对象转换为字符串，包括其标记、函数名和参数信息。
 * 
 * @param stmt FuncCallStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含FuncCallStmt对象信息的字符串
 */
std::string func_call_stmt_str(const FuncCallStmt* stmt, int deep){
    if(stmt == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[FuncCallStmt]标记
    res << deep_print(deep) << "|____[FuncCallStmt] \n";
    // 打印函数名信息
    res << deep_print(deep+1) << "id: " + stmt->id + "\n";
    // 遍历所有参数，递归调用expr_stmt_str函数打印参数表达式信息
    for(auto &expr : stmt->args){
        res << expr_stmt_str(expr.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将ExprStmt列表转换为字符串
 * 
 * 该函数将ExprStmt列表转换为字符串，包括列表标记和每个ExprStmt的信息。
 * 
 * @param stmts ExprStmt列表的指针
 * @param deep 打印时的缩进深度
 * @return 包含ExprStmt列表信息的字符串
 */
std::string expr_stmt_list_str(const std::vector<ExprStmt *> *stmts, int deep){
    if(stmts == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[ExprStmtList]标记
    res << deep_print(deep) << "|____[ExprStmtList]\n";
    // 遍历所有ExprStmt，递归调用expr_stmt_str函数打印每个ExprStmt信息
    for(auto &stmt : *stmts){
        res << expr_stmt_str(stmt, deep + 1);
    }
    return res.str();
};

/**
 * @brief 将标识符列表转换为字符串
 * 
 * 该函数将标识符列表转换为字符串，包括列表标记和每个标识符信息。
 * 
 * @param id_list 标识符列表的引用
 * @param deep 打印时的缩进深度
 * @return 包含标识符列表信息的字符串
 */
std::string id_list_str(const std::vector<std::string>& id_list, int deep){
    std::stringstream res; 
    // 打印缩进和[IDList]标记
    res << deep_print(deep) << "|____[IDList]\n";
    // 遍历所有标识符，打印每个标识符信息
    for(auto &id : id_list){
        res << deep_print(deep+1) + id + "\n";
    }
    return res.str();
};

/**
 * @brief 将PeriodStmt对象转换为字符串
 * 
 * 该函数将PeriodStmt对象转换为字符串，包括其标记、起始值和结束值信息。
 * 
 * @param period PeriodStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含PeriodStmt对象信息的字符串
 */
std::string period_stmt_str(const PeriodStmt* period, int deep){
    if(period == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[PeriodStmt]标记
    res << deep_print(deep) << "|____[PeriodStmt]\n";
    // 打印起始值和结束值信息
    res << deep_print(deep+1) << "begin: " + std::to_string(period->begin) << " end: " + std::to_string(period->end) << "\n";
    return res.str();
};

/**
 * @brief 将PeriodStmt列表转换为字符串
 * 
 * 该函数将PeriodStmt列表转换为字符串，包括列表标记和每个PeriodStmt的信息。
 * 
 * @param period_list PeriodStmt列表的指针
 * @param deep 打印时的缩进深度
 * @return 包含PeriodStmt列表信息的字符串
 */
std::string period_list_str(const std::vector<PeriodStmt*>* period_list, int deep){
    if(period_list == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[PeriodList]标记
    res << deep_print(deep) << "|____[PeriodList]\n";
    // 遍历所有PeriodStmt，递归调用period_stmt_str函数打印每个PeriodStmt信息
    for(auto &period : *period_list){
        res << period_stmt_str(period, deep + 1);
    }
    return res.str();
};

/**
 * @brief 将基本类型转换为字符串
 * 
 * 该函数将基本类型枚举值转换为对应的字符串表示。
 * 
 * @param type 基本类型枚举值
 * @return 基本类型的字符串表示
 */
std::string basic_type_str(BasicType type){
    if(type == BasicType::INT){
        return "int";
    }else if(type == BasicType::REAL){
        return "real";
    }else if(type == BasicType::CHAR){
        return "char";
    }else if(type == BasicType::BOOLEAN){
        return "boolean";
    }else{
        return "ERROR";
    }
};

/**
 * @brief 将数据类型转换为字符串
 * 
 * 该函数将数据类型枚举值转换为对应的字符串表示。
 * 
 * @param type 数据类型枚举值
 * @return 数据类型的字符串表示
 */
std::string data_type_str(DataType type){
    if(type == DataType::NULL_TYPE){
        return "null";
    }else if(type == DataType::BasicType){
        return "BasicType";
    }else if(type == DataType::ArrayType){
        return "ArrayType";
    }else{
        return "ERROR";
    }
};

/**
 * @brief 将VarDeclStmt对象转换为字符串
 * 
 * 该函数将VarDeclStmt对象转换为字符串，包括其标记、标识符列表、数据类型、基本类型和数组范围信息。
 * 
 * @param var_decl VarDeclStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含VarDeclStmt对象信息的字符串
 */
std::string var_decl_stmt_str(const VarDeclStmt* var_decl, int deep){
    if(var_decl == nullptr){
        return "";
    }
    printf("var_decl\n");
    std::stringstream res; 
    // 打印缩进和[VarDeclStmt]标记
    res << deep_print(deep) << "|____[VarDeclStmt]\n";
    // 递归调用id_list_str函数打印标识符列表信息
    res << id_list_str(var_decl->id, deep + 1);
    // 打印数据类型信息
    res << deep_print(deep+1) << "data_type: " + data_type_str(var_decl->data_type) << "\n";
    // 打印基本类型信息
    res << deep_print(deep+1) << "basic_type: " + basic_type_str(var_decl->basic_type) << "\n";
    // 遍历所有数组范围，递归调用period_stmt_str函数打印数组范围信息
    for(auto &period : var_decl->array_range){
        res << period_stmt_str(period.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将VarDeclStmt列表转换为字符串
 * 
 * 该函数将VarDeclStmt列表转换为字符串，包括列表标记和每个VarDeclStmt的信息。
 * 
 * @param var_decl_list VarDeclStmt列表的指针
 * @param deep 打印时的缩进深度
 * @return 包含VarDeclStmt列表信息的字符串
 */
std::string var_decl_stmt_list_str(const std::vector<VarDeclStmt*>* var_decl_list, int deep){
    if(var_decl_list == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[VarDeclStmtList]标记
    res << deep_print(deep) << "|____[VarDeclStmtList]\n";
    // 遍历所有VarDeclStmt，递归调用var_decl_stmt_str函数打印每个VarDeclStmt信息
    for(auto &var_decl : *var_decl_list){
        res << var_decl_stmt_str(var_decl, deep + 1);
    }
    return res.str();
};

/**
 * @brief 将FuncDeclStmt对象转换为字符串
 * 
 * 该函数将FuncDeclStmt对象转换为字符串，包括其标记、函数头和函数体信息。
 * 
 * @param func_decl FuncDeclStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含FuncDeclStmt对象信息的字符串
 */
std::string func_stmt_str(const FuncDeclStmt* func_decl, int deep){
    if(func_decl == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[FuncDeclStmt]标记
    res << deep_print(deep) << "|____[FuncDeclStmt]\n";
    // 递归调用func_head_decl_stmt_str函数打印函数头信息
    res << func_head_decl_stmt_str(func_decl->header.get(), deep + 1);
    // 递归调用func_body_decl_stmt_str函数打印函数体信息
    res << func_body_decl_stmt_str(func_decl->body.get(), deep + 1);
    return res.str();
};

/**
 * @brief 将FuncHeadDeclStmt对象转换为字符串
 * 
 * 该函数将FuncHeadDeclStmt对象转换为字符串，包括其标记、函数名、返回类型和参数信息。
 * 
 * @param func_head FuncHeadDeclStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含FuncHeadDeclStmt对象信息的字符串
 */
std::string func_head_decl_stmt_str(const FuncHeadDeclStmt* func_head, int deep){
    if(func_head == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[FuncHeadDeclStmt]标记
    res << deep_print(deep) << "|____[FuncHeadDeclStmt]\n";
    // 打印函数名信息
    res << deep_print(deep+1) << "func_name: " + func_head->func_name + "\n";
    // 打印返回类型信息
    res << deep_print(deep+1) << "ret_type: " + basic_type_str(func_head->ret_type) << "\n";
    // 打印参数信息
    res << deep_print(deep+1) << "args:\n";
    // 遍历所有参数，递归调用var_decl_stmt_str函数打印参数信息
    for(auto &var_decl : func_head->args){
        res << var_decl_stmt_str(var_decl.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将FuncBodyDeclStmt对象转换为字符串
 * 
 * 该函数将FuncBodyDeclStmt对象转换为字符串，包括其标记、常量声明、变量声明和复合语句信息。
 * 
 * @param func_body FuncBodyDeclStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含FuncBodyDeclStmt对象信息的字符串
 */
std::string func_body_decl_stmt_str(const FuncBodyDeclStmt* func_body, int deep){
    if(func_body == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[FuncBodyDeclStmt]标记
    res << deep_print(deep) << "|____[FuncBodyDeclStmt]\n";
    // 递归调用const_decl_stmt_str函数打印常量声明信息
    res << const_decl_stmt_str(func_body->const_decl.get(), deep + 1);
    // 遍历所有变量声明，递归调用var_decl_stmt_str函数打印变量声明信息
    for(auto &var_decl : func_body->var_decl){
        res << var_decl_stmt_str(var_decl.get(), deep + 1);
    }
    int i = 0;
    // std::string res1 = "";
    // 遍历所有复合语句，递归调用base_stmt_str函数打印复合语句信息
    for(auto &stmt : func_body->comp_stmt){
        // 根据 BaseStmt 类型的子类，调用对应的函数
        res << base_stmt_str(stmt.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将AssignStmt对象转换为字符串
 * 
 * 该函数将AssignStmt对象转换为字符串，包括其标记、左值是否为函数标志、左值信息和表达式信息。
 * 
 * @param assign AssignStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含AssignStmt对象信息的字符串
 */
std::string assign_stmt_str(const AssignStmt* assign, int deep){
    if(assign == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[AssignStmt]标记
    res << deep_print(deep) << "|____[AssignStmt]\n";
    // 打印左值是否为函数标志信息
    res << deep_print(deep+1) << "is_lval_func: " + std::to_string(assign->is_lval_func) << "\n";
    // 递归调用lval_stmt_str函数打印左值信息
    res << lval_stmt_str(assign->lval.get(), deep + 1);
    // 递归调用expr_stmt_str函数打印表达式信息
    res << expr_stmt_str(assign->expr.get(), deep + 1);
    return res.str();
};

/**
 * @brief 将IfStmt对象转换为字符串
 * 
 * 该函数将IfStmt对象转换为字符串，包括其标记、条件表达式、真语句和假语句信息。
 * 
 * @param if_stmt IfStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含IfStmt对象信息的字符串
 */
std::string if_stmt_str(const IfStmt* if_stmt, int deep){
    if(if_stmt == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[IfStmt]标记
    res << deep_print(deep) << "|____[IfStmt]\n";
    // 打印条件表达式信息
    res << deep_print(deep+1) << "expr:\n";
    res << expr_stmt_str(if_stmt->expr.get(), deep + 1);
    // 打印真语句信息
    res << deep_print(deep+1) << "true_stmt:\n";
    for(auto &stmt : if_stmt->true_stmt){
        res << base_stmt_str(stmt.get(), deep + 1);
    }
    // 打印分隔线
    res << deep_print(deep+1) << "-----------------------\n";
    // 打印假语句信息
    res << deep_print(deep+1) << "false_stmt:\n";
    for(auto &stmt : if_stmt->false_stmt){
        res << base_stmt_str(stmt.get(), deep + 1);
    }
    // 打印分隔线
    res << deep_print(deep+1) << "-----------------------\n";
    return res.str();
};

/**
 * @brief 将ForStmt对象转换为字符串
 * 
 * 该函数将ForStmt对象转换为字符串，包括其标记、循环变量、起始表达式、结束表达式和循环体语句信息。
 * 
 * @param for_stmt ForStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含ForStmt对象信息的字符串
 */
std::string for_stmt_str(const ForStmt* for_stmt, int deep){
    if(for_stmt == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[ForStmt]标记
    res << deep_print(deep) << "|____[ForStmt]\n";
    // 打印循环变量信息
    res << deep_print(deep+1) << "id: " + for_stmt->id + "\n";
    // 递归调用expr_stmt_str函数打印起始表达式信息
    res << expr_stmt_str(for_stmt->begin.get(), deep + 1);
    // 递归调用expr_stmt_str函数打印结束表达式信息
    res << expr_stmt_str(for_stmt->end.get(), deep + 1);
    // 遍历所有循环体语句，递归调用base_stmt_str函数打印循环体语句信息
    for(auto &stmt : for_stmt->stmt){
        res << base_stmt_str(stmt.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将ReadFuncStmt对象转换为字符串
 * 
 * 该函数将ReadFuncStmt对象转换为字符串，包括其标记和左值信息。
 * 
 * @param read_func ReadFuncStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含ReadFuncStmt对象信息的字符串
 */
std::string read_func_stmt_str(const ReadFuncStmt* read_func, int deep){
    if(read_func == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[ReadFuncStmt]标记
    res << deep_print(deep) << "|____[ReadFuncStmt]\n";
    // 遍历所有左值，递归调用lval_stmt_str函数打印左值信息
    for(auto &lval : read_func->lval){
        res << lval_stmt_str(lval.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将WriteFuncStmt对象转换为字符串
 * 
 * 该函数将WriteFuncStmt对象转换为字符串，包括其标记和表达式信息。
 * 
 * @param write_func WriteFuncStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含WriteFuncStmt对象信息的字符串
 */
std::string write_func_stmt_str(const WriteFuncStmt* write_func, int deep){
    if(write_func == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[WriteFuncStmt]标记
    res << deep_print(deep) << "|____[WriteFuncStmt]\n";
    // 遍历所有表达式，递归调用expr_stmt_str函数打印表达式信息
    for(auto &expr : write_func->expr){    
        res << expr_stmt_str(expr.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将ExprStmt对象转换为字符串
 * 
 * 该函数将ExprStmt对象转换为字符串，包括其标记和关系表达式信息。
 * 
 * @param stmt ExprStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含ExprStmt对象信息的字符串
 */
std::string expr_stmt_str(const ExprStmt* stmt, int deep){
    if(stmt == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[ExprStmt]标记
    res << deep_print(deep) << "|____[ExprStmt]\n";
    // 递归调用rel_expr_stmt_str函数打印关系表达式信息
    res << rel_expr_stmt_str(stmt->rel_expr.get(), deep + 1) << "\n";
    return res.str();
};

/**
 * @brief 将ProgramHeadStmt对象转换为字符串
 * 
 * 该函数将ProgramHeadStmt对象转换为字符串，包括其标记和标识符列表信息。
 * 
 * @param program_head ProgramHeadStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含ProgramHeadStmt对象信息的字符串
 */
std::string program_head_stmt_str(const ProgramHeadStmt* program_head, int deep){
    if(program_head == nullptr){
        return "";
    }
    printf("program_head\n");
    std::stringstream res; 
    // 打印缩进和[ProgramHeadStmt]标记
    res << deep_print(deep) << "|____[ProgramHeadStmt]\n";
    // 遍历所有标识符，打印标识符信息
    for(auto &id : program_head->id_list){
        res << deep_print(deep+1) + id + "\n";
    }
    return res.str();
};

/**
 * @brief 将ProgramBodyStmt对象转换为字符串
 * 
 * 该函数将ProgramBodyStmt对象转换为字符串，包括其标记、常量声明、变量声明、函数声明和复合语句信息。
 * 
 * @param program_body ProgramBodyStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含ProgramBodyStmt对象信息的字符串
 */
std::string program_body_stmt_str(const ProgramBodyStmt* program_body, int deep){
    if(program_body == nullptr){
        return "";
    }
    printf("program_body\n");
    std::stringstream res; 
    // 打印缩进和[ProgramBodyStmt]标记
    res << deep_print(deep) << "|____[ProgramBodyStmt]\n";
    // 递归调用const_decl_stmt_str函数打印常量声明信息
    res << const_decl_stmt_str(program_body->const_decl.get(), deep + 1);
    // 遍历所有变量声明，递归调用var_decl_stmt_str函数打印变量声明信息
    for(auto &var_decl : program_body->var_decl){
        res << var_decl_stmt_str(var_decl.get(), deep + 1);
    }
    // 遍历所有函数声明，递归调用func_stmt_str函数打印函数声明信息
    for(auto &func_decl : program_body->func_decl){
        res << func_stmt_str(func_decl.get(), deep + 1);
    }
    // 遍历所有复合语句，递归调用base_stmt_str函数打印复合语句信息
    for(auto &stmt : program_body->comp_stmt){
        res << base_stmt_str(stmt.get(), deep + 1);
    }
    return res.str();
};

/**
 * @brief 将ProgramStmt对象转换为字符串
 * 
 * 该函数将ProgramStmt对象转换为字符串，包括其标记、程序头和程序体信息。
 * 
 * @param program ProgramStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含ProgramStmt对象信息的字符串
 */
std::string program_stmt_str(const ProgramStmt* program, int deep){
    if(program == nullptr){
        return "";
    }
    printf("program\n");
    std::stringstream res; 
    // 打印缩进和[ProgramStmt]标记
    res << deep_print(deep) << "|____[ProgramStmt]\n";
    // 递归调用program_head_stmt_str函数打印程序头信息
    res << program_head_stmt_str(program->head.get(), deep + 1);
    // 递归调用program_body_stmt_str函数打印程序体信息
    res << program_body_stmt_str(program->body.get(), deep + 1);
    return res.str();
};

/**
 * @brief 将关系表达式操作符类型转换为字符串
 * 
 * 该函数将关系表达式操作符类型枚举值转换为对应的字符串表示。
 * 
 * @param type 关系表达式操作符类型枚举值
 * @return 关系表达式操作符的字符串表示
 */
std::string rel_op(RelExprStmt::RelExprType type){
    if(type == RelExprStmt::RelExprType::Equal){
        return "=";
    }else if(type == RelExprStmt::RelExprType::NotEqual){
        return "<>";
    }else if(type == RelExprStmt::RelExprType::Less){
        return "<";
    }else if(type == RelExprStmt::RelExprType::LessEqual){
        return "<=";
    }else if(type == RelExprStmt::RelExprType::Greater){
        return ">";
    }else if(type == RelExprStmt::RelExprType::GreaterEqual){
        return ">=";
    }else if(type == RelExprStmt::RelExprType::In){
        return "in";
    }else{
        return "ERROR";
    }
};

/**
 * @brief 将加法表达式操作符类型转换为字符串
 * 
 * 该函数将加法表达式操作符类型枚举值转换为对应的字符串表示。
 * 
 * @param type 加法表达式操作符类型枚举值
 * @return 加法表达式操作符的字符串表示
 */
std::string add_op(AddExprStmt::AddExprType type){
    if(type == AddExprStmt::AddExprType::Plus){
        return "+";
    }else if(type == AddExprStmt::AddExprType::Minus){
        return "-";
    }else if(type == AddExprStmt::AddExprType::Or){
        return "or";
    }else{
        return "ERROR";
    }
};

/**
 * @brief 将乘法表达式操作符类型转换为字符串
 * 
 * 该函数将乘法表达式操作符类型枚举值转换为对应的字符串表示。
 * 
 * @param type 乘法表达式操作符类型枚举值
 * @return 乘法表达式操作符的字符串表示
 */
std::string mul_op(MulExprStmt::MulExprType type){
    if(type == MulExprStmt::MulExprType::Mul){
        return "*";
    }else if(type == MulExprStmt::MulExprType::Div){
        return "/";
    }else if(type == MulExprStmt::MulExprType::Mod){
        return "mod";
    }else if(type == MulExprStmt::MulExprType::And){
        return "and";
    }else if(type == MulExprStmt::MulExprType::AndThen){
        return "and then";
    }else{
        return "ERROR";
    }
};

/**
 * @brief 将一元表达式操作符类型转换为字符串
 * 
 * 该函数将一元表达式操作符类型枚举值转换为对应的字符串表示。
 * 
 * @param type 一元表达式操作符类型枚举值
 * @return 一元表达式操作符的字符串表示
 */
std::string unary_op(UnaryExprStmt::UnaryExprType type){
    if(type == UnaryExprStmt::UnaryExprType::Minus){
        return "-";
    }else if(type == UnaryExprStmt::UnaryExprType::Not){
        return "not";
    }else{
        return "ERROR";
    }
};

/**
 * @brief 将BaseStmt对象转换为字符串
 * 
 * 该函数根据BaseStmt对象的具体子类类型，调用相应的转换函数将其转换为字符串。
 * 
 * @param stmt BaseStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含BaseStmt对象信息的字符串
 */
std::string base_stmt_str(const BaseStmt* stmt, int deep){
    if(stmt == nullptr){
        return "";
    }
    // 根据 BaseStmt 类型的子类，调用对应的函数
    if (typeid(*stmt) == typeid(AssignStmt)){
        printf("assign\n");
        return assign_stmt_str((const AssignStmt *)stmt, deep);
    }else if(typeid(*stmt) == typeid(IfStmt)){
        printf("if\n");
        return if_stmt_str((const IfStmt *)stmt, deep);
    }else if(typeid(*stmt) == typeid(WhileStmt)){
        printf("while\n");
        return while_stmt_str((const WhileStmt *)stmt, deep);
    }else if(typeid(*stmt) == typeid(ForStmt)){
        printf("for\n");
        return for_stmt_str((const ForStmt *)stmt, deep);
    }else if(typeid(*stmt) == typeid(ReadFuncStmt)){
        printf("read\n");
        return read_func_stmt_str((const ReadFuncStmt *)stmt, deep);
    }else if(typeid(*stmt) == typeid(WriteFuncStmt)){
        printf("write\n");
        return write_func_stmt_str((const WriteFuncStmt *)stmt, deep);
    }else if(typeid(*stmt) == typeid(FuncCallStmt)){
        printf("func\n");
        return func_call_stmt_str((const FuncCallStmt *)stmt, deep);
    }else {
        return "ERROR";
    }
};

/**
 * @brief 将WhileStmt对象转换为字符串
 * 
 * 该函数将WhileStmt对象转换为字符串，包括其标记、条件表达式和循环体语句信息。
 * 
 * @param while_stmt WhileStmt对象的指针
 * @param deep 打印时的缩进深度
 * @return 包含WhileStmt对象信息的字符串
 */
std::string while_stmt_str(const WhileStmt* while_stmt, int deep){
    if(while_stmt == nullptr){
        return "";
    }
    std::stringstream res; 
    // 打印缩进和[WhileStmt]标记
    res << deep_print(deep) << "|____[WhileStmt]\n";
    // 打印条件表达式信息
    res << deep_print(deep+1) << "expr:\n";
    res << expr_stmt_str(while_stmt->expr.get(), deep + 1);
    // 遍历所有循环体语句，递归调用base_stmt_str函数打印循环体语句信息
    for(auto &stmt : while_stmt->stmt){
        res << base_stmt_str(stmt.get(), deep + 1);
    }
    return res.str();
};
