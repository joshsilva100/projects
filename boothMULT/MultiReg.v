`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/22/2024 12:01:25 PM
// Design Name: 
// Module Name: MultiReg
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


// Define a module for a multi-functional register with parameterizable width N.
module MultiReg #(
    parameter N = 8  // N is the number of bits in the register.
)(
    input clk,                    // Clock input: triggers the operations within the register.
    input set,                    // Set control: if high, load 'din' into the register.
    input shift,                  // Shift control: if high, perform a shift operation on the register content.
    input [1:0] shift_in,         // 2-bit input value to be shifted into the register from the left.
    input [N-1:0] din,            // Data input: value to load into the register when 'set' is high.
    output reg [N-1:0] dout       // Data output: the current value of the register.
);
    
    // Sequential logic block triggered by the rising edge of the clock.
    always @(posedge clk) begin
        if (shift == 1'b1) begin
            // When 'shift' is high, perform a shift operation.
            // The content of 'dout' is updated to include the 2-bit 'shift_in' value at the left
            // and the existing 'din' data shifted right by two positions.
            dout <= {shift_in, din[N-1:2]};
        end 
        else if (set == 1'b1) begin
            // When 'set' is high, load the 'din' value directly into 'dout'.
            // This operation overrides the shifting behavior if both 'shift' and 'set' are high.
            dout <= din;  
        end
    end

endmodule

