`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/22/2024 12:02:46 PM
// Design Name: 
// Module Name: MUX_recoded
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


// Module for recoding multiplier data according to Booth's encoding.
// This is typically used to reduce the number of operations in binary multiplication.
module MUX_recoded(
    input [2:0] mul_data,       // 3-bit input representing a part of the multiplier.
    output reg [2:0] recoded_data // 3-bit output representing the recoded multiplier data.
);

    // Combinational logic to recode the input data based on typical Booth's algorithm patterns.
    always @(*) begin
        case (mul_data)
            // No change needed: Represents multiplication by 0.
            3'b000: recoded_data = 3'b000; // +0

            // Standard positive multipliers: Represents multiplication by +1.
            3'b001: recoded_data = 3'b001; // +1
            3'b010: recoded_data = 3'b001; // +1 (redundant, but aligns with Booth for certain implementations)

            // Special case for handling +2 (not typically standard Booth but may represent a double operation).
            3'b011: recoded_data = 3'b010; // +2

            // Negative multipliers: Represents multiplication by -1.
            3'b100: recoded_data = 3'b110; // -2 (In two's complement, '110' represents -2)
            3'b101: recoded_data = 3'b101; // -1
            3'b110: recoded_data = 3'b101; // -1 (redundant, but aligns with Booth for certain implementations)

            // Represents a return to zero or no operation needed at the end of a cycle.
            3'b111: recoded_data = 3'b000; // +0
            
            // Default cases can be added if required, especially for handling unexpected inputs.
            default: recoded_data <= 3'bxxx; // Undefined or X state for illegal inputs.
        endcase    
    end
endmodule

