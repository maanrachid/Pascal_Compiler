program someName(input, output);
var 
   n, result : integer;
procedure factorial(k : integer; var answer : integer);
   begin
      if k = 0 then
	 answer := 1
      else
      begin
	 factorial(k - 1, answer);
	 answer := answer * k
      end
   end; { factorial }
begin
   writeString('Value for n: ');
   readInteger(n);
   factorial(n, result);
   writeln;
end.
