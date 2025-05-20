#include "ast/ast_visitor.hpp"
#include "ast/ast_stmt.hpp"

/**
 * @brief ExprStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问ExprStmt对象。它调用visitor的visit方法，
 * 并将当前ExprStmt对象的引用传递给它，从而实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前表达式语句
 */
void ExprStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief RelExprStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问RelExprStmt对象。它调用visitor的visit方法，
 * 并将当前RelExprStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前关系表达式语句
 */
void RelExprStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief AddExprStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问AddExprStmt对象。它调用visitor的visit方法，
 * 并将当前AddExprStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前加法表达式语句
 */
void AddExprStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief MulExprStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问MulExprStmt对象。它调用visitor的visit方法，
 * 并将当前MulExprStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前乘法表达式语句
 */
void MulExprStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief UnaryExprStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问UnaryExprStmt对象。它调用visitor的visit方法，
 * 并将当前UnaryExprStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前一元表达式语句
 */
void UnaryExprStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief PrimaryExprStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问PrimaryExprStmt对象。它调用visitor的visit方法，
 * 并将当前PrimaryExprStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前主表达式语句
 */
void PrimaryExprStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief ValueStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问ValueStmt对象。它调用visitor的visit方法，
 * 并将当前ValueStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前值语句
 */
void ValueStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief NumberStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问NumberStmt对象。它调用visitor的visit方法，
 * 并将当前NumberStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前数字语句
 */
void NumberStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief StrStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问StrStmt对象。它调用visitor的visit方法，
 * 并将当前StrStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前字符串语句
 */
void StrStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief LValStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问LValStmt对象。它调用visitor的visit方法，
 * 并将当前LValStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前左值语句
 */
void LValStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief FuncCallStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问FuncCallStmt对象。它调用visitor的visit方法，
 * 并将当前FuncCallStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前函数调用语句
 */
void FuncCallStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief PeriodStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问PeriodStmt对象。它调用visitor的visit方法，
 * 并将当前PeriodStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前范围语句
 */
void PeriodStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief ConstDeclStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问ConstDeclStmt对象。它调用visitor的visit方法，
 * 并将当前ConstDeclStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前常量声明语句
 */
void ConstDeclStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief VarDeclStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问VarDeclStmt对象。它调用visitor的visit方法，
 * 并将当前VarDeclStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前变量声明语句
 */
void VarDeclStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief FuncHeadDeclStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问FuncHeadDeclStmt对象。它调用visitor的visit方法，
 * 并将当前FuncHeadDeclStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前函数头部声明语句
 */
void FuncHeadDeclStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief FuncBodyDeclStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问FuncBodyDeclStmt对象。它调用visitor的visit方法，
 * 并将当前FuncBodyDeclStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前函数体声明语句
 */
void FuncBodyDeclStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief FuncDeclStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问FuncDeclStmt对象。它调用visitor的visit方法，
 * 并将当前FuncDeclStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前函数声明语句
 */
void FuncDeclStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief AssignStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问AssignStmt对象。它调用visitor的visit方法，
 * 并将当前AssignStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前赋值语句
 */
void AssignStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief IfStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问IfStmt对象。它调用visitor的visit方法，
 * 并将当前IfStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前条件语句（if）
 */
void IfStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief ForStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问ForStmt对象。它调用visitor的visit方法，
 * 并将当前ForStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前循环语句（for）
 */
void ForStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief WhileStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问WhileStmt对象。它调用visitor的visit方法，
 * 并将当前WhileStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前循环语句（while）
 */
void WhileStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief ReadFuncStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问ReadFuncStmt对象。它调用visitor的visit方法，
 * 并将当前ReadFuncStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前读取函数语句
 */
void ReadFuncStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief WriteFuncStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问WriteFuncStmt对象。它调用visitor的visit方法，
 * 并将当前WriteFuncStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前写入函数语句
 */
void WriteFuncStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief ProgramHeadStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问ProgramHeadStmt对象。它调用visitor的visit方法，
 * 并将当前ProgramHeadStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前程序头部语句
 */
void ProgramHeadStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief ProgramBodyStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问ProgramBodyStmt对象。它调用visitor的visit方法，
 * 并将当前ProgramBodyStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前程序体语句
 */
void ProgramBodyStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief ProgramStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问ProgramStmt对象。它调用visitor的visit方法，
 * 并将当前ProgramStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前程序语句
 */
void ProgramStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief BreakStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问BreakStmt对象。它调用visitor的visit方法，
 * 并将当前BreakStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前跳出循环语句
 */
void BreakStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}

/**
 * @brief ContinueStmt类的accept方法实现
 * 
 * 该方法允许StmtVisitor访问ContinueStmt对象。它调用visitor的visit方法，
 * 并将当前ContinueStmt对象的引用传递给它，实现访问者模式。
 * 
 * @param visitor 一个StmtVisitor对象的引用，用于访问当前继续循环语句
 */
void ContinueStmt::accept(StmtVisitor &visitor)
{
    visitor.visit(*this);
}
