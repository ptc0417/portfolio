module divider( clk, dataA, dataB, Signal, dataOut, reset );
input clk ;
input reset ;
input [31:0] dataA ;
input [31:0] dataB ;
input Signal ;
output [63:0] dataOut ;

reg [63:0] rem;
reg [31:0] quot;
reg [63:0] dir; 
reg [63:0] temp; 
reg [63:0] dataOut;
reg [5:0] count;
reg i;

always@( posedge clk or reset )
begin
	if (reset)
		begin
		    temp = 64'b0;
		end
	else begin
		if ( Signal ) begin
			temp = 64'b0 ;
			quot = 32'b0 ;
			rem = {32'b0, dataA}; 
			dir = {dataB,32'b0};
			count = 6'b0;
			i = 1;
		end	
		if ( i == 1'b1 ) begin
			if ( count == 6'd33 ) begin		
				temp[31:0] = quot;
				temp[63:32] = rem;
				dataOut = temp ;
			end
			else if ( count < 6'd33 ) begin
				rem = rem - dir ;
				if ( $signed(rem) >= $signed(0) )
					quot = {quot[30:0],1'b1};
					
				else begin
					rem = rem + dir;
					quot = {quot[30:0],1'b0};
				end
				dir = dir >> 1;
			end
			count = count + 1;
		end	
	end

end

endmodule