module math(input clk, input nreset, input[15:0] data_in, 
				output [63:0] sum_out, output [63:0] sum_square_out,
				input mode, input status);
	reg [63:0] sum; 
	reg [63:0] sum_square;
	reg [31:0] square;
	assign sum_out = sum;
	assign sum_square_out = sum_square;
	
	always @(posedge clk or negedge nreset) begin
		if (nreset == 1'b0) begin
			sum <= 64'b0;
			sum_square <= 64'b0;
			square <= 64'b0;
		end else if(mode == 1'b0) begin
			sum <= 64'b0;
			square <= 64'b0;
			sum_square <= 64'b0;
		end else begin
			if(status == 1'b0 && mode == 1'b1) begin	
				sum <= sum + data_in;
				sum_square <= sum_square + data_in * data_in;
			end
		end
	end
endmodule

module ExpFunction(input clk, input nreset, input signed [15:0] original_exponent, output [15:0] exp_result);
	reg signed [31:0] exp_constant [0:4];
	
	reg signed [31:0] exponent [0:3];
	reg signed [31:0] exponent2 [1:3];
	reg signed [31:0] exponent3; 
	reg signed [31:0] exponent3_divided;
	
	reg signed [31:0] exp_shifted;
	reg signed [31:0] exp_calculated;

	assign exp_result = exp_calculated;
	
	always @(posedge clk or negedge nreset) begin
		if (nreset == 1'b0) begin
			exp_calculated <= 0;
			exp_constant[0] <= 1;
		end else begin
			if (original_exponent < -768) begin
				exponent[0] <= original_exponent + 1024;
				exp_constant[0] <= 1;
			end
			if (original_exponent >= -768 && original_exponent < -256) begin
				exponent[0] <= original_exponent + 512;
				exp_constant[0] <= 2;
			end
			if (original_exponent >= -256 && original_exponent < 256) begin
				exponent[0] <= original_exponent;
				exp_constant[0] <= 3;
			end
			if (original_exponent >= 256 && original_exponent < 768) begin
				exponent[0] <= original_exponent - 512;
				exp_constant[0] <= 4;
			end
			if (original_exponent >= 768) begin
				exponent[0] <= original_exponent - 1024;
				exp_constant[0] <= 5;
			end

			exp_constant[4] <= exp_constant[3];
			exp_constant[3] <= exp_constant[2];
			exp_constant[2] <= exp_constant[1];
			exp_constant[1] <= exp_constant[0];

			exponent[3] <= exponent[2];
			exponent[2] <= exponent[1];
			exponent[1] <= exponent[0];
			
			exponent2[3] <= exponent2[2];
			exponent2[2] <= exponent2[1];
			exponent2[1] <= (exponent[0] * exponent[0]) >>> 8;

			exponent3 <= (exponent2[1] * exponent[1]) >>> 8;
			exponent3_divided <= ((exponent3 <<< 7) + (exponent3 <<< 5) + (exponent3 <<< 3) + (exponent3 <<< 1)) >>> 10;
			exp_shifted <= 256 + exponent[3] + (exponent2[3] >>> 1) + exponent3_divided;
			case(exp_constant[4])
				1:exp_calculated <=((exp_shifted <<< 2) + exp_shifted ) >>> 8;
				2:exp_calculated <=((exp_shifted <<< 5) + (exp_shifted <<< 1) + exp_shifted) >>> 8;
				3:exp_calculated <= exp_shifted;
				4:exp_calculated <=((exp_shifted <<< 10) + (exp_shifted <<< 9) + (exp_shifted <<< 8)
										+ (exp_shifted <<< 6) + (exp_shifted <<< 5) + (exp_shifted <<< 2)) >>> 8;
				5:exp_calculated <=((exp_shifted <<< 13) + (exp_shifted <<< 12) + (exp_shifted <<< 10)
										+ (exp_shifted <<< 9) + (exp_shifted <<< 7) + (exp_shifted <<< 4)
										+ (exp_shifted <<< 3) + (exp_shifted <<< 1)) >>> 8;
				default : exp_calculated <= exp_shifted;
			endcase	
		end
	end
endmodule
