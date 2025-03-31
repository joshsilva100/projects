`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/22/2024 12:03:42 PM
// Design Name: 
// Module Name: btn_lab4
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



// Module definition for lab 4 button interface with parameterizable data width N.
module btn_lab4 #(
    parameter N = 4  // Define the data width for the system.
)(
    input clk,               // Clock input for synchronization.
    input rst,               // Global reset signal.
    input key,               // Input signal from a physical button.
    input start,             // Control signal to start the processor.
    input [N-1:0] multiplican,    // Data input port for the multiplicand.
    input [N-1:0] multiplier,       // Data input port for the multiplier.
    output [2*N-1:0] product,       // Output port for the product result.
    output Busy,                     // Processor's busy status output.
    output Done                      // Signal indicating completion of the operation.
);
    
    // Internal signals for debouncing logic.
    wire key_pulse;          // Pulse signal generated after key debounce.
    reg [7:0] key_buffer;    // Buffer to store states of the key for debouncing.
    reg key_reg;             // Register to hold the debounced key value.
    
    // Debounce logic for the key input, triggered on clock edge or reset.
    always @(posedge clk or negedge rst) begin
        if (!rst)
            key_reg <= 1'b0;  // Clear the key register on reset.
        else if (key == 1'b1)
            key_reg <= 1'b1;  // Set the key register if key input is high.
        else 
            key_reg <= 1'b0;  // Clear the key register if key input is low.
    end

integer i; 
    // Update key buffer on each clock cycle, used for edge detection.
    always @(posedge clk or negedge rst) begin
        if (!rst)
            key_buffer <= 8'b0;  // Reset the key buffer on reset.
        else begin
            // Sequentially fill the buffer based on the debounced key value.
            for (i = 0; i < 8; i = i + 1) begin
                if (key_reg == 1'b1)
                    key_buffer[i] <= 1'b1;  // Set buffer bits if key is pressed.
                else
                    key_buffer[i] <= 1'b0;  // Clear buffer bits if key is not pressed.
            end            
        end  
    end
    
    // Generate a single-cycle pulse when the key is pressed.
    // This is a simplification; typically, edge detection logic would be used.
    assign key_pulse = &key_buffer;  // AND all bits to generate a pulse.
    
    // Instantiation of the system_processor (or Bitpair_Mult) module 
    // with the debounced key_pulse signal as the clock input.
    Bitpair_Mult #(
        .N(N)  // Pass the parameter N to the instantiated module.
    ) dut (
        .clk(key_pulse),        // Use the debounced key signal as a clock for simulation purposes.
        .rst(rst),              // Pass the reset signal.
        .start(start),          // Pass the start control signal.
        .multiplican(multiplican), // Pass the multiplicand input.
        .multiplier(multiplier),   // Pass the multiplier input.
        .product(product),         // Connect to the product output.
        .busy(Busy),               // Connect to the busy status output.
        .done(Done)                // Connect to the done signal output.
    );
    
endmodule

