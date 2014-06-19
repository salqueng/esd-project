module MathNG(input clk, input nreset, input signed [10:0] rand_ng, output signed [10:0] mean_ng, output signed [21:0] var_ng);

	reg signed [39:0] sum;
	reg signed [79:0] sum_square;
	integer cnt;
	assign mean_ng = sum / cnt ;
	assign var_ng = sum_square / cnt - sum * sum / cnt / cnt ;
	
	always @(posedge clk or negedge nreset) begin
		if (nreset == 1'b0) begin
			sum <= 40'b0;
			sum_square <= 80'b0;
			cnt <= -4 ;
		end else begin
			sum <= sum + rand_ng;
			sum_square <= sum_square + rand_ng * rand_ng;
			cnt <= cnt + 1;
		end
	end
endmodule