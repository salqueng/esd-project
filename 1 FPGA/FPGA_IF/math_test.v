include "math.v";
include "mathng.v";
include "rand.v";
include "present_val.v";

module MathTest;
	reg clk;
	reg nreset;
	wire signed [15:0] rand;
	NormalGenerator NG(clk,nreset,rand);
	wire signed[15:0] val;
	wire [15:0] KerT = 11017;
	wire [15:0] Se05sigmaT = 12820;
	wire [15:0] sigmaSqrtT = 110;
	PresentValue PV(clk, nreset,
		KerT, Se05sigmaT, sigmaSqrtT, rand, val);
	wire signed [15:0] mean;
	wire signed [31:0] var;
	math MT(clk, nreset, val, mean, var);
	initial begin
		clk = 0;
		nreset = 0;
		#10 nreset = 1;
	end
	always #50 clk = ~clk;
endmodule

module ExpTest;
	reg clk;
	reg nreset;
	wire signed [15:0] rand;
	wire [15:0] exp;
	NormalGenerator NG(clk,nreset,rand);
	ExpFunction EXP(clk, nreset, rand, exp);
	initial begin
		clk = 0;
		nreset = 0;
		#10 nreset = 1;
	end
	always #50 clk = ~clk;
endmodule
