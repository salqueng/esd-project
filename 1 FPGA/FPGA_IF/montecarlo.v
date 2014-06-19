module montecarlo(input clk, input nreset, 
						input signed[15:0] KerT, input signed[15:0] Se05sigmaT, input signed[15:0] sigmaSqrtT,
						input Status, output [63:0] sum_out, output [63:0] sum_square_out,
						input Mode);
	//clk part
	wire CLK_1Hz, CLK_1MHz, CLK_1KHz, CLK_3KHz;
	CLK_div_gen clk_block(clk, CLK_1Hz, CLK_1MHz, CLK_1KHz, CLK_3KHz, nreset);
	

	//generator & math block
	wire signed[15:0] normal_0, normal_1, normal_2, normal_3, normal_4;
	wire [15:0] present_val_0, present_val_1, present_val_2, present_val_3, present_val_4;
	wire [63:0] sum_0, sum_1, sum_2, sum_3, sum_4;
	wire [63:0] sum_square_0, sum_square_1, sum_square_2, sum_square_3, sum_square_4;
	
	defparam ng_block_0.seed = 12'b100011011101;
	NormalGenerator ng_block_0(clk, nreset, normal_0);
	PresentValue pv_block_0(clk, nreset, KerT, Se05sigmaT, sigmaSqrtT, normal_0, present_val_0);
	math math_0(clk, nreset, present_val_0, sum_0, sum_square_0, Mode, Status);					
	
	defparam ng_block_1.seed = 12'b100011011111;
	NormalGenerator ng_block_1(clk, nreset, normal_1);
	PresentValue pv_block_1(clk, nreset, KerT, Se05sigmaT, sigmaSqrtT, normal_1, present_val_1);
	math math_1(clk, nreset, present_val_1, sum_1, sum_square_1, Mode, Status);
	
	defparam ng_block_2.seed = 12'b100011011110;
	NormalGenerator ng_block_2(clk, nreset, normal_2);
	PresentValue pv_block_2(clk, nreset, KerT, Se05sigmaT, sigmaSqrtT, normal_2, present_val_2);
	math math_2(clk, nreset, present_val_2, sum_2, sum_square_2, Mode, Status);
	
	defparam ng_block_3.seed = 12'b100011011100;
	NormalGenerator ng_block_3(clk, nreset, normal_3);
	PresentValue pv_block_3(clk, nreset, KerT, Se05sigmaT, sigmaSqrtT, normal_3, present_val_3);
	math math_3(clk, nreset, present_val_3, sum_3, sum_square_3, Mode, Status);
	
	defparam ng_block_4.seed = 10'b100011011001;
	NormalGenerator ng_block_4(clk, nreset, normal_4);
	PresentValue pv_block_4(clk, nreset, KerT, Se05sigmaT, sigmaSqrtT, normal_4, present_val_4);
	math math_4(clk, nreset, present_val_4, sum_4, sum_square_4, Mode, Status);

	sum_merge SUM_MERGE(clk, nreset, 
							  sum_0, sum_1, sum_2, sum_3, sum_4, sum_out,
							  sum_square_0, sum_square_1, sum_square_2, sum_square_3, sum_square_4, sum_square_out);
endmodule