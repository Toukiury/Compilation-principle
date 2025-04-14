import json


def translate_type(pascal_type):
    """将Pascal类型转换为C类型"""
    type_mapping = {
        'integer': 'int',
        'real': 'float',
        'boolean': 'bool',
        'char': 'char',
        'string': 'char*'
    }
    return type_mapping.get(pascal_type, pascal_type)


def generate_var_decl(var_decl):
    """生成变量声明代码"""
    var_type = translate_type(var_decl['var_type'])
    declarations = []
    for name in var_decl['names']:
        declarations.append(f"{var_type} {name};")
    return '\n'.join(declarations)


def generate_identifier(identifier):
    """生成标识符代码"""
    return identifier['name']


def generate_literal(literal):
    """生成字面量代码"""
    value = literal['value']
    literal_type = literal.get('literal_type', 'unknown')

    if literal_type == 'string':
        return f'"{value[1:-1]}"' if value.startswith("'") else f'"{value}"'
    elif literal_type == 'char':
        return f"'{value}'" if len(value) == 1 else value
    return str(value)


def generate_binary_op(binary_op):
    """生成二元运算代码"""
    left = generate_code(binary_op['left'])
    right = generate_code(binary_op['right'])
    operator = binary_op['operator']

    operator_mapping = {
        'and': '&&',
        'or': '||',
        'mod': '%',
        'div': '/',
        '=': '==',
        '<>': '!='
    }
    operator = operator_mapping.get(operator, operator)

    return f"({left} {operator} {right})"


def generate_assignment(assignment):
    """生成赋值语句代码"""
    left = generate_code(assignment['left'])
    right = generate_code(assignment['right'])
    return f"{left} = {right};"


def generate_block(block):
    """生成代码块"""
    statements = [generate_code(stmt) for stmt in block['statements']]
    return '\n'.join(statements)


def generate_if_statement(if_stmt):
    """生成if语句代码"""
    condition = generate_code(if_stmt['condition'])
    then_branch = generate_code(if_stmt['then'])

    else_branch = ""
    if if_stmt.get('else_'):
        else_branch = f"else {{\n{generate_code(if_stmt['else_'])}}}"

    return f"if ({condition}) {{\n{then_branch}\n}}{else_branch}"


def generate_function_call(func_call):
    """生成函数调用代码，特殊处理Pascal的write/writeln"""
    if func_call['name'] in ('write', 'writeln'):
        return generate_pascal_output(func_call)
    else:
        args = ', '.join(generate_code(arg) for arg in func_call['args'])
        return f"{func_call['name']}({args});"


def generate_pascal_output(func_call):
    """专门处理Pascal的write/writeln输出语句"""
    args = func_call['args']
    needs_printf = False
    format_parts = []
    value_parts = []

    for arg in args:
        if arg['node_type'] == 'Literal':
            literal_type = arg.get('literal_type', 'unknown')
            value = arg['value']

            if literal_type == 'string':
                format_parts.append("%s")
                value_parts.append(f'"{value[1:-1]}"' if value.startswith("'") else f'"{value}"')
            elif literal_type == 'char':
                format_parts.append("%c")
                value_parts.append(f"'{value}'" if len(value) == 1 else value)
            elif literal_type == 'integer':
                format_parts.append("%d")
                value_parts.append(str(value))
            elif literal_type == 'real':
                format_parts.append("%f")
                value_parts.append(str(value))
            elif literal_type == 'boolean':
                format_parts.append("%s")
                value_parts.append("true" if value else "false")
            else:
                format_parts.append("%d")
                value_parts.append(str(value))
            needs_printf = True
        else:
            format_parts.append("%d")
            value_parts.append(generate_code(arg))
            needs_printf = True

    if not needs_printf:
        return ""

    format_str = '"' + ' '.join(format_parts) + ('\\n" ' if func_call['name'] == 'writeln' else '" ')
    value_str = ', '.join(value_parts)
    return f'printf({format_str}, {value_str});'


def generate_program(program):
    """生成整个程序代码"""
    declarations = []
    has_output = False

    def check_for_output(node):
        nonlocal has_output
        if isinstance(node, dict):
            if node.get('node_type') == 'FunctionCall' and node['name'] in ('write', 'writeln'):
                has_output = True
            for value in node.values():
                check_for_output(value)
        elif isinstance(node, list):
            for item in node:
                check_for_output(item)

    check_for_output(program)

    for decl in program['declarations']:
        declarations.append(generate_code(decl))

    body = generate_code(program['body'])

    headers = ['#include <stdbool.h>']
    if has_output:
        headers.append('#include <stdio.h>')

    return f"""{'\n'.join(headers)}

/* Global declarations */
{'\n'.join(declarations)}

int main() {{
    /* Program body */
    {body}
    return 0;
}}
"""


def generate_code(node):
    """根据IR节点生成对应的C代码"""
    node_type = node['node_type']

    generators = {
        'Program': generate_program,
        'VarDecl': generate_var_decl,
        'Assignment': generate_assignment,
        'Block': generate_block,
        'Identifier': generate_identifier,
        'Literal': generate_literal,
        'BinaryOp': generate_binary_op,
        'IfStatement': generate_if_statement,
        'FunctionCall': generate_function_call
    }

    generator = generators.get(node_type)
    if generator:
        return generator(node)
    else:
        raise ValueError(f"Unsupported node type: {node_type}")


def ir_to_c(ir_data):
    """将IR数据转换为C代码"""
    if isinstance(ir_data, str):
        ir_data = json.loads(ir_data)
    return generate_code(ir_data)