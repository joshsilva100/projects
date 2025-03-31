`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/22/2024 12:03:10 PM
// Design Name: 
// Module Name: Bitpair_Mult
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


// Parameterized module for bit-pair multiplication, typically used in DSPs or ALUs.
module Bitpair_Mult #(
    parameter N = 4  // Specifies the bit width for the multiplicand and multiplier.
)(
    input clk,                         // Clock input for synchronization.
    input rst,                         // Asynchronous reset signal.
    input start,                       // Start signal to initiate the multiplication process.
    input [N-1:0] multiplican,         // Input multiplicand.
    input [N-1:0] multiplier,          // Input multiplier.
    output [2*N-1:0] product,          // Result of the multiplication.
    output busy,                       // Flag indicating the module is currently processing.
    output done                        // Flag indicating completion of multiplication.
);

    // Control signals for internal operations.
    wire load, shift, add, count, tick, LorA, sign_extend;
    wire [2:0] code;                   // Encoded value for multiplication steps.

    // Intermediate values and registers.
    wire [N:0] product_1, product_0;   // Extended product registers for accumulation.
    wire [N-1:0] multiplican_save;     // Register to store the multiplicand.
    wire [N:0] multi_multiplican;      // Result of selected operation on multiplicand.
    wire [N:0] partial;                // Partial sum/product for each step.
    wire [N:0] reset;                  // Reset value for the adder block.
    wire [N-1:0] data;                 // Additional data, possibly for future use or debugging. 

    // Register to store and potentially modify the multiplicand.
    shift_reg #(
        .N(N)
    ) multiplican_Reg (
        .clk(clk),
        .set(load),
        .shift(),              // Not used here as multiplicand does not need shifting.
        .shift_in(),           // No input needed as there's no shift operation.
        .din(multiplican),
        .dout(multiplican_save)
    );
    
    // Register to store and shift the multiplier, integrating partial products.
    shift_reg #(
        .N(N+1)
    ) multiplier_Reg (
        .clk(clk),
        .set(load),
        .shift(shift),
        .shift_in(product_1[1:0]),  //Feedback lower bits of product for Booth encoding.
        .din({multiplier,1'b0}),    // Initialize with multiplier and an added zero bit.
        .dout(product_0)
    );	

    // Compute complementary version of the multiplicand for subtraction. 
    wire [N:0] x3;
    assign x3 = (multiplican_save[N-1] == 1'b1) ? {1'b0, ~multiplican_save} : {1'b1, ~multiplican_save};	

    // Multiplexing different operation results based on Booth encoding.
    MUX_multiplicand #(
        .N(N+1)
    ) multi_multi (
        .sel(code),                           // Selection based on Booth recoding.
        .x1(0),                               // No operation (mult by 0).
        .x2({1'b0, multiplican_save}),        // Addition (mult by +1).
        .x3(x3),                              // Subtraction (mult by -1).
        .x4({multiplican_save, 1'b0}),        // Double (mult by +2).
        .x5({~multiplican_save, 1'b1}),       // Negative double (mult by -2).
        .y(multi_multiplican)                 // Output selected operation result.
    );
    
    // Adder for accumulating the multiplication results.
adder #(
		.N(N + 1)
	) adder_inst (
		.cin(code[2]),//+1
		.extend(sign_extend),
		.x0(multi_multiplican),
		.x1(product_1),
		.y(partial)
	);
    // Reset block to clear or set the partial sum/product.
    RESET #(
        .N(N+1)
    ) Reset_blk (
        .s(!load),
        .x1(partial),
        .y(reset)
    );

    // Logic to determine load or add operation.
    assign LorA = load | add;
    
    // Register block used for updating and shifting the partial product.
    MultiReg #(
        .N(N+1)  // Parameter for bit width, extended by one for overflow.
    ) product_Reg (
        .clk(clk),                           // Clock signal to synchronize operations.
        .set(LorA),                          // Signal to set (load) or add to the register.
        .shift(shift),                       // Signal to shift the contents of the register.
        .shift_in({sign_extend, sign_extend}), // Input bits for shifting, based on sign extension.
        .din(reset),                         // Data input, coming from the RESET module or partial sum.
        .dout(product_1)                     // Output of the register, fed back into the system.
    );
    
    // Counter used for controlling the number of multiplication cycles.
    Counter#(
        .N(N)  // Using the same parameter N for consistency with bit width.
    ) timer (
        .start(load),   // Start signal for the counter, initiates counting.
        .tick(count),   // Tick signal used to increment the counter.
        .done(tick)     // Done signal, indicates counting is complete.
    );
    
    // Control unit orchestrates the overall operation of the multiplication.
    CTRL #(
        .N(N)  // Parameter for matching bit width.
    ) ctrl_unit (
        .clk(clk),         // Clock signal for synchronization.
        .rst(rst),         // Reset signal for initializing the control unit.
        .start(start),     // Start signal to kick-off the multiplication process.
        .alarm(tick),      // Alarm or trigger from the counter indicating a cycle is complete.
        .bits(reset[1:0]), // Bits used for control decisions, derived from the RESET module.
        .shiftReg(shift),  // Signal to control shifting operation.
        .addReg(add),      // Signal to control addition operation.
        .loadReg(load),    // Signal to control data loading operation.
        .tick(count),      // Signal to manage timing and counting.
        .busy(busy),       // Output signal indicating the module is processing.
        .done(done),       // Output signal indicating the multiplication is complete.
        .shift_data(data)  // Data involved in shifting operations.
    );  
    
    // Booth recoding unit for transforming multiplier bits into control signals.
    MUX_recoded booth (
        .mul_data(product_0[2:0]),  // Input data from the shifted multiplier.
        .recoded_data(code)         // Output recoded data for controlling the multiplication steps.
    );

    // Final product assembly, combining sign extension, partial product, and data.
    // This forms the complete output product from the multiplication process.
    assign product = {sign_extend, product_1[N:0], data};  // Concatenate to form the full product.

endmodule

