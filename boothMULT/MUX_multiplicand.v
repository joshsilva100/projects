`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/22/2024 12:00:05 PM
// Design Name: 
// Module Name: MUX_multiplicand
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


// Define a module for a multiplexer that selects one of five multiplicand values.
// The width of each multiplicand and the output is parameterizable through N.
module MUX_multiplicand #(parameter N = 8)(
    input [2:0] sel,         // 3-bit select input to choose which data line is output.
    input [N-1:0] x1,        // Multiplicand option +0 (no change).
    input [N-1:0] x2,        // Multiplicand option +1 (original value).
    input [N-1:0] x3,        // Multiplicand option -1 (two's complement or negated value).
    input [N-1:0] x4,        // Multiplicand option +2 (double the original value).
    input [N-1:0] x5,        // Multiplicand option -2 (negated double value).
    output reg [N-1:0] y     // Output of the multiplexer, selected based on `sel`.
    );
    
    // Combinational logic to select the output based on the select signal.
    always @(*) begin
        case(sel)
            3'b000: y = x1; // Selects +0 multiplicand, often used for no operation. 
            3'b001: y = x2; // Selects +1 multiplicand, equivalent to the original number. 
            //3'b011: y = ; 
            //3'b100: y = ;
            3'b101: y = x3; // Selects -1 multiplicand, which is the negated original number.
            3'b010: y = x4; // Selects +2 multiplicand, double the original number.
            3'b110: y = x5; // Selects -2 multiplicand, negated and double the original number. 
            //3'b111: y = ;
            // Note: Other case values like 3'b011, 3'b100, and 3'b111 are not covered.
            // Depending on the design, you may want to handle these undefined states. 
            default: y = x1;
        endcase    
    end
endmodule


