//	Title: MIPS Single-Cycle Processor
//	Editor: Selene (Computer System and Architecture Lab, ICE, CYCU)


module mips_pipelined( clk, rst );
	input clk, rst;
	
	// instruction bus
	wire[31:0] instr, instr_out;
	
	// break out important fields from instruction
	wire [5:0] opcode, funct, funct_out;
        wire [4:0] rs, rt, rt_out, rd, rd_out, shamt, shamt_out;
        wire [15:0] immed;
        wire [31:0] extend_immed, extend_immed_un, immed_result, extend_out, b_offset;
        wire [25:0] jumpoffset;
		wire [63:0] DivAns;
	
	// datapath signals
        wire [4:0] rfile_wn, wn_1, wn_2;
        wire [31:0] rfile_rd1, rd1_out, rfile_rd2, rd2_out, rd2ToWD, rfile_wd, alu_b, alu_ans, alu_out,
				aluToADDR, ADDR_out, b_tgt, b_tgt_out, pc_next, HiOut, LoOut, pc_jump, 
                pc, pc_incr, pc_add, dmem_rdata, dmem_rdata_out, jump_addr, branch_addr;

	// control signals
        wire RegWrite, Branch, PCSrc, RegDst, MemtoReg, MemRead, MemWrite, ALUSrc, Zero, Jump, Divu, ExtendSel, JumpReg;
        wire [1:0] ALU_Out_Sel;
        wire [2:0] ALUOp, Operation;
	
		wire [1:0] WB_reg, WB_reg_1, WB_reg_2, WB_reg_3;
		wire [1:0] MEM_reg, MEM_reg_1, MEM_reg_2;
		wire [4:0] EX_reg, EX_reg_1;
	
        assign opcode = instr_out[31:26];
        assign rs = instr_out[25:21];
        assign rt = instr_out[20:16];
        assign rd = instr_out[15:11];
        assign shamt = instr_out[10:6];
        assign funct = instr_out[5:0];
        assign immed = instr_out[15:0];
        assign jumpoffset = instr_out[25:0];
	
	// branch offset shifter
        assign b_offset = extend_immed << 2;
	
	// jump offset shifter & concatenation
	assign jump_addr = { pc_incr[31:28], jumpoffset <<2 };
	
	// module instantiations
	// Insturction Fetch 
	
	reg32 PC( .clk(clk), .rst(rst), .en_reg(1'b1), .d_in(pc_next), .d_out(pc) );  
	
	memory InstrMem( .clk(clk), .MemRead(1'b1), .MemWrite(1'b0), .wd(32'd0), .addr(pc), .rd(instr) );
	
	add32 PCADD( .a(pc), .b(32'd4), .result(pc_incr) );
	
	mux2 #(32) PCMUX( .sel(PCSrc), .a(pc_incr), .b(b_tgt), .y(branch_addr) );

	mux2 #(32) JMUX( .sel(Jump), .a(branch_addr), .b(jump_addr), .y(pc_jump) );	
	
	mux2 #(32) JRMUX( .sel(JumpReg), .a(pc_jump), .b(rd1_out), .y(pc_next) );
	
	IF_ID Reg1( .RD_in(instr), .RD_out(instr_out), .pc_in(pc_incr), .pc_out(pc_add), .clk(clk), .rst(rst) ); 
	
	// Instruction Decode
	
	reg_file RegFile( .clk(clk), .RegWrite(RegWrite), .RN1(rs), .RN2(rt), .WN(wn_2), .WD(rfile_wd), .RD1(rfile_rd1), .RD2(rfile_rd2) );
	
	sign_extend SignExt( .immed_in(immed), .ext_immed_out(extend_immed) );
	
	unsign_extend UnsignExt( .immed_in(immed), .ext_immed_out(extend_immed_un) );
	
	mux2 #(32) EXTENDMUX( .sel(ExtendSel), .a(extend_immed), .b(extend_immed_un), .y(immed_result) );
	
	control_pipelined CTL(.opcode(opcode), .Branch(Branch), .Jump(Jump), .ExtendSel(ExtendSel), .EX(EX_reg), .MEM(MEM_reg), .WB(WB_reg) );
	
	branch_equ equ( .opcode(opcode), .zero(Zero), .a(rfile_rd1), .b(rfile_rd2) );
	
	add32 BRADD( .a(pc_add), .b(b_offset), .result(b_tgt) );
	
	and BR_AND(PCSrc, Branch, Zero);
	
	ID_EX Reg2( .immed_in(immed_result), .immed_out(extend_out), .RD1_in(rfile_rd1), .RD1_out(rd1_out), .RD2_in(rfile_rd2), .RD2_out(rd2_out), 
	            .rt_in(rt), .rt_out(rt_out), .rd_in(rd), .rd_out(rd_out), .MEM_in(MEM_reg), .MEM_out(MEM_reg_1), .EX(EX_reg), 
				.RegDst(RegDst), .ALUSrc(ALUSrc), .ALUOp(ALUOp), .WB_in(WB_reg), .WB_out(WB_reg_1), 
				.funct_in(funct), .funct_out(funct_out), .shamt_in(shamt), .shamt_out(shamt_out), .clk(clk), .rst(rst) );
				
	// Execute/Address Calculation		

	alu_ctl ALUCTL( .ALUOp(ALUOp), .Funct(funct_out), .ALUOperation(Operation), .Divu(Divu), .sel(ALU_Out_Sel), .JumpReg(JumpReg));
	
	mux2 #(32) ALUMUX( .sel(ALUSrc), .a(rd2_out), .b(extend_out), .y(alu_b) );
	
	alu ALU( .ctl(Operation), .a(rd1_out), .b(alu_b), .shamt(shamt_out), .result(alu_ans) );
	
	divider Divider( .clk(clk), .dataA(rd1_out), .dataB(rd2_out), .dataOut(DivAns), .Signal(Divu), .reset(reset) );
	
	HiLo HiLo( .clk(clk), .DivAns(DivAns), .HiOut(HiOut), .LoOut(LoOut), .reset(reset) );	
	
	mux3 #(32) OUTMUX( .sel(ALU_Out_Sel), .a(alu_ans), .b(HiOut), .c(LoOut), .y(alu_out) );
	
	mux2 #(5) RFMUX( .sel(RegDst), .a(rt_out), .b(rd_out), .y(rfile_wn) );
	
	EX_MEM Reg3( .MEM(MEM_reg_1), .MemWrite(MemWrite), .MemRead(MemRead), .RD2_in(rd2_out), .RD2_out(rd2ToWD), .WN_in(rfile_wn), .WN_out(wn_1), 
	             .ALU_in(alu_out), .ALU_out(aluToADDR), .WB_in(WB_reg_1), .WB_out(WB_reg_2), .clk(clk), .rst(rst) );
				 
	// Memory Access
	
	memory DatMem( .clk(clk), .MemRead(MemRead), .MemWrite(MemWrite), .wd(rd2ToWD), .addr(aluToADDR), .rd(dmem_rdata) );	 			 				  

	MEM_WB Reg4( .RD_in(dmem_rdata), .RD_out(dmem_rdata_out), .ALU_in(aluToADDR), .ALU_out(ADDR_out), 
	             .WN_in(wn_1), .WN_out(wn_2), .WB(WB_reg_2), .MemtoReg(MemtoReg), .RegWrite(RegWrite), .clk(clk), .rst(rst) );
				 
	// Write Back			 
				 
	mux2 #(32) WRMUX( .sel(MemtoReg), .a(ADDR_out), .b(dmem_rdata_out), .y(rfile_wd) );			 
	
endmodule
