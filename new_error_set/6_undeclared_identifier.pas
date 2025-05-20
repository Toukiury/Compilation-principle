program variables;
var
  a: integer;
begin
  a := 5;
  b := 10;  // 错误：使用未声明的变量 b
  c := a + b;  // 错误：使用未声明的变量 c
end. 