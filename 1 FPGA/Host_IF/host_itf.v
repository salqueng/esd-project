module host_itf (
	clk, nRESET, FPGA_nRST, HOST_nOE, HOST_nWE, HOST_nCS, 
	HOST_ADD, HDI, SUM_OUTPUT, SUM_SQUARE_OUTPUT, HDO, KerT, Se05sigmaT,sigmaSqrtT, M_count, Mode, Status, Current_Count);
	
	input clk, nRESET, FPGA_nRST, HOST_nOE, HOST_nWE, HOST_nCS;
	input [20:0] HOST_ADD;
	input [15:0] HDI;
	output reg [15:0] HDO;

	input [63:0] SUM_OUTPUT;
	input [63:0] SUM_SQUARE_OUTPUT;
	input Status;
	input [31:0] Current_Count;
	
	output [15:0] KerT;
	output [15:0] Se05sigmaT;
	output [15:0] sigmaSqrtT;
	output [31:0] M_count;
	output Mode;
	
	reg [15:0] x8800_0100, x8800_0102, x8800_0104, x8800_0106, x8800_0108,
				  x8800_010a;
				 
	wire [15:0] x8800_010e, x8800_0110, x8800_0112, x8800_0114,
				   x8800_0116, x8800_0118, x8800_011a, x8800_011c,
					x8800_011e, x8800_0120, x8800_0122;
					
	assign KerT = x8800_0100;
	assign sigmaSqrtT = x8800_0102;
	assign Se05sigmaT = x8800_0104;
	assign M_count = {x8800_0106, x8800_0108};
	assign Mode = x8800_010a[0];
	
	
	assign x8800_010e = SUM_OUTPUT[15:0];
	assign x8800_0110 = SUM_OUTPUT[31:16];
	assign x8800_0112 = SUM_OUTPUT[47:32];
	assign x8800_0114 = SUM_OUTPUT[63:48];
	
	assign x8800_0116 = SUM_SQUARE_OUTPUT[15:0];
	assign x8800_0118 = SUM_SQUARE_OUTPUT[31:16];
	assign x8800_011a = SUM_SQUARE_OUTPUT[47:32];
	assign x8800_011c = SUM_SQUARE_OUTPUT[63:48];
	
	assign x8800_0120 = Current_Count[15:0];
	assign x8800_0122 = Current_Count[31:16];
	assign x8800_011e = {15'b0,Status};

	
	always @(posedge clk or negedge nRESET) begin
		if (nRESET == 1'b0) begin
			HDO <= 16'b0;
		end else begin
			if (HOST_nCS == 1'b0 && HOST_nOE == 1'b0) begin
				case (HOST_ADD[19:0])
					20'h0010e: HDO <= x8800_010e;
					20'h00110: HDO <= x8800_0110;
					20'h00112: HDO <= x8800_0112;
					20'h00114: HDO <= x8800_0114;
					20'h00116: HDO <= x8800_0116;
					20'h00118: HDO <= x8800_0118;
					20'h0011a: HDO <= x8800_011a;
					20'h0011c: HDO <= x8800_011c;
					20'h0011e: HDO <= x8800_011e;
					20'h00120: HDO <= x8800_0120;
					20'h00122: HDO <= x8800_0122;
					20'h00100: HDO <= x8800_0100;
					20'h00102: HDO <= x8800_0102;
					20'h00104: HDO <= x8800_0104;
					20'h00106: HDO <= x8800_0106;
					20'h00108: HDO <= x8800_0108;
					20'h0010a: HDO <= x8800_010a;
				endcase
			end
		end
	end
	
	always @(posedge clk or negedge nRESET) begin
		if (nRESET == 1'b0) begin
			x8800_0100 <= 16'b0;
			x8800_0102 <= 16'b0;
			x8800_0104 <= 16'b0;
			x8800_0106 <= 16'b0;
			x8800_0108 <= 16'b0;
			x8800_010a <= 16'b0;
		end else begin
			if (HOST_nCS == 1'b0 && HOST_nWE == 1'b0) begin
				case (HOST_ADD[19:0])
					20'h00100: x8800_0100 <= HDI;
					20'h00102: x8800_0102 <= HDI;
					20'h00104: x8800_0104 <= HDI;
					20'h00106: x8800_0106 <= HDI;
					20'h00108: x8800_0108 <= HDI;
					20'h0010a: x8800_010a <= HDI;

				endcase
			end 
		end
	end
	

	
endmodule

