module MEM_WB( RD_in, RD_out, ALU_in, ALU_out, WN_in, WN_out, WB, MemtoReg, RegWrite, clk, rst );
	input [31:0] RD_in, ALU_in;
	input [4:0] WN_in;
	input [1:0] WB;
	input clk, rst;
	output [31:0] RD_out, ALU_out;
	output [4:0] WN_out;
	output MemtoReg, RegWrite;
	
	reg [31:0] RD_out, ALU_out;
	reg [4:0] WN_out;
	reg MemtoReg, RegWrite;
	
	always @ ( posedge clk ) begin
		if ( rst ) begin
			RD_out <= 32'b0;
			ALU_out <= 32'b0;
			WN_out <= 5'b0;
			MemtoReg <= 1'b0;
			RegWrite <= 1'b0;
		end
		else begin
			RD_out <= RD_in;
			ALU_out <= ALU_in;
			WN_out <= WN_in;
			MemtoReg <= WB[1];
			RegWrite <= WB[0];
		end
	end
endmodule	