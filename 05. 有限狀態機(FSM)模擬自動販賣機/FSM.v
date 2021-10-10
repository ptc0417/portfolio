module FSM(drink_output, coin_total, clk, drink_choose, coin_input, reset);

input clk, reset ;
input [2:0] drink_choose ;
input [5:0] coin_input ;
output [7:0] coin_total ;
output [2:0] drink_output ;

reg [7:0] coin_total ;
reg [2:0] drink_output ;
reg [3:0] current_state, next_state ;

parameter S0 = 4'b0000 ;
parameter S1 = 4'b0001 ;
parameter S2 = 4'b0010 ;
parameter S3 = 4'b0011 ;

parameter tea = 3'b100 ;
parameter coke = 3'b101 ;
parameter coffee = 3'b110 ;
parameter milk = 3'b111 ;

initial
begin
	coin_total = 0 ;
end

always@(posedge clk)begin
	if( reset )
		current_state <= S3 ;
	else
		current_state <= next_state ;
end

always@(coin_input or drink_choose)begin
	if ( coin_input != 0 )
		next_state <= S0 ;
	
	if ( drink_choose == tea && coin_total >= 10 )
	begin
		drink_output <= tea ;
		coin_total = coin_total - 10 ;
		next_state <= S2 ;
	end
	
	else if ( drink_choose == coke && coin_total >= 15 )
	begin
		drink_output <= coke ;
		coin_total = coin_total - 15 ;
		next_state <= S2 ;
	end
	
	else if ( drink_choose == coffee && coin_total >= 20 )
	begin
		drink_output <= coffee ;
		coin_total = coin_total - 20 ;
		next_state <= S2 ;
	end
	
	else if ( drink_choose == milk && coin_total >= 25 )
	begin
		drink_output <= milk ;
		coin_total = coin_total - 25 ;
		next_state <= S2 ;
	end
	
	else if ( 4 <= drink_choose && drink_choose <= 7 )
		$display( "You can not choose it!!" ) ;
	
end


always@(current_state)begin
	case (current_state)
	
	S0:
	begin
		if ( coin_input != 0 )
		begin
			$display( "coin %d,\t", coin_input ) ;
			coin_total = coin_total + coin_input ;
			$display( "total %d dollars\t", coin_total ) ;
		end
		
		if ( coin_total >= 10 )
			next_state <= S1 ;
		else
			next_state = 4'b1000 ;
	end
	
	S1:
	begin
		if ( coin_total >= 25 )
			$display( "tea | coke | coffee | milk" ) ;
			
		else if ( coin_total >= 20 )
			$display( "tea | coke | coffee" ) ;
			
		else if ( coin_total >= 15 )
			$display( "tea | coke" ) ;
			
		else if ( coin_total >= 10 )
			$display( "tea" ) ;
		
	end
	
	S2:
	begin
		if ( drink_output == tea )
			$display( "tea out" ) ;
		else if ( drink_output == coke )
			$display( "coke out" ) ;
		else if ( drink_output == coffee )
			$display( "coffee out" ) ;
		else if ( drink_output == milk )
			$display( "milk out" ) ;
			
		drink_output = 0 ;
		next_state <= S3 ;
	end
	
	S3:
	begin
		if ( coin_total != 0 )
			$display( "exchange %d dollars", coin_total ) ;
		
		coin_total = 0 ;
		next_state <= S0 ;
	end
	
	endcase
	
end

endmodule
