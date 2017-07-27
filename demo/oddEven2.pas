program oddEven1(input, output);
var
   k : integer;
procedure announceOddEven(i: integer);
   var
      isEven : boolean;
   procedure odd(n: integer; var b : boolean); forward;
   procedure even(n : integer; var b : boolean);
      begin
	 if n = 0 then
	    b := true
	 else
	    odd(n - 1, b)
      end; { even }
   procedure odd;
      begin
	 if n = 0 then
	    b := false
	 else
	    even(n - 1, b)
      end; { odd }
   begin
      even(i, isEven);
      if isEven then
	 writeln('The value was even.')
      else
	 writeln('The value was odd.')
   end; { announceOddEven }
begin
   write('Value of k: ');
   read(k);
   announceOddEven(k);
end.
