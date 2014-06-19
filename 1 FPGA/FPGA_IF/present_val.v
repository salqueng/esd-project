module PresentValue (input clk, input nreset,
	input signed [15:0] KerT, input signed [15:0] Se05sigmaT, input signed[15:0] sigmaSqrtT,
	input signed [15:0] grand, output [15:0] val);
	
	reg [31:0] exp_result;
	reg [31:0] Se05sigmaTxExpResult;

	reg [15:0] calculated;
	assign val = calculated;
	
	wire [15:0] wired_exp_result;
	reg signed [31:0] exponent;
	ExpFunction EXP (clk, nreset, exponent[15:0], wired_exp_result);
	
	always @(posedge clk or negedge nreset) begin
		if (nreset == 1'b0) begin
			exponent <= 0;
			exp_result <= 0;
			Se05sigmaTxExpResult <= 0;
		end else begin
			Se05sigmaTxExpResult <= Se05sigmaT * exp_result >>> 8;
			exp_result <= wired_exp_result;
			exponent <= sigmaSqrtT * grand >>> 8;
		end
	end
	always @(posedge clk or negedge nreset) begin
		if (nreset == 1'b0) begin
			calculated <= 16'b0;
		end else begin
			//put something haha
			if(KerT> Se05sigmaTxExpResult) begin
				calculated <= KerT-Se05sigmaTxExpResult;
			end else begin
				calculated <= 0;
			end
		end
	end
endmodule
