module Counter(input clk, input nreset, input [31:0] counter_input, output[31:0] Current_Count,
					input mode, output reg status);
	
	reg [31:0] count;
	assign Current_Count = count;
	
	always@ (posedge clk or negedge nreset) begin
		if (nreset == 1'b0) begin
			count <= 0;
		end else if (mode == 1'b0 ) begin
			count <= 0;
		end else begin
/*			
			if(count < 30000000 && mode == 1'b1) begin
				status <= 1;
				count <= count + 1;
			end else begin
				status <= 0;
			end

*/
			if(count < 30000000) begin
				status <= 0;
				count <= count + 1;
			end else begin
				status <= 1;
			end
		end
	end
	


endmodule
