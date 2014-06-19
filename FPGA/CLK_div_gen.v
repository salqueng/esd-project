module CLK_div_gen (CLK_50MHz, CLK_1Hz, CLK_1MHz, CLK_1KHz, CLK_3KHz, nreset);
	input CLK_50MHz, nreset;
	output CLK_1Hz, CLK_1MHz, CLK_1KHz, CLK_3KHz;
	
	
	integer CNT_1MHz;
	reg BCLK_1MHz;
	
	integer CNT_1KHz;
	reg BCLK_1KHz;
	
	integer CNT_3KHz;
	reg BCLK_3KHz;
	
	integer CNT_1Hz;
	reg BCLK_1Hz;
	

	always @(posedge CLK_50MHz or negedge nreset) begin
		if (nreset == 1'b0) begin
			BCLK_1MHz <= 1'b0;
			CNT_1MHz <= 0;
		end else begin
			if (CNT_1MHz == 24) begin
				CNT_1MHz <= 0;
				BCLK_1MHz <= ~BCLK_1MHz;
			end else begin
				CNT_1MHz <= CNT_1MHz+1'b1;
			end
		end
	end
	
	assign CLK_1MHz = BCLK_1MHz;
	
	always @(posedge CLK_50MHz or negedge nreset) begin
		if (nreset == 1'b0) begin
			BCLK_1KHz <= 1'b0;
			CNT_1KHz <= 0;
		end else begin
			if (CNT_1KHz == 24999) begin
				CNT_1KHz <= 0;
				BCLK_1KHz <= ~BCLK_1KHz;
			end else begin
				CNT_1KHz <= CNT_1KHz+1'b1;
			end
		end
	end
	
	assign CLK_1KHz = BCLK_1KHz;
	
	always @(posedge CLK_50MHz or negedge nreset) begin
		if (nreset == 1'b0) begin
			BCLK_3KHz <= 1'b0;
			CNT_3KHz <= 0;
		end else begin
			if (CNT_3KHz == 4167) begin
				CNT_3KHz <= 0;
				BCLK_3KHz <= ~BCLK_3KHz;
			end else begin
				CNT_3KHz <= CNT_3KHz+1'b1;
			end
		end
	end
	
	assign CLK_3KHz = BCLK_3KHz;
	
	always @(posedge CLK_50MHz or negedge nreset) begin
		if (nreset == 1'b0) begin
			BCLK_1Hz <= 1'b0;
			CNT_1Hz <= 0;
		end else begin
			if (CNT_1Hz == 24999999) begin
				CNT_1Hz <= 0;
				BCLK_1Hz <= ~BCLK_1Hz;
			end else begin
				CNT_1Hz <= CNT_1Hz+1'b1;
			end
		end
	end
	
	assign CLK_1Hz = BCLK_1Hz;
	
endmodule
