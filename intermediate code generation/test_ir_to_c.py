import json
from ir_generator import ir_to_c


def test_ir_to_c():
    # 从ir.json读取IR数据
    with open('ir.json', 'r') as f:
        ir_data = json.load(f)

    # 转换为C代码
    c_code = ir_to_c(ir_data)

    # 将结果写入output.c
    with open('output.c', 'w') as f:
        f.write(c_code)

    print("C code generated successfully in output.c")


if __name__ == '__main__':
    test_ir_to_c()