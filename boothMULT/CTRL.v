`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/22/2024 12:02:20 PM
// Design Name: 
// Module Name: CTRL
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


// Control module for managing operations such as counting, shifting, and loading,
// parameterized by bit width N.
module CTRL #(
    parameter N = 8  // Bit width for shift_data.
)(
    input clk,              // Clock input for synchronous operation.
    input rst,              // Active low reset to initialize the state machine.
    input start,            // Start signal to initiate counting or processing.
    input alarm,            // Alarm or external signal to transition from counting.
    input [1:0] bits,       // Input bits for shifting into shift_data.
    output reg busy,        // Busy signal indicating the machine is in operation.
    output reg done,        // Done signal indicating completion of a cycle.
    output reg count,       // Count signal to trigger counting (unused in current context).
    output reg tick,        // Tick signal, typically used for timing or synchronization.
    output reg addReg,      // Control signal to trigger addition operation.
    output reg shiftReg,    // Control signal to trigger shifting operation.
    output reg loadReg,     // Control signal to trigger loading operation.
    output reg [N-1:0] shift_data  // Data register for holding or shifting data.
);

    // State definitions for the state machine.
    reg [2:0] state, next_state;  // Current and next state variables.
    
    // State constants for readability and maintainability.
    parameter RESET = 2'b00;  // State constant for the reset state.
    parameter COUNT = 2'b11;  // State constant for the counting state.
    
    // Shift operation for shift_data, happens every positive clock edge or when reset.
    always @(posedge clk or negedge rst) begin
        if (!rst)
            shift_data <= 0;  // Clear shift_data on reset.
        else
            shift_data <= {bits, shift_data[N-1:2]};  // Shift in new bits on clock edge.
    end
    
    // State transition logic, triggered on clock edge or reset.
    always @(posedge clk or negedge rst) begin
        if (!rst)
            state <= RESET;  // Move to RESET state on reset.
        else
            state <= next_state;  // Transition to next state on clock edge.
    end
    
    // State behavior and output logic based on current state and inputs.
    always @(*) begin
        case (state)
            RESET: begin  // In RESET state...
                if (start == 1'b1) begin
                    next_state = COUNT;  // Move to COUNT state if start is high.
                    busy = 1'b1;  // Indicate that machine is busy.
                    done = 1'b0;  // Not done yet.
                end else begin
                    next_state = RESET;  // Stay in RESET if start is not high.
                    busy = 1'b0;  // Indicate machine is not busy.
                    done = 1'b0;  // Not done as we are in reset.
                end
            end
            COUNT: begin  // In COUNT state...
                if (alarm == 1'b0) begin
                    next_state = RESET;  // Return to RESET state if alarm is low.
                    busy = 1'b1;  // Still busy as we were counting.
                    done = 1'b1;  // Indicate completion as we leave COUNT state.
                end else begin
                    next_state = COUNT;  // Stay in COUNT state if alarm is high.
                    busy = 1'b1;  // Indicate machine is busy.
                    done = 1'b0;  // Not done as counting continues.
                end
            end
            default: begin  // In default case...
                next_state = RESET;  // Return to RESET for any undefined states.
                busy = 1'b0;  // Not busy in undefined state.
                done = 1'b0;  // Not done as this is an unexpected state.
            end
        endcase
    end
    
    // Output logic for control signals based on current state.
    always @(*) begin
        if (rst == 1'b0) begin  // If in reset...
            loadReg = 1'b1;  // Prepare to load data.
            addReg = 1'b0;  // No addition.
            shiftReg = 1'b0;  // No shifting.
            tick = 1'b0;  // No tick signal.
        end else begin
            case (state)
              RESET: 
		begin		
			if (start == 1'b1)
				loadReg 	= 1'b1;
			else
				loadReg 	= 1'b0;
			addReg 	= 1'b0;
			shiftReg = 1'b0;
			tick 	= 1'b0;					
		end
		COUNT: 
		begin
			loadReg 	= 1'b0;
			addReg 	= 1'b1;
			shiftReg = 1'b1;
			tick 	= ~clk;
		end
		default: 
			begin
			loadReg 	= 1'b0;
			addReg 	= 1'b0;
			shiftReg = 1'b0;
			tick 	= 1'b0;
			end
	endcase	
	end
end

endmodule

