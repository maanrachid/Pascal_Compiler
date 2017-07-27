program arrDemo(input, output);
type
   t1	= array[1..10, 1..10] of integer;
   { t1	= array[1..10] of array[1..10] of integer; }
   t2	= t1; {just to show that types can be renamed }
var
   a1	: t1;
   a2	: t2;
   i, j	:  integer;
begin
   i := 1;
   while i <= 10 do
   begin
      j := 1;
      while j <= 10 do
      begin
	 a1[i, j] := i - j;
	 j := j + 1;
      end;
      i := i + 1
   end;

   a2 := a1;   { Can assign entire arrays  }
   
   i := 1;
   while i <= 10 do
   begin
      j := 1;
      while j <= 10 do
      begin
	 writeReal(a2[i][j]);
	 j := j + 1;
      end;
      writeln;
      i := i + 1
   end
end.

