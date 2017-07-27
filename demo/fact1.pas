program someName(input, output);
var 
   n, result : integer;
procedure factorial(k : integer; var answer : integer);
   var 
      i	: integer;
   begin
      i := 1;
      answer := 1;
      while (i <= k) do
      begin
	 answer := answer * i;
	 i := i + 1
      end
   end; { factorial }
begin
   writeString('Value for n: ');
   readInteger(n);
   factorial(n, result);
   writeString('The answer: ')
end.
