begin
    integer aVeryLongVariable114514;
    integer function F(n);
        begin
            integer n;
            if n <= 0 then F := 1
            else F := n * F(n - 1)
        end;
    read(m);
    aVeryLongVariable114514 := F(m);
    write(aVeryLongVariable114514);
end