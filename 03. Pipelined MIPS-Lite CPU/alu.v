
module alu(ctl, a, b, result, shamt);
  input  [2:0]  ctl ; 
  input  [4:0]  shamt;
  input  [31:0] a, b ;
  output [31:0] result ;

  wire [31:0] srl_ans, temp ;
  wire [31:0] c ;
  wire [1:0] sel ;
  wire set, binvert ;

  alu_one tao0( .a(a[0]), .b(b[0]), .cin(ctl[2]), .cout(c[0]), .sel(ctl[1:0]), .binv(ctl[2]), .less(set), .si(temp[0]) ) ;
  alu_one tao1( .a(a[1]), .b(b[1]), .cin(c[0]), .cout(c[1]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[1]) ) ;
  alu_one tao2( .a(a[2]), .b(b[2]), .cin(c[1]), .cout(c[2]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[2]) ) ;
  alu_one tao3( .a(a[3]), .b(b[3]), .cin(c[2]), .cout(c[3]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[3]) ) ;
  alu_one tao4( .a(a[4]), .b(b[4]), .cin(c[3]), .cout(c[4]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[4]) ) ;
  alu_one tao5( .a(a[5]), .b(b[5]), .cin(c[4]), .cout(c[5]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[5]) ) ;
  alu_one tao6( .a(a[6]), .b(b[6]), .cin(c[5]), .cout(c[6]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[6]) ) ;
  alu_one tao7( .a(a[7]), .b(b[7]), .cin(c[6]), .cout(c[7]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[7]) ) ;
  alu_one tao8( .a(a[8]), .b(b[8]), .cin(c[7]), .cout(c[8]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[8]) ) ;
  alu_one tao9( .a(a[9]), .b(b[9]), .cin(c[8]), .cout(c[9]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[9]) ) ;
  alu_one tao10( .a(a[10]), .b(b[10]), .cin(c[9]), .cout(c[10]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[10]) ) ;
  alu_one tao11( .a(a[11]), .b(b[11]), .cin(c[10]), .cout(c[11]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[11]) ) ;
  alu_one tao12( .a(a[12]), .b(b[12]), .cin(c[11]), .cout(c[12]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[12]) ) ;
  alu_one tao13( .a(a[13]), .b(b[13]), .cin(c[12]), .cout(c[13]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[13]) ) ;
  alu_one tao14( .a(a[14]), .b(b[14]), .cin(c[13]), .cout(c[14]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[14]) ) ;
  alu_one tao15( .a(a[15]), .b(b[15]), .cin(c[14]), .cout(c[15]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[15]) ) ;
  alu_one tao16( .a(a[16]), .b(b[16]), .cin(c[15]), .cout(c[16]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[16]) ) ;
  alu_one tao17( .a(a[17]), .b(b[17]), .cin(c[16]), .cout(c[17]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[17]) ) ;
  alu_one tao18( .a(a[18]), .b(b[18]), .cin(c[17]), .cout(c[18]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[18]) ) ;
  alu_one tao19( .a(a[19]), .b(b[19]), .cin(c[18]), .cout(c[19]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[19]) ) ;
  alu_one tao20( .a(a[20]), .b(b[20]), .cin(c[19]), .cout(c[20]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[20]) ) ;
  alu_one tao21( .a(a[21]), .b(b[21]), .cin(c[20]), .cout(c[21]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[21]) ) ;
  alu_one tao22( .a(a[22]), .b(b[22]), .cin(c[21]), .cout(c[22]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[22]) ) ;
  alu_one tao23( .a(a[23]), .b(b[23]), .cin(c[22]), .cout(c[23]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[23]) ) ;
  alu_one tao24( .a(a[24]), .b(b[24]), .cin(c[23]), .cout(c[24]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[24]) ) ;
  alu_one tao25( .a(a[25]), .b(b[25]), .cin(c[24]), .cout(c[25]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[25]) ) ;
  alu_one tao26( .a(a[26]), .b(b[26]), .cin(c[25]), .cout(c[26]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[26]) ) ;
  alu_one tao27( .a(a[27]), .b(b[27]), .cin(c[26]), .cout(c[27]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[27]) ) ;
  alu_one tao28( .a(a[28]), .b(b[28]), .cin(c[27]), .cout(c[28]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[28]) ) ;
  alu_one tao29( .a(a[29]), .b(b[29]), .cin(c[28]), .cout(c[29]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[29]) ) ;
  alu_one tao30( .a(a[30]), .b(b[30]), .cin(c[29]), .cout(c[30]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[30]) ) ;
  alu_31 tao31( .a(a[31]), .b(b[31]), .cin(c[30]), .cout(c[31]), .sel(ctl[1:0]), .binv(ctl[2]), .less(1'b0), .si(temp[31]), .set(set) ) ;
  
  Shifter shifter( .dataA(b), .dataB(shamt), .dataOut(srl_ans) );

 assign result = (ctl == 3'b011)? srl_ans : temp ;

endmodule
