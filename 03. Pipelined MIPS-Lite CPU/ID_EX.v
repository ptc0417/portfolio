module ID_EX( immed_in, immed_out, RD1_in, RD1_out, RD2_in, RD2_out, rt_in, rt_out, rd_in, rd_out, 
              MEM_in, MEM_out, EX, RegDst, ALUSrc, ALUOp, 
			  WB_in, WB_out, funct_in, funct_out, shamt_in, shamt_out, clk, rst );
	input [31:0] immed_in, RD1_in, RD2_in;
	input [4:0] rt_in, rd_in, shamt_in, EX;
	input [5:0] funct_in;
	input [1:0] MEM_in;
	input [1:0] WB_in;
	input clk, rst;
	output [31:0] immed_out, RD1_out, RD2_out;
	output [4:0] rt_out, rd_out, shamt_out;
	output [5:0] funct_out;
	output [1:0] MEM_out;
	output [2:0] ALUOp;
	output [1:0] WB_out;
	output RegDst, ALUSrc;
	
	reg [31:0] immed_out, RD1_out, RD2_out;
	reg [4:0] rt_out, rd_out, shamt_out;
	reg [5:0] funct_out;
	reg [2:0] MEM_out, ALUOp;
	reg [1:0] WB_out;
	reg RegDst, ALUSrc;
	
	always @ ( posedge clk ) begin
		if ( rst ) begin
			immed_out <= 32'b0;
			RD1_out <= 32'b0;
			RD2_out <= 32'b0;
			rt_out <= 5'b0;
			rd_out <= 5'b0;
			shamt_out <= 5'b0;
			funct_out <= 6'b0;
			MEM_out <= 4'b0;
			ALUOp <= 3'b0;
			WB_out <= 2'b0;
			ALUSrc <= 1'b0;
			RegDst <= 1'b0;
		end
		else begin
			immed_out <= immed_in;
			RD1_out <= RD1_in;
			RD2_out <= RD2_in;
			rt_out <= rt_in;
			rd_out <= rd_in;
			shamt_out <= shamt_in;
			funct_out <= funct_in;
			MEM_out <= MEM_in;
			WB_out <= WB_in;			
			ALUOp <= EX[2:0];
			RegDst <= EX[3];
			ALUSrc <= EX[4];
		end
	end
endmodule	