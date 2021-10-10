// 1-bit ALU
module alu_one( a, b, cin, cout, sel, binv, less, si ) ;

input a, b, cin, binv, less ;
input[1:0] sel ;
output cout, si ;
wire xor_b ;
wire e0, e1 ;
wire fasi ;

xor( xor_b, b, binv ) ;
or( e1, a, b ) ;
and( e0, a, b ) ;

FA fFA( .a(a), .b(xor_b), .c(cin), .cout(cout), .sum(fasi) ) ;

assign si = (sel==2'b00)?e0:
            (sel==2'b01)?e1:
            (sel==2'b10)?fasi:
            (sel==2'b11)?less:
             1'b0 ;

endmodule
