#pragma once

// 前置声明各类语句，用于后续定义访问者模式的接口
// 表达式语句
class ExprStmt;
// 关系表达式语句
class RelExprStmt;
// 加法类表达式语句
class AddExprStmt;
// 乘法类表达式语句
class MulExprStmt;
// 一元表达式语句
class UnaryExprStmt;
// 主表达式语句
class PrimaryExprStmt;
// 值语句
class ValueStmt;
// 数字语句
class NumberStmt;
// 字符串语句
class StrStmt;
// 左值语句
class LValStmt;

// 函数调用语句
class FuncCallStmt;
// 范围语句
class PeriodStmt;
// 常量声明语句
class ConstDeclStmt;
// 变量声明语句
class VarDeclStmt;
// 函数头部声明语句
class FuncHeadDeclStmt;
// 函数体声明语句
class FuncBodyDeclStmt;
// 函数声明语句
class FuncDeclStmt;
// 赋值语句
class AssignStmt;
// 条件语句（if）
class IfStmt;
// 循环语句（for）
class ForStmt;
// 循环语句（while）
class WhileStmt;
// 读取函数语句
class ReadFuncStmt;
// 写入函数语句
class WriteFuncStmt;
// 跳出循环语句
class BreakStmt;
// 继续循环语句
class ContinueStmt;
// 程序头部语句
class ProgramHeadStmt;
// 程序体语句
class ProgramBodyStmt;
// 程序语句
class ProgramStmt;

/**
 * @brief 语句访问者的抽象基类，定义了访问各种语句的接口
 * 
 * 该类使用了访问者模式，允许在不改变语句类本身的情况下，定义新的操作。
 * 派生类需要实现所有的 visit 方法，以处理不同类型的语句。
 */
class StmtVisitor {
public:
    /**
     * @brief 访问表达式语句
     * @param stmt 表达式语句对象的引用
     */
    virtual void visit(ExprStmt &stmt) = 0;
    /**
     * @brief 访问关系表达式语句
     * @param stmt 关系表达式语句对象的引用
     */
    virtual void visit(RelExprStmt &stmt) = 0;
    /**
     * @brief 访问加法类表达式语句
     * @param stmt 加法类表达式语句对象的引用
     */
    virtual void visit(AddExprStmt &stmt) = 0;
    /**
     * @brief 访问乘法类表达式语句
     * @param stmt 乘法类表达式语句对象的引用
     */
    virtual void visit(MulExprStmt &stmt) = 0;
    /**
     * @brief 访问一元表达式语句
     * @param stmt 一元表达式语句对象的引用
     */
    virtual void visit(UnaryExprStmt &stmt) = 0;
    /**
     * @brief 访问主表达式语句
     * @param stmt 主表达式语句对象的引用
     */
    virtual void visit(PrimaryExprStmt &stmt) = 0;
    /**
     * @brief 访问值语句
     * @param stmt 值语句对象的引用
     */
    virtual void visit(ValueStmt &stmt) = 0;
    /**
     * @brief 访问数字语句
     * @param stmt 数字语句对象的引用
     */
    virtual void visit(NumberStmt &stmt) = 0;
    /**
     * @brief 访问字符串语句
     * @param stmt 字符串语句对象的引用
     */
    virtual void visit(StrStmt &stmt) = 0;
    /**
     * @brief 访问左值语句
     * @param stmt 左值语句对象的引用
     */
    virtual void visit(LValStmt &stmt) = 0;
    /**
     * @brief 访问函数调用语句
     * @param stmt 函数调用语句对象的引用
     */
    virtual void visit(FuncCallStmt &stmt) = 0;
    /**
     * @brief 访问范围语句
     * @param stmt 范围语句对象的引用
     */
    virtual void visit(PeriodStmt &stmt) = 0;
    /**
     * @brief 访问常量声明语句
     * @param stmt 常量声明语句对象的引用
     */
    virtual void visit(ConstDeclStmt &stmt) = 0;
    /**
     * @brief 访问变量声明语句
     * @param stmt 变量声明语句对象的引用
     */
    virtual void visit(VarDeclStmt &stmt) = 0;
    /**
     * @brief 访问函数头部声明语句
     * @param stmt 函数头部声明语句对象的引用
     */
    virtual void visit(FuncHeadDeclStmt &stmt) = 0;
    /**
     * @brief 访问函数体声明语句
     * @param stmt 函数体声明语句对象的引用
     */
    virtual void visit(FuncBodyDeclStmt &stmt) = 0;
    /**
     * @brief 访问函数声明语句
     * @param stmt 函数声明语句对象的引用
     */
    virtual void visit(FuncDeclStmt &stmt) = 0;
    /**
     * @brief 访问赋值语句
     * @param stmt 赋值语句对象的引用
     */
    virtual void visit(AssignStmt &stmt) = 0;
    /**
     * @brief 访问条件语句（if）
     * @param stmt 条件语句对象的引用
     */
    virtual void visit(IfStmt &stmt) = 0;
    /**
     * @brief 访问循环语句（for）
     * @param stmt 循环语句对象的引用
     */
    virtual void visit(ForStmt &stmt) = 0;
    /**
     * @brief 访问循环语句（while）
     * @param stmt 循环语句对象的引用
     */
    virtual void visit(WhileStmt &stmt) = 0; 
    /**
     * @brief 访问读取函数语句
     * @param stmt 读取函数语句对象的引用
     */
    virtual void visit(ReadFuncStmt &stmt) = 0;
    /**
     * @brief 访问写入函数语句
     * @param stmt 写入函数语句对象的引用
     */
    virtual void visit(WriteFuncStmt &stmt) = 0;
    /**
     * @brief 访问跳出循环语句
     * @param stmt 跳出循环语句对象的引用
     */
    virtual void visit(BreakStmt &stmt) = 0;
    /**
     * @brief 访问继续循环语句
     * @param stmt 继续循环语句对象的引用
     */
    virtual void visit(ContinueStmt &stmt) = 0;
    /**
     * @brief 访问程序头部语句
     * @param stmt 程序头部语句对象的引用
     */
    virtual void visit(ProgramHeadStmt &stmt) = 0;
    /**
     * @brief 访问程序体语句
     * @param stmt 程序体语句对象的引用
     */
    virtual void visit(ProgramBodyStmt &stmt) = 0;
    /**
     * @brief 访问程序语句
     * @param stmt 程序语句对象的引用
     */
    virtual void visit(ProgramStmt &stmt) = 0;
};
