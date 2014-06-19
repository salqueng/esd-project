include "montecarlo.v";
include "rand.v";
include "present_val.v";
include "math.v";
include "sum_merge.v";
include "m_counter.v";
include "CLK_div_gen.v";


module MonteCarloTest;
	reg clk;
	reg nreset;
	wire [63:0] sum;
	wire [63:0] sum_square;
	wire signed [15:0] KerT = 6978;
	wire signed [15:0] Se05sigmaT = 4086;
	wire signed [15:0] sigmaSqrtT = 315;
	wire [31:0] M_count=2000000;
	reg Mode;
	wire Status;
	
	montecarlo carlo(clk, nreset, KerT,
		Se05sigmaT, sigmaSqrtT, Status, sum, sum_square, Mode);
	Counter counter(clk,nreset,M_count,Mode,Status);
	initial begin
		clk = 0;
		nreset = 0;
		#10 nreset = 1;
		#10 Mode = 1;
		
		#80001000 Mode = 0;
		#1000 Mode = 1;
		
	end
	
	always #20 clk = ~clk;
endmodule
