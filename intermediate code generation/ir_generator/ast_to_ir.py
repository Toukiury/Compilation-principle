"""
AST到IR转换器
"""

from .ast_nodes import *
from .ir_nodes import *


def ast_to_ir(ast_node):
    """
    将带注释的AST转换为结构化IR（字典格式）
    :param ast_node: 字典形式的AST节点
    :return: 字典形式的IR节点
    """
    node_type = ast_node["type"]

    if node_type == "Program":
        # 处理程序节点
        declarations = [ast_to_ir(decl) for decl in ast_node["declarations"]]
        body = ast_to_ir(ast_node["body"])
        return create_ir_node(
            PROGRAM,
            name=ast_node["name"],
            declarations=declarations,
            body=body
        )

    elif node_type == "VarDeclaration":
        # 处理变量声明
        return create_ir_node(
            VAR_DECL,
            names=ast_node["names"],
            var_type=ast_node["var_type"],
            scope=ast_node["scope"]
        )

    elif node_type == "FunctionDeclaration":
        # 处理函数声明
        params = [{
            "name": p["name"],
            "param_type": p["param_type"],
            "pass_by": p["pass_by"]
        } for p in ast_node["params"]]

        body = ast_to_ir(ast_node["body"])
        return create_ir_node(
            FUNCTION,
            name=ast_node["name"],
            params=params,
            return_type=ast_node["return_type"],
            body=body,
            scope=ast_node["scope"]
        )

    elif node_type == "Block":
        # 处理语句块
        statements = [ast_to_ir(stmt) for stmt in ast_node["statements"]]
        return create_ir_node(BLOCK, statements=statements)

    elif node_type == "Assignment":
        # 处理赋值语句
        left = ast_to_ir(ast_node["left"])
        right = ast_to_ir(ast_node["right"])
        return create_ir_node(ASSIGNMENT, left=left, right=right)

    elif node_type == "IfStatement":
        # 处理if语句
        condition = ast_to_ir(ast_node["condition"])
        then_branch = ast_to_ir(ast_node["then"])
        else_branch = ast_to_ir(ast_node["else"]) if "else" in ast_node else None
        return create_ir_node(
            IF_STATEMENT,
            condition=condition,
            then=then_branch,
            else_=else_branch
        )

    elif node_type == "BinaryExpression":
        # 处理二元表达式
        left = ast_to_ir(ast_node["left"])
        right = ast_to_ir(ast_node["right"])
        return create_ir_node(
            BINARY_OP,
            operator=ast_node["operator"],
            left=left,
            right=right,
            data_type=ast_node.get("data_type")
        )

    elif node_type == "FunctionCall":
        # 处理函数调用
        args = [ast_to_ir(arg) for arg in ast_node["args"]]
        return create_ir_node(
            FUNCTION_CALL,
            name=ast_node["name"],
            args=args,
            return_type=ast_node["return_type"]
        )

    elif node_type == "Identifier":
        # 处理标识符
        return create_ir_node(
            IDENTIFIER,
            name=ast_node["name"],
            var_type=ast_node["var_type"]
        )

    elif node_type == "IntegerLiteral":
        # 处理整数字面量
        return create_ir_node(
            LITERAL,
            value=ast_node["value"],
            literal_type="integer"
        )

    elif node_type == "RealLiteral":
        # 处理实数字面量
        return create_ir_node(
            LITERAL,
            value=ast_node["value"],
            literal_type="real"
        )

    elif node_type == "CharLiteral":
        # 处理字符字面量
        return create_ir_node(
            LITERAL,
            value=ast_node["value"],
            literal_type="char"
        )

    elif node_type == "ReturnStatement":
        # 处理返回语句
        expr = ast_to_ir(ast_node["expression"]) if "expression" in ast_node else None
        return create_ir_node(RETURN, expression=expr)

    else:
        raise ValueError(f"Unknown AST node type: {node_type}")
        raise ValueError(f"Unknown AST node type: {node_type}")