module Simulate() ;

reg clk, reset ;
reg [2:0] drink_choose ;
reg [5:0] coin_input ;

wire [2:0] drink_output ;
wire [7:0] coin_total ;

parameter tea = 3'b100 ;
parameter coke = 3'b101 ;
parameter coffee = 3'b110 ;
parameter milk = 3'b111 ;

FSM fsm( drink_output, coin_total, clk, drink_choose, coin_input, reset ) ;

initial clk = 1'b1;
always #5 clk = ~clk;

initial
begin
	// B-1
	#5
	coin_input = 10 ;
	
	#20
	coin_input = 1 ;
	
	#20
	coin_input = 10 ;
	
	#20
	coin_input = 0 ;
	drink_choose = coffee ;
	
	#20
	drink_choose = 0 ;
	
	#10
	reset = 1 ;
	
	// B-2
	#10
	reset = 0 ;
	coin_input = 5 ;
	
	#20
	coin_input = 10 ;
	
	#20
	reset = 1 ;
	coin_input = 0 ;
	
	#10
	reset = 0 ;
	coin_input = 10 ;
	
	#20
	coin_input = 0 ;
	
	#10
	coin_input = 10 ;
	
	#20
	coin_input = 0 ;
	
	#10
	coin_input = 1 ;
	
	#20
	coin_input = 0 ;
	
	#10
	coin_input = 1 ;
	
	#20
	coin_input = 0 ;
	
	#10
	coin_input = 1 ;
	
	#20
	coin_input = 0 ;
	
	#10
	coin_input = 1 ;
	
	#20
	coin_input = 0 ;
	
	#10
	coin_input = 1 ;
	
	#20
	coin_input = 0 ;
	drink_choose = milk ;
	
	#20
	drink_choose = 0 ;
	
	#10
	reset = 1 ;
	
	// B-3
	#10
	reset = 0 ;
	coin_input = 5 ;
	
	#20
	coin_input = 10 ;
	
	#20
	reset = 1 ;
	coin_input = 0 ;
	
	#10
	reset = 0 ;
	coin_input = 10 ;
	
	#20
	coin_input = 0 ;
	
	#10
	coin_input = 10 ;
	
	#20
	coin_input = 0 ;
	drink_choose = coke ;
	
	#20
	drink_choose = 0 ;
	
	#20
	coin_input = 10 ;
	
	#20
	coin_input = 0 ;
	
	#10
	coin_input = 10 ;
	
	#20
	coin_input = 0 ;
	drink_choose = milk ;
	
	#20
	drink_choose = 0 ;
	
	#20
	coin_input = 10 ;
	
	#20
	coin_input = 0 ;
	drink_choose = tea ;
	
	#20
	drink_choose = 0 ;
	
	#20
	coin_input = 50 ;
	
	#20
	coin_input = 0 ;
	drink_choose = coffee ;
	
	#20
	drink_choose = 0 ;
	
	#10
	reset = 1 ;
	
end

endmodule
