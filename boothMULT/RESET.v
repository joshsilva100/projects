`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/22/2024 12:01:04 PM
// Design Name: 
// Module Name: RESET
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


// Module definition for a resettable register.
// Parameter N specifies the width of the register.
module RESET #(
	parameter N = 8  // Width of the register and the inputs/outputs.
)(
	input s,                // Reset signal: when high, output follows input; when low, output is cleared.
	input [N-1:0] x1,       // Data input: value to be loaded into the register when 's' is high.
	output reg [N-1:0] y    // Data output: the current value of the register.
);

    // The always block triggers on any change of inputs.
	always @(*) 
	begin
		if (s == 1'b1)
			y = x1;  // If reset signal 's' is high, load 'x1' into the output 'y'.
		else if (s == 1'b0)
			y = 0;   // If reset signal 's' is low, clear the output 'y'.
	end
	
endmodule



