#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "traffic_simulator.h"

/**
 * Initialize the road structure
 * @param road Pointer to the road structure
 * @param lanes Number of lanes
 * @param length Length of the road
 * @return 0 if successful, -1 otherwise
 */
int init_road(Road *road, int lanes, int length) {
    if (road == NULL || lanes <= 0 || lanes > MAX_LANES || 
        length <= 0 || length > MAX_ROAD_LENGTH) {
        return -1;
    }
    
    // Initialize road properties
    road->lanes = lanes;
    road->length = length;
    road->vehicles_count = 0;
    road->total_vehicles_generated = 0;
    road->total_vehicles_exited = 0;
    
    // Clear vehicle array
    memset(road->vehicles, 0, sizeof(road->vehicles));
    
    // Create empty road grid
    for (int i = 0; i < MAX_LANES; i++) {
        for (int j = 0; j < MAX_ROAD_LENGTH; j++) {
            road->grid[i][j] = -1; // -1 means empty
        }
    }
    
    return 0;
}

/**
 * Check if a position on the road is occupied
 * @param road Pointer to the road structure
 * @param lane Lane number
 * @param position Position on the lane
 * @return 1 if occupied, 0 if free, -1 on error
 */
int is_position_occupied(Road *road, int lane, int position) {
    if (road == NULL || lane < 0 || lane >= road->lanes || 
        position < 0 || position >= road->length) {
        return -1;
    }
    
    return (road->grid[lane][position] != -1) ? 1 : 0;
}

/**
 * Update the entire traffic system for one time step
 * @param road Pointer to the road structure
 */
void update_traffic(Road *road) {
    if (road == NULL) {
        return;
    }
    
    // Clear the road grid
    for (int i = 0; i < MAX_LANES; i++) {
        for (int j = 0; j < MAX_ROAD_LENGTH; j++) {
            road->grid[i][j] = -1;
        }
    }
    
    // Update each vehicle
    for (int i = 0; i < road->vehicles_count; i++) {
        // Skip vehicles that have exited
        if (road->vehicles[i].status == VEHICLE_EXITED) {
            continue;
        }
        
        Vehicle *vehicle = &road->vehicles[i];
        
        // Check if vehicle should change lanes
        int should_change_lane = 0;
        int new_lane = vehicle->lane;
        
        // Simple lane-changing logic: 
        // Try to find a faster lane if current speed is limited
        int blocked = 0;
        
        // Check if there's a vehicle ahead in current lane
        for (int pos = vehicle->position + 1; 
             pos <= vehicle->position + vehicle->speed && pos < road->length; 
             pos++) {
            if (is_position_occupied(road, vehicle->lane, pos)) {
                blocked = 1;
                break;
            }
        }
        
        // If blocked, consider changing lanes
        if (blocked && rand() % 100 < 40) { // 40% chance to try changing lanes
            // Check lanes up and down
            int possible_lanes[2] = {
                vehicle->lane - 1,
                vehicle->lane + 1
            };
            
            for (int j = 0; j < 2; j++) {
                int candidate_lane = possible_lanes[j];
                
                // Skip invalid lanes
                if (candidate_lane < 0 || candidate_lane >= road->lanes) {
                    continue;
                }
                
                // Check if the lane change is safe
                int safe = 1;
                
                // Check current position in target lane (side)
                if (is_position_occupied(road, candidate_lane, vehicle->position)) {
                    safe = 0;
                    continue;
                }
                
                // Check behind in target lane (blind spot)
                for (int pos = vehicle->position - 1; 
                     pos >= vehicle->position - 2 && pos >= 0; 
                     pos--) {
                    if (is_position_occupied(road, candidate_lane, pos)) {
                        safe = 0;
                        break;
                    }
                }
                
                // Check ahead in target lane for enough space
                int space_ahead = 0;
                for (int pos = vehicle->position + 1; 
                     pos <= vehicle->position + vehicle->speed && pos < road->length; 
                     pos++) {
                    if (!is_position_occupied(road, candidate_lane, pos)) {
                        space_ahead++;
                    } else {
                        break;
                    }
                }
                
                if (safe && space_ahead > 0) {
                    should_change_lane = 1;
                    new_lane = candidate_lane;
                    break;
                }
            }
        }
        
        // Update lane if needed
        if (should_change_lane) {
            vehicle->lane = new_lane;
        }
        
        // Calculate new position based on speed and obstacles
        int new_position = vehicle->position;
        int actual_speed = vehicle->speed;
        
        // Check for obstacles and adjust speed accordingly
        for (int pos = vehicle->position + 1; 
             pos <= vehicle->position + vehicle->speed && pos < road->length; 
             pos++) {
            if (is_position_occupied(road, vehicle->lane, pos)) {
                actual_speed = pos - vehicle->position - 1;
                break;
            }
        }
        
        // Apply actual speed
        new_position += actual_speed;
        
        // Check if vehicle has exited the road
        if (new_position >= road->length) {
            vehicle->status = VEHICLE_EXITED;
            road->total_vehicles_exited++;
        } else {
            // Update position
            vehicle->position = new_position;
            
            // Mark position on grid
            road->grid[vehicle->lane][vehicle->position] = i;
        }
    }
    
    // Remove exited vehicles from the array by compacting it
    int new_count = 0;
    for (int i = 0; i < road->vehicles_count; i++) {
        if (road->vehicles[i].status != VEHICLE_EXITED) {
            if (i != new_count) {
                road->vehicles[new_count] = road->vehicles[i];
            }
            new_count++;
        }
    }
    
    road->vehicles_count = new_count;
}

/**
 * Display the current state of the road
 * @param road Pointer to the road structure
 */
void display_road(Road *road) {
    if (road == NULL) {
        return;
    }
    
    // Display legend
    printf("Legend: [C]=Car  [T]=Truck  [M]=Motorcycle  [ ]=Empty\n\n");
    
    // Display road boundaries
    for (int i = 0; i < road->length + 2; i++) {
        printf("=");
    }
    printf("\n");
    
    // Display each lane
    for (int lane = 0; lane < road->lanes; lane++) {
        printf("|");
        
        for (int pos = 0; pos < road->length; pos++) {
            int vehicle_idx = road->grid[lane][pos];
            
            if (vehicle_idx == -1) {
                printf(" ");
            } else {
                Vehicle *v = &road->vehicles[vehicle_idx];
                
                switch (v->type) {
                    case 0: // Car
                        printf("C");
                        break;
                    case 1: // Truck
                        printf("T");
                        break;
                    case 2: // Motorcycle
                        printf("M");
                        break;
                    default:
                        printf("V");
                }
            }
        }
        
        printf("|\n");
    }
    
    // Display road boundaries
    for (int i = 0; i < road->length + 2; i++) {
        printf("=");
    }
    printf("\n");
    
    // Display some vehicle details
    printf("\nVehicle details (showing up to 5):\n");
    int shown = 0;
    for (int i = 0; i < road->vehicles_count && shown < 5; i++) {
        Vehicle *v = &road->vehicles[i];
        const char *type_str = "Unknown";
        
        switch (v->type) {
            case 0: type_str = "Car"; break;
            case 1: type_str = "Truck"; break;
            case 2: type_str = "Motorcycle"; break;
        }
        
        printf("ID: %3d | Type: %-10s | Lane: %d | Pos: %3d | Speed: %d\n",
               v->id, type_str, v->lane, v->position, v->speed);
        shown++;
    }
    
    if (road->vehicles_count > 5) {
        printf("... and %d more vehicles\n", road->vehicles_count - 5);
    }
}