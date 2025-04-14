class ASTNode:
    """AST节点基类"""
    def __init__(self, node_type, value=None, children=None, metadata=None):
        self.node_type = node_type  # 节点类型(大写，如PROGRAM/VAR等)
        self.value = value          # 节点值(如标识符名)
        self.children = children or []  # 子节点列表
        self.metadata = metadata or {}   # 元数据(类型/作用域等)
    
    def add_child(self, child_node):
        self.children.append(child_node)
    
    def __repr__(self):
        meta = f" [{self.metadata}]" if self.metadata else ""
        val = f"({self.value})" if self.value is not None else ""
        return f"{self.node_type}{val}{meta}"

# AST节点类型常量(与文本AST一致)
PROGRAM = "PROGRAM"
VAR = "VAR"
TOKEN_BEGIN = "TOKEN_BEGIN"
IDENTIFIER = "IDENTIFIER"
INTEGER = "INTEGER"
REAL = "REAL"
CHAR = "CHAR"
BOOLEAN = "BOOLEAN"
ASSIGN = "ASSIGN"
IF = "IF"
WHILE = "WHILE"
FOR = "FOR"
PLUS = "PLUS"
MINUS = "MINUS"
MULTIPLY = "MULTIPLY"
DIVIDE = "DIVIDE"
GREATER = "GREATER"
LESS = "LESS"
EQUAL = "EQUAL"
WRITE = "WRITE"
READ = "READ"
INTEGER_CONST = "INTEGER_CONST"
REAL_CONST = "REAL_CONST"
CHAR_CONST = "CHAR_CONST"
BOOL_CONST = "BOOL_CONST"