`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/22/2024 11:59:35 AM
// Design Name: 
// Module Name: shift_reg
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


module shift_reg #(
    parameter N = 8  // N is the number of bits in the shift register.
)(
    input clk,                     // Clock input: triggers the shifting or setting actions.
    input set,                     // Set control: if high, load 'din' into the register.
    input shift,                   // Shift control: if high, perform a right shift on the register content.
    input [1:0] shift_in,          // 2-bit input value to be shifted into the register from the left.
    input [N-1:0] din,             // Data input: value to load into the register when 'set' is high.
    output reg [N-1:0] dout        // Data output: the current value of the shift register.
);
    
    // Sequential logic block triggered by the rising edge of the clock.
    always @(posedge clk) begin
        if (set == 1'b1) begin
            // When 'set' is high, load 'din' into the register.
            dout <= din; 
        end 
        else if (shift == 1'b1) begin
            // When 'shift' is high, perform a right shift of the register content by two bits,
            // and insert 'shift_in' in the two bits on the leftmost side.
            dout <= {shift_in, dout[N-1:2]};  // Correct for a 2-bit right shift
        end
    end

endmodule



