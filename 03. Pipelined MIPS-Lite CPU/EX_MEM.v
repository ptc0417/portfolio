module EX_MEM( ALU_in, ALU_out, RD2_in, RD2_out, WN_in, WN_out, MEM, MemWrite, MemRead, WB_in, WB_out, clk, rst );
	input [31:0] ALU_in, RD2_in;
	input [4:0] WN_in; 
	input clk, rst;
    input [1:0] MEM;
	input [1:0] WB_in;
	output [31:0] ALU_out, RD2_out;
	output [4:0] WN_out;
	output MemWrite, MemRead;
	output [1:0] WB_out;
	
	reg [31:0] ALU_out, RD2_out;
	reg [4:0] WN_out;
	reg MemWrite, MemRead;
	reg [1:0] WB_out;
	
	always @ ( posedge clk ) begin
		if ( rst ) begin
			ALU_out <= 32'b0;
			RD2_out <= 32'b0;
			WN_out <= 5'b0;
			WB_out <= 2'b0;
			MemWrite <= 1'b0;
			MemRead <= 1'b0;
		end
		else begin
			MemWrite <= MEM[1];
			MemRead <= MEM[0];
			ALU_out <= ALU_in;
			RD2_out <= RD2_in;
			WN_out <= WN_in;
			WB_out <= WB_in;
		end
	end
endmodule	