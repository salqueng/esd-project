module host_io (CLK, nCS, nOE, nWE, Hdo, Hdi, Hdata);
	input CLK, nCS, nOE, nWE;
	input [15:0] Hdo;
	output reg [15:0] Hdi;
	inout reg [15:0] Hdata;
	
	reg re_dly, re_dly1;
	
	always @(posedge CLK) begin: cdly
		re_dly <= nOE;
		re_dly1 <= re_dly;
	end
	
	always @(negedge nCS) begin: write
		Hdi <= Hdata;
	end
	
	always @(nCS or nOE or re_dly1 or Hdo) begin: read
		if (nCS == 1'b0) begin
			if (nOE == 1'b0 || re_dly1 == 1'b0) begin
				Hdata <= Hdo;
			end else begin
				Hdata <= {16{1'bz}};
			end
		end else begin
			Hdata <= {16{1'bz}};
		end
	end
endmodule
