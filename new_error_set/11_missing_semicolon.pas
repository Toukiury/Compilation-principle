program syntax;
var
  x, y: integer
begin  // 错误：变量声明后缺少分号
  x := 10;
  y := 20
  x := x + y;  // 错误：上一行缺少分号
  write('Result: ', x)  // 错误：缺少分号
end. 