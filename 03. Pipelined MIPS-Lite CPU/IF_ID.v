module IF_ID( RD_in,  RD_out, pc_in, pc_out, clk, rst ); 
	input [31:0] RD_in, pc_in;
	input rst, clk;
	output [31:0] RD_out, pc_out;
	
	reg [31:0] RD_out, pc_out;
	always @ ( posedge clk ) begin
		if ( rst ) begin
			RD_out <= 32'b0;	
			pc_out <= 32'b0;
		end
		else begin
			pc_out <= pc_in;
			RD_out <= RD_in;
		end	
	end
endmodule	