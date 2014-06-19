module counter_demo (clk_1KHz, nreset, SET, START, data_input, data_output);
	input clk_1KHz, nreset;
	input SET, START;
	input [31:0] data_input;
	output [31:0] data_output;
		
	assign data_output = data;
	reg[31:0] data;
	
	integer cnt_1000;

	always @(posedge clk_1KHz or negedge nreset) begin
		if(nreset == 1'b0) begin
			data <= 32'b0;
			cnt_1000 <= 0;
		end else if(SET == 1'b1) begin
			data <= data_input;
			cnt_1000 <= 0;
		end else begin
			if(data != 32'b0) begin	
				if(START == 1'b1) begin
					if(cnt_1000==999) begin
						data <= data-1;
						cnt_1000 <= 0;
					end else begin
						cnt_1000 <= cnt_1000 + 1;
					end
				end else begin
					cnt_1000 <= 0;
				end
			end
		end
	end	
endmodule
