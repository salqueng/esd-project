include "present_val.v";

module PresentValueTest;
	reg clk;
	reg nreset;
	wire [15:0] val;
	wire [15:0] KerT = 11017;
	wire [15:0] Se05sigmaT = 12820;
	wire [15:0] sigmaSqrtT = 110;
	wire signed[15:0] rand_compare = -120;
	wire [31:0] sum_out;
	reg signed [10:0] rand;
	
	PresentValue NG(clk, nreset, rand_compare,
		KerT, Se05sigmaT, sigmaSqrtT, rand, val,sum_out);
	
	initial begin
		clk = 0;
		nreset = 0;
		#10 nreset = 1;
		#40 rand = -107;
		#100 rand = -600;
		#100 rand = -500;
		#100 rand = -1000;
		#100 rand = -1024;
		#100 rand = 1024;
	end
	
	always #50 clk = ~clk;
endmodule
