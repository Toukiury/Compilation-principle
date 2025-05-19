#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class StmtVisitor;

// stmt的基类，即AST的节点
class BaseStmt {
public:
    virtual void accept(StmtVisitor &visitor) = 0;
    BaseStmt() = default;
    virtual ~BaseStmt() = default;
};

// 表达式相关类
class FuncCallStmt;
class LValStmt;
class StrStmt;
class NumberStmt;
class ValueStmt;
class PrimaryExprStmt;
class UnaryExprStmt;
class MulExprStmt;
class AddExprStmt;
class RelExprStmt;

// 表达式
class ExprStmt : public BaseStmt {
public:
    std::unique_ptr<RelExprStmt> rel_expr;
    void accept(StmtVisitor &visitor) override;
};

// 关系表达式
class RelExprStmt : public BaseStmt {
public:
    enum class RelExprType { NULL_TYPE, Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual, In };
    struct Term {
        RelExprType type;
        std::unique_ptr<AddExprStmt> add_expr;
    };
    std::vector<Term> terms;
    void accept(StmtVisitor &visitor) override;
};

// 加法类表达式
class AddExprStmt : public BaseStmt {
public:
    enum class AddExprType { NULL_TYPE, Plus, Minus, Or };
    struct Term {
        AddExprType type;
        std::unique_ptr<MulExprStmt> mul_expr;
    };
    std::vector<Term> terms;
    void accept(StmtVisitor &visitor) override;
};

// 乘积类表达式
class MulExprStmt : public BaseStmt {
public:
    enum class MulExprType { NULL_TYPE, Mul, Div, Mod, And, AndThen };
    struct Term {
        MulExprType type;
        std::unique_ptr<UnaryExprStmt> unary_expr;
    };
    std::vector<Term> terms;
    void accept(StmtVisitor &visitor) override;
};

// 一元表达式
class UnaryExprStmt : public BaseStmt {
public:
    enum class UnaryExprType { NULL_TYPE, Not, Minus };
    std::vector<UnaryExprType> types;
    std::unique_ptr<PrimaryExprStmt> primary_expr;
    void accept(StmtVisitor &visitor) override;
};

// 优先级最高的表达式
class PrimaryExprStmt : public BaseStmt {
public:
    enum class PrimaryExprType { NULL_TYPE, Value, Parentheses };
    PrimaryExprType type;
    std::unique_ptr<ValueStmt> value;
    std::unique_ptr<ExprStmt> expr;
    void accept(StmtVisitor &visitor) override;
};

// 值的stmt
class ValueStmt : public BaseStmt {
public:
    enum class ValueType { NULL_TYPE, Number, Str, LVal, FuncCall };
    ValueType type;
    std::unique_ptr<NumberStmt> number;
    std::unique_ptr<StrStmt> str;
    std::unique_ptr<LValStmt> lval;
    std::unique_ptr<FuncCallStmt> func_call;
    void accept(StmtVisitor &visitor) override;
};

// 保存数值类型的值
class NumberStmt : public BaseStmt {
public:
    bool is_real, is_signed, is_char, is_unsigned;
    union {
        long long int_val;
        double real_val;
        unsigned long long uint_val;
        char char_val;
    };
    std::string literal;
    void accept(StmtVisitor &visitor) override;
};

// 保存字符串类型的值
class StrStmt : public BaseStmt {
public:
    std::string val;
    void accept(StmtVisitor &visitor) override;
};

// 左值
class LValStmt : public BaseStmt {
public:
    std::string id;
    std::vector<std::unique_ptr<ExprStmt>> array_index;
    void accept(StmtVisitor &visitor) override;
};

// 函数调用
class FuncCallStmt : public BaseStmt {
public:
    std::string id;
    std::vector<std::unique_ptr<ExprStmt>> args;
    void accept(StmtVisitor &visitor) override;
};

// 声明语句相关类
enum class BasicType { VOID, INT, REAL, CHAR, BOOLEAN };
enum class DataType { NULL_TYPE, BasicType, ArrayType };

// 范围表达式
class PeriodStmt : public BaseStmt {
public:
    int begin, end;
    void accept(StmtVisitor &visitor) override;
};

// 常量声明语句
class ConstDeclStmt : public BaseStmt {
public:
    typedef std::pair<std::string, std::unique_ptr<ValueStmt>> KvPair;
    std::vector<KvPair> pairs;
    void accept(StmtVisitor &visitor) override;
};

// 变量声明语句
class VarDeclStmt : public BaseStmt {
public:
    std::vector<std::string> id;
    DataType data_type;
    BasicType basic_type;
    int type_size;
    bool is_var;
    std::vector<std::unique_ptr<PeriodStmt>> array_range;
    void accept(StmtVisitor &visitor) override;
};

// 函数（function）和过程（procedure）头部声明
class FuncHeadDeclStmt : public BaseStmt {
public:
    std::string func_name;
    BasicType ret_type;
    std::vector<std::unique_ptr<VarDeclStmt>> args;
    void accept(StmtVisitor &visitor) override;
};

// 函数体声明
class FuncBodyDeclStmt : public BaseStmt {
public:
    std::unique_ptr<ConstDeclStmt> const_decl;
    std::vector<std::unique_ptr<VarDeclStmt>> var_decl;
    std::vector<std::unique_ptr<BaseStmt>> comp_stmt;
    void accept(StmtVisitor &visitor) override;
};

// 函数声明
class FuncDeclStmt : public BaseStmt {
public:
    std::unique_ptr<FuncHeadDeclStmt> header;
    std::unique_ptr<FuncBodyDeclStmt> body;
    void accept(StmtVisitor &visitor) override;
};

// 功能语句相关类
// 赋值语句
class AssignStmt : public BaseStmt {
public:
    bool is_lval_func;
    std::unique_ptr<LValStmt> lval;
    std::unique_ptr<ExprStmt> expr;
    void accept(StmtVisitor &visitor) override;
};

// 分支语句(If)
class IfStmt : public BaseStmt {
public:
    std::unique_ptr<ExprStmt> expr;
    std::vector<std::unique_ptr<BaseStmt>> true_stmt;
    std::vector<std::unique_ptr<BaseStmt>> false_stmt;
    void accept(StmtVisitor &visitor) override;
};

// 循环语句
class ForStmt : public BaseStmt {
public:
    std::string id;
    std::unique_ptr<ExprStmt> begin;
    std::unique_ptr<ExprStmt> end;
    std::vector<std::unique_ptr<BaseStmt>> stmt;
    void accept(StmtVisitor &visitor) override;
};

// while循环语句
class WhileStmt : public BaseStmt {
public:
    std::unique_ptr<ExprStmt> expr;
    std::vector<std::unique_ptr<BaseStmt>> stmt;
    void accept(StmtVisitor &visitor) override;
};

// 输入函数
class ReadFuncStmt : public BaseStmt {
public:
    std::vector<std::unique_ptr<LValStmt>> lval;
    void accept(StmtVisitor &visitor) override;
};

// 输出函数
class WriteFuncStmt : public BaseStmt {
public:
    std::vector<std::unique_ptr<ExprStmt>> expr;
    void accept(StmtVisitor &visitor) override;
};

class BreakStmt : public BaseStmt {
public:
    void accept(StmtVisitor &visitor) override;
};

class ContinueStmt : public BaseStmt {
public:
    void accept(StmtVisitor &visitor) override;
};

// 主函数相关类
// 主函数头部
class ProgramHeadStmt : public BaseStmt {
public:
    std::vector<std::string> id_list;
    void accept(StmtVisitor &visitor) override;
};

// 主函数体
class ProgramBodyStmt : public BaseStmt {
public:
    std::unique_ptr<ConstDeclStmt> const_decl;
    std::vector<std::unique_ptr<VarDeclStmt>> var_decl;
    std::vector<std::unique_ptr<FuncDeclStmt>> func_decl;
    std::vector<std::unique_ptr<BaseStmt>> comp_stmt;
    void accept(StmtVisitor &visitor) override;
};

// 主函数
class ProgramStmt : public BaseStmt {
public:
    std::unique_ptr<ProgramHeadStmt> head;
    std::unique_ptr<ProgramBodyStmt> body;
    void accept(StmtVisitor &visitor) override;
};
