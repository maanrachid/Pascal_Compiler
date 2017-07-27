program typeDemo(input, output);
type
   t1 = record
	   val, rootTrunc : integer;
	   root		  : integer
	end;		  
   t2 =  array[2..10] of t1;
var
   i : integer;
   a : t2;
begin
   i := 2;
   while (i <= 10) do
   begin
      a[i].val := i;
      a[i].root := 100;
      a[i].rootTrunc := 300;
      i := i + 1
   end;

   i := 2;
   while (i <= 10) do
   begin
      i := i + 1
   end;
end.
