module HiLo( clk, DivAns, HiOut, LoOut, reset );
input clk ;
input reset ;
input [63:0] DivAns ;
output [31:0] HiOut ;
output [31:0] LoOut ;

reg [63:0] HiLo ;
reg [31:0] HiOut;
reg [31:0] LoOut;

always@( posedge clk or reset )
begin
  if ( reset )
  begin
    HiOut = 32'b0 ;
	LoOut = 32'b0 ;
  end
  else
  begin
    HiLo = DivAns ;
	HiOut = HiLo[63:32] ;
	LoOut = HiLo[31:0] ;
  end

end

endmodule