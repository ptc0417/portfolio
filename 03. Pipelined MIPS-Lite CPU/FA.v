// Full Adder
module FA( a, b, c, cout, sum ) ;
 
input a, b, c ;
output cout, sum ;
wire e0, e1, e2, e3 ;

xor( e0, a, b ) ;
xor( sum, e0, c ) ;

or( e1, a, b ) ;
and( e2, e1, c ) ;
and( e3, a, b ) ;
or ( cout, e2, e3 ) ;

endmodule
