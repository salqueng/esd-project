module sum_merge(input clk, input nreset, 
					  input [63:0] sum_0, input [63:0] sum_1, input [63:0] sum_2, 
					  input [63:0] sum_3, input [63:0] sum_4, output reg[63:0] sum_out,
					  input [63:0] sum_square_0, input [63:0] sum_square_1, input [63:0] sum_square_2, 
					  input [63:0] sum_square_3, input [63:0] sum_square_4, output reg[63:0] sum_square_out);
	reg [63:0] sum_temp_1, sum_temp_2, sum_square_temp_1, sum_square_temp_2;
	always @(posedge clk or negedge nreset) begin
		if (nreset == 1'b0) begin
			sum_out <= 0;
			sum_square_out <= 0;
		end else begin
			sum_temp_1 <= sum_0 + sum_1;
			sum_temp_2 <= sum_2 + sum_3 + sum_4;
			sum_out <= sum_temp_1 + sum_temp_2;
			sum_square_temp_1 <= sum_square_0 + sum_square_1;
			sum_square_temp_2 <= sum_square_2 + sum_square_3 + sum_square_4;
			sum_square_out <= sum_square_temp_1 + sum_square_temp_2;
		end
	end
endmodule