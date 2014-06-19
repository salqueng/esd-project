module(input fpga_ncs,output [15:0]data_out);
	assign data_out = {15'b0,fpga_ncs};
endmodule 