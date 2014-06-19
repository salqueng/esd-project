module NormalGenerator (input clk, input nreset, output reg signed [15:0] out);
	parameter seed = 0;
	reg [11:0] lfsr;
	reg [11:0] temp_lfsr [0:6];
	reg [14:0] sum;
	
	wire signed [15:0] temp_out = {{7{~sum[14]}}, sum[13:5]};

	//12 bit lfsr 
	always @(posedge clk or negedge nreset) begin
		if (nreset == 1'b0) begin
			temp_lfsr[6] <= seed;
			temp_lfsr[5] <= seed;
			temp_lfsr[4] <= seed;
			temp_lfsr[3] <= seed;
			temp_lfsr[2] <= seed;
			temp_lfsr[1] <= seed;
			temp_lfsr[0] <= seed;
			lfsr <= seed;
		end else begin
			temp_lfsr[6] <= temp_lfsr[5];
			temp_lfsr[5] <= temp_lfsr[4];
			temp_lfsr[4] <= temp_lfsr[3];
			temp_lfsr[3] <= temp_lfsr[2];
			temp_lfsr[2] <= temp_lfsr[1];
			temp_lfsr[1] <= temp_lfsr[0];
			temp_lfsr[0] <= lfsr;
			lfsr <= {lfsr[10:0], lfsr[11] ^ lfsr[10] ^ lfsr[7] ^ lfsr[5]};
		end
	end
	
	//sum
	always @(posedge clk or negedge nreset) begin
		if (nreset == 1'b0) begin
			sum <= {seed, 3'b000};
			out <= 0; 
		end else begin
			sum <= sum - temp_lfsr[6] + lfsr;
			out <= (temp_out <<< 1) - (temp_out >>> 2) - (temp_out >>> 4) - (temp_out >>> 9);
		end
	end
endmodule
