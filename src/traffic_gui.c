#include <stdio.h>
#include <SDL.h>
#include "traffic_simulator.h"

// Constants for the graphical display
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600
#define LANE_HEIGHT 50
#define VEHICLE_WIDTH 30
#define VEHICLE_HEIGHT 20

// Colors for different vehicle types
SDL_Color car_color = {0, 100, 255, 255};      // Blue
SDL_Color truck_color = {255, 100, 0, 255};    // Orange
SDL_Color motorcycle_color = {0, 200, 0, 255}; // Green
SDL_Color background_color = {50, 50, 50, 255}; // Dark gray
SDL_Color lane_marker_color = {255, 255, 255, 255}; // White

// SDL variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

/**
 * Initialize the SDL window and renderer
 * @param road Pointer to the road structure
 * @return 0 if successful, -1 on error
 */
int init_graphics(Road *road) {
    if (road == NULL) {
        return -1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Create window
    window = SDL_CreateWindow("Traffic Simulator", 
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                              WINDOW_WIDTH, WINDOW_HEIGHT, 
                              SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    printf("Graphics initialized successfully.\n");
    return 0;
}

/**
 * Clean up SDL resources
 */
void cleanup_graphics() {
    if (renderer != NULL) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    
    if (window != NULL) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    
    SDL_Quit();
    printf("Graphics resources cleaned up.\n");
}

/**
 * Render the current state of the traffic simulation
 * @param road Pointer to the road structure
 * @param time_step Current simulation time step
 */
void render_traffic(Road *road, int time_step) {
    if (road == NULL || renderer == NULL) {
        return;
    }
    
    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, 
                          background_color.b, background_color.a);
    SDL_RenderClear(renderer);
    
    // Calculate scaling factor for road length
    double scale_factor = (double)WINDOW_WIDTH / road->length;
    
    // Calculate the total height needed for all lanes
    int total_lanes_height = road->lanes * LANE_HEIGHT;
    int start_y = (WINDOW_HEIGHT - total_lanes_height) / 2;
    
    // Draw lane markers
    SDL_SetRenderDrawColor(renderer, lane_marker_color.r, lane_marker_color.g, 
                          lane_marker_color.b, lane_marker_color.a);
    
    // Draw horizontal lane dividers
    for (int i = 0; i <= road->lanes; i++) {
        int y = start_y + i * LANE_HEIGHT;
        SDL_RenderDrawLine(renderer, 0, y, WINDOW_WIDTH, y);
    }
    
    // Draw dashed lines for lane centers
    if (road->length > 20) {  // Only if road is long enough
        int dash_length = 20;
        for (int lane = 0; lane < road->lanes; lane++) {
            int y = start_y + lane * LANE_HEIGHT + LANE_HEIGHT / 2;
            
            for (int x = 0; x < road->length; x += dash_length * 2) {
                int dash_start = (int)(x * scale_factor);
                int dash_end = (int)((x + dash_length) * scale_factor);
                SDL_RenderDrawLine(renderer, dash_start, y, dash_end, y);
            }
        }
    }
    
    // Draw vehicles
    for (int i = 0; i < road->vehicles_count; i++) {
        Vehicle *v = &road->vehicles[i];
        
        if (v->status == VEHICLE_ACTIVE) {
            SDL_Rect vehicle_rect;
            vehicle_rect.x = (int)(v->position * scale_factor);
            vehicle_rect.y = start_y + v->lane * LANE_HEIGHT + (LANE_HEIGHT - VEHICLE_HEIGHT) / 2;
            
            // Adjust width based on vehicle type
            if (v->type == 1) {  // Truck
                vehicle_rect.w = (int)(VEHICLE_WIDTH * 1.5);
            } else {
                vehicle_rect.w = v->type == 2 ? VEHICLE_WIDTH / 2 : VEHICLE_WIDTH; // Motorcycles are smaller
            }
            
            vehicle_rect.h = VEHICLE_HEIGHT;
            
            // Set color based on vehicle type
            switch (v->type) {
                case 0: // Car
                    SDL_SetRenderDrawColor(renderer, car_color.r, car_color.g, car_color.b, car_color.a);
                    break;
                case 1: // Truck
                    SDL_SetRenderDrawColor(renderer, truck_color.r, truck_color.g, truck_color.b, truck_color.a);
                    break;
                case 2: // Motorcycle
                    SDL_SetRenderDrawColor(renderer, motorcycle_color.r, motorcycle_color.g, motorcycle_color.b, motorcycle_color.a);
                    break;
                default:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for unknown
            }
            
            // Draw filled rectangle for vehicle
            SDL_RenderFillRect(renderer, &vehicle_rect);
            
            // Add black outline
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &vehicle_rect);
        }
    }
    
    // Display statistics
    char stats_buffer[256];
    sprintf(stats_buffer, "Time: %d | Vehicles: %d | Generated: %d | Exited: %d",
            time_step, road->vehicles_count, road->total_vehicles_generated, 
            road->total_vehicles_exited);
    
    // We would need SDL_ttf to render text, but it's not included in this example
    // Instead, we'll print to the console
    printf("\r%s", stats_buffer);
    fflush(stdout);
    
    // Present the renderer
    SDL_RenderPresent(renderer);
    
    // Handle events
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            // Set a global running flag to false (needs to be accessible from main)
            extern int running;
            running = 0;
        }
    }
}