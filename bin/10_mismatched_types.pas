program type_check;
var
  a: integer;
  b: real;
  c: boolean;
begin
  a := 3.14;        // 错误：将实数赋值给整数
  c := 1;           // 错误：将整数赋值给布尔值
  b := a > c;       // 错误：将布尔表达式赋值给实数
  write(a + c);     // 错误：整数和布尔值相加
end. 