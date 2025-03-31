`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/22/2024 12:01:52 PM
// Design Name: 
// Module Name: Counter
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


// Define a module for a countdown counter with a parameterizable width N.
module Counter #(
    parameter N = 8  // N defines the total count range; actual counter width is N/2.
)(
    input start,          // Start signal: when high, resets or initializes the counter.
    input tick,           // Tick signal: serves as a clock input, driving the counter's decrement.
    output done           // Done signal: goes high when the counter reaches zero.
);

    // Internal counter register with a width of N/2 bits.
    // The counter size is halved presumably to fit specific design constraints or requirements.
    reg [N/2-1:0] count;
    
    // Sequential logic block triggered by the rising edge of 'tick' or 'start'.
    // This block is responsible for resetting the counter on 'start' and decrementing it on 'tick'.
    always @(posedge tick or posedge start) begin
        if (start)
            count <= -1; // Reset the counter to its maximum value (all bits set due to -1 in two's complement).
        else 
            count <= {1'b0, count[N/2-1:1]}; // Perform a right shift, effectively decrementing the counter.
    end
    
    // The 'done' signal is assigned the value of the least significant bit of the counter.
    // When the counter reaches zero, 'count[0]' would be 0, indicating the countdown is not yet complete.
    // There seems to be a logical discrepancy here: typically, 'done' might be expected to signal completion.
    // You may need to revise this based on your intended counter behavior.
    assign done = count[0];

endmodule

