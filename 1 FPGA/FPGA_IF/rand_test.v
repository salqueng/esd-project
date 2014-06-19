include "rand.v";

module NormalGeneratorTest;
	reg clk;
	reg nreset;
	wire signed[10:0] out;
	NormalGenerator NG(clk, nreset, out);
	
	initial begin
		clk = 0;
		nreset = 0;
		#10 nreset = 1;
	end
	always #50 clk = ~clk;
endmodule
