include "rand.v";
include "present_val.v";

module AllTest;
	reg clk;
	reg nreset;
	wire signed[10:0] out;
	NormalGenerator NG(clk, nreset, out);
	
	wire [15:0] val;
	wire [15:0] KerT = 11017;
	wire [15:0] Se05sigmaT = 12820;
	wire [15:0] sigmaSqrtT = 110;
	wire signed[15:0] rand_compare = -120;
	PresentValue PV(clk, nreset, rand_compare,
		KerT, Se05sigmaT, sigmaSqrtT, out, val);
	initial begin
		clk = 0;
		nreset = 0;
		#10 nreset = 1;
	end
	always #60 clk = ~clk;
endmodule


