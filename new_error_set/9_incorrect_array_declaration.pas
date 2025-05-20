program arrays;
var
  numbers: array[-5..10] of integer;  // 错误：数组下标不能为负数
  matrix: array[1..3, 1..4] of real;  // 错误：Pascal 数组声明语法错误
begin
  numbers[1] := 100;
  matrix[1,2] := 3.14;
end. 