`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/22/2024 12:00:30 PM
// Design Name: 
// Module Name: adder
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


// Parameterizable N-bit adder module.
module adder #(parameter N = 8)(
    input cin,                    // Carry-in input: Adds an extra bit to the sum if set to 1.
    input [N-1:0] x0,              // First N-bit input operand.
    input [N-1:0] x1,              // Second N-bit input operand.
    output reg extend,            // Output flag for sign extension (carry-out or overflow indication).
    output reg [N-1:0] y          // N-bit output sum of the inputs.
);

    // Combinational logic block to compute the sum.
    always @(*) begin
        /*
        // If carry-in (cin) is high, add both operands and an additional 1.
        // Otherwise, just add the two operands.
        if(cin == 1)
            y = x0 + x1 + 1;  // Add with carry
        else
            y = x0 + x1;      // Regular addition
       */
       y = x0 + x1 + cin;
    end 

    // Combinational logic block to determine the need for sign extension.
    always @(*) begin
        // Check combinations of the most significant bits (MSBs) of inputs for sign extension.
        if (x0[N-1] == 1'b1 & x1[N-1] == 1'b1)
            extend = 1'b1;  // Both MSBs are 1, indicating potential overflow for positive numbers.
        else if (x0[N-1] == 1'b0 & x1[N-1] == 1'b0)
            extend = 1'b0;  // Both MSBs are 0, with no overflow for positive numbers.
        else if (x0[N-1] == 1'b1 & x1[N-1] == 1'b0)
            extend = y[N-1];  // Mixed MSBs, check MSB of result for overflow indication.
        else if (x0[N-1] == 1'b0 & x1[N-1] == 1'b1)
            extend = y[N-1];  // Mixed MSBs, check MSB of result for overflow indication.
        else
            extend = 1'b0;  // Default case, should logically never occur due to covered cases.
    end

endmodule

