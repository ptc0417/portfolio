/*
	Title: MIPS Single Cycle CPU Testbench
	Author: Selene (Computer System and Architecture Lab, ICE, CYCU) 
*/
module tb_Pipelined();
	reg clk, rst;
	
	// 產生時脈，週期：10ns
	initial begin
		clk = 1;
		forever #5 clk = ~clk;
	end

	initial begin
		rst = 1'b1;
		/*
			指令資料記憶體，檔名"instr_mem.txt, data_mem.txt"可自行修改
			每一行為1 Byte資料，以兩個十六進位數字表示
			且為Little Endian編碼
		*/
		$readmemh("instr_mem.txt", CPU.InstrMem.mem_array );
		$readmemh("data_mem.txt", CPU.DatMem.mem_array );
		// 設定暫存器初始值，每一行為一筆暫存器資料
		$readmemh("reg.txt", CPU.RegFile.file_array );
		#10;
		rst = 1'b0;
	end
	
	always @( posedge clk ) begin
		$display( "%d, PC:", $time/10-1, CPU.pc );
		if ( CPU.opcode == 6'd0 ) begin
			if ( CPU.funct == 6'd32 ) $display( "%d, ADD\n", $time/10-1 );
			else if ( CPU.funct == 6'd34 ) $display( "%d, SUB\n", $time/10-1 );
			else if ( CPU.funct == 6'd36 ) $display( "%d, AND\n", $time/10-1 );
			else if ( CPU.funct == 6'd37 ) $display( "%d, OR\n", $time/10-1 );
			else if ( CPU.funct == 6'd2 ) $display( "%d, SRL\n", $time/10-1 );
			else if ( CPU.funct == 6'd42 ) $display( "%d, SLT\n", $time/10-1 );
			else if ( CPU.funct == 6'd8 ) $display( "%d, JR\n", $time/10-1 );
			else if ( CPU.funct == 6'd27 ) $display( "%d, DIVU\n", $time/10-1 );
			else if ( CPU.funct == 6'd16 ) $display( "%d, MFHI\n", $time/10-1 );
			else if ( CPU.funct == 6'd18 ) $display( "%d, MFLO\n", $time/10-1 );
			else if ( CPU.funct == 6'd0 ) $display( "%d, NOP\n", $time/10-1 );
		end
		else if ( CPU.opcode == 6'd35 ) $display( "%d, LW\n", $time/10-1 );
		else if ( CPU.opcode == 6'd43 ) $display( "%d, SW\n", $time/10-1 );
		else if ( CPU.opcode == 6'd4 ) $display( "%d, BEQ\n", $time/10-1 );
		else if ( CPU.opcode == 6'd2 ) $display( "%d, J\n", $time/10-1 );
		else if ( CPU.opcode == 6'd13 ) $display( "%d, ORI\n", $time/10-1 );
		
	end
	
	mips_pipelined CPU( clk, rst );
	
endmodule
