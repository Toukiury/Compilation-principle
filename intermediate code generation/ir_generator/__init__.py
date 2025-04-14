"""Pascal-S 到 C 编译器的中间代码生成模块"""
from .ast_to_ir import ast_to_ir
from .ir_to_c import ir_to_c

__all__ = ['ast_to_ir', 'ir_to_c']