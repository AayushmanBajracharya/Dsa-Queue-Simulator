#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "traffic_simulator.h"

// Global variables
Road road;
int running = 1;
int time_step = 0;
int entry_probability = 20; // Default 20% chance of new vehicle per step
int use_graphics = 1;      // By default, use graphical display

// Signal handler for graceful termination
void handle_signal(int sig) {
    printf("\nShutting down traffic simulator...\n");
    running = 0;
}

// Display usage information
void display_usage(const char* program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("Options:\n");
    printf("  -l <lanes>     Number of lanes (default: 3)\n");
    printf("  -r <length>    Road length (default: 100)\n");
    printf("  -p <prob>      Entry probability (0-100, default: 20)\n");
    printf("  -s <seed>      Random seed (default: based on time)\n");
    printf("  -t <delay>     Delay between steps in milliseconds (default: 200)\n");
    printf("  -g <0|1>       Use graphics (0=off, 1=on, default: 1)\n");
    printf("  -h             Display this help message\n");
}

int main(int argc, char *argv[]) {
    // Default configuration
    int lanes = 3;
    int road_length = 100;
    int delay_ms = 200;
    unsigned int seed = 0;
    
    // Parse command line arguments
    int opt;
    while ((opt = getopt(argc, argv, "l:r:p:s:t:g:h")) != -1) {
        switch (opt) {
            case 'l':
                lanes = atoi(optarg);
                break;
            case 'r':
                road_length = atoi(optarg);
                break;
            case 'p':
                entry_probability = atoi(optarg);
                break;
            case 's':
                seed = (unsigned int)atoi(optarg);
                break;
            case 't':
                delay_ms = atoi(optarg);
                break;
            case 'g':
                use_graphics = atoi(optarg);
                break;
            case 'h':
                display_usage(argv[0]);
                return 0;
            default:
                display_usage(argv[0]);
                return 1;
        }
    }
    
    // Validate parameters
    if (lanes <= 0 || lanes > MAX_LANES) {
        printf("Error: Number of lanes must be between 1 and %d\n", MAX_LANES);
        return 1;
    }
    
    if (road_length <= 0 || road_length > MAX_ROAD_LENGTH) {
        printf("Error: Road length must be between 1 and %d\n", MAX_ROAD_LENGTH);
        return 1;
    }
    
    // Set up signal handlers for graceful termination
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    // Initialize the system
    init_generator(seed, entry_probability);
    init_road(&road, lanes, road_length);
    
    // Initialize graphics if requested
    if (use_graphics) {
        if (init_graphics(&road) != 0) {
            printf("Failed to initialize graphics, falling back to text mode.\n");
            use_graphics = 0;
        }
    }
    
    printf("Traffic Simulator Started\n");
    printf("----------------------------------------\n");
    printf("Road configuration: %d lanes, %d units long\n", lanes, road_length);
    printf("Press Ctrl+C to stop the simulation\n");
    printf("----------------------------------------\n\n");
    
    // Main simulation loop
    while (running) {
        time_step++;
        
        // Generate new vehicles
        generate_vehicle(&road, entry_probability);
        
        // Update vehicle positions and behaviors
        update_traffic(&road);
        
        // Display current state
        if (use_graphics) {
            // Use graphical display
            render_traffic(&road, time_step);
        } else {
            // Use text-based display
            printf("\033[2J\033[H"); // Clear screen (ANSI escape code)
            printf("Time step: %d | Vehicles: %d | Total created: %d | Total exited: %d\n\n", 
                   time_step, road.vehicles_count, road.total_vehicles_generated, 
                   road.total_vehicles_exited);
                   
            display_road(&road);
        }
        
        // Small delay between steps
        usleep(delay_ms * 1000);
    }
    
    // Clean up graphics resources
    if (use_graphics) {
        cleanup_graphics();
    }
    
    // Display final statistics
    printf("\nSimulation Summary:\n");
    printf("Total time steps: %d\n", time_step);
    printf("Total vehicles generated: %d\n", road.total_vehicles_generated);
    printf("Total vehicles that exited: %d\n", road.total_vehicles_exited);
    printf("Vehicles still on road: %d\n", road.vehicles_count);
    printf("Average flow rate: %.2f vehicles/time step\n", 
           (float)road.total_vehicles_exited / time_step);
    
    return 0;
}