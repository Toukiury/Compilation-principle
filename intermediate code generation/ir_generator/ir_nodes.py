"""
结构化IR节点定义
使用字典格式表示IR，便于序列化和后续处理
"""

# 定义节点类型常量
PROGRAM = "Program"
FUNCTION = "Function"
VAR_DECL = "VarDecl"
ASSIGNMENT = "Assignment"
IF_STATEMENT = "IfStatement"
BINARY_OP = "BinaryOp"
FUNCTION_CALL = "FunctionCall"
BLOCK = "Block"
IDENTIFIER = "Identifier"
LITERAL = "Literal"
RETURN = "Return"

def create_ir_node(node_type, **kwargs):
    """
    创建结构化IR节点（字典格式）
    :param node_type: 节点类型
    :param kwargs: 节点属性
    :return: 字典形式的IR节点
    """
    node = {"node_type": node_type}
    node.update(kwargs)
    return node