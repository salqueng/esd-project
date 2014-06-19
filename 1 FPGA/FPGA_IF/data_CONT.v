module data_CONT (
	CLK, V_SEL,
	H_SEG_COM, H_SEG_DATA,
	F_SEG_COM, F_SEG_DATA,
	SEG_COM,   SEG_DATA);
	 
	input CLK, V_SEL;

	input [5:0] H_SEG_COM,  F_SEG_COM;
	input [7:0] H_SEG_DATA, F_SEG_DATA;
	
	output reg [5:0] SEG_COM;
	output reg [7:0] SEG_DATA;

	
	always @(*) begin
		if (V_SEL == 1'b0) begin
			SEG_COM  <= F_SEG_COM;
			SEG_DATA <= F_SEG_DATA;
		end else begin
			SEG_COM  <= H_SEG_COM;
			SEG_DATA <= H_SEG_DATA;
		end
	end
endmodule
