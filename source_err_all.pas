begin
    integer aVeryLongVariableHaHa!;
    integer function &F(n);
        begin
            integer n;
            if n <= 0 then F :(= 1
            else F :+= n * F(n - 1)
        end;
    read(m#);
    aVeryLongVariableHaHa :~= F(m);
    write(aVeryLongVariableHaHa);
end$