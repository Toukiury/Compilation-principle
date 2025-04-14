import json
from ir_generator import ast_to_ir


def read_annotated_ast(file_path):
    """读取带注释的AST文件"""
    with open(file_path, 'r', encoding='utf-8') as f:
       
        content = f.read()
        try:
            return json.loads(content)
        except json.JSONDecodeError:
            # 如果JSON解析失败，尝试作为Python字典解析
            import ast
            return ast.literal_eval(content)


def write_structured_ir(ir_node, file_path):
    """将结构化IR写入文件（JSON格式）"""
    with open(file_path, 'w', encoding='utf-8') as f:
        json.dump(ir_node, f, indent=2, ensure_ascii=False)


def main():
    # 读取输入的带注释AST
    input_file = "annotated_ast.txt"
    ast_data = read_annotated_ast(input_file)

    # 转换为结构化IR
    ir_root = ast_to_ir(ast_data)

    # 写入输出文件
    output_file = "ir.json"
    write_structured_ir(ir_root, output_file)

    print(f"Successfully converted AST to structured IR and saved to {output_file}")


if __name__ == "__main__":
    main()