module seg_demo (clk, nreset, data_in, seg_com, seg_disp);
	input clk, nreset;
	input [15:0] data_in;
	output reg [5:0] seg_com;
	output reg [7:0] seg_disp;
	reg [2:0] cnt_segcon;
	
	always @(posedge clk or negedge nreset) begin
		if (nreset == 1'b0) begin
			seg_com <= 0;
			seg_disp <= 8'b11111111;
		end else begin
			if (cnt_segcon == 5) cnt_segcon <= 0;
			else                 cnt_segcon <= cnt_segcon+1'b1;
			
			case (cnt_segcon)
				3'd0:   begin seg_com <= 6'b011111; seg_disp <= {conv_int(data_in%10), 1'b0}; end
				3'd1:   begin seg_com <= 6'b101111; seg_disp <= {conv_int(data_in/10%10), 1'b0}; end
				3'd2:   begin seg_com <= 6'b110111; seg_disp <= {conv_int(data_in/100%10), 1'b0}; end
				3'd3:   begin seg_com <= 6'b111011; seg_disp <= {conv_int(data_in/1000%10), 1'b0}; end
				3'd4:   begin seg_com <= 6'b111101; seg_disp <= {conv_int(data_in/10000%10), 1'b0}; end
				3'd5:   begin seg_com <= 6'b111110; seg_disp <= {conv_int(data_in/100000), 1'b0}; end
				default begin seg_com <= 6'b111111; seg_disp <= 8'b11111111; end
			endcase
		end
	end
				
	
	function [6:0] conv_int;
		input [3:0] cnt;
		begin
			case (cnt)
				4'd0:   conv_int = 7'b1111110;
				4'd1:   conv_int = 7'b0110000;
				4'd2:   conv_int = 7'b1101101;
				4'd3:   conv_int = 7'b1111001;
				4'd4:   conv_int = 7'b0110011;
				4'd5:   conv_int = 7'b1011011;
				4'd6:   conv_int = 7'b1011111;
				4'd7:   conv_int = 7'b1110000;
				4'd8:   conv_int = 7'b1111111;
				4'd9:   conv_int = 7'b1111011;
				default conv_int = 7'b0000000;
			endcase
		end
	endfunction
	
	function [7:0] conv_lcd;
		input [3:0] cnt;
		begin
			case (cnt)
				4'd0:   conv_lcd = 8'b00110000;
				4'd1:   conv_lcd = 8'b00110001;
				4'd2:   conv_lcd = 8'b00110010;
				4'd3:   conv_lcd = 8'b00110011;
				4'd4:   conv_lcd = 8'b00110100;
				4'd5:   conv_lcd = 8'b00110101;
				4'd6:   conv_lcd = 8'b00110110;
				4'd7:   conv_lcd = 8'b00110111;
				4'd8:   conv_lcd = 8'b00111000;
				4'd9:   conv_lcd = 8'b00111001;
				default conv_lcd = 8'b00000000;
			endcase
		end
	endfunction
endmodule
