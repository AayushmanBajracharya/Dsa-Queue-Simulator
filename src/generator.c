#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "traffic_simulator.h"

/**
 * Generates random vehicles to enter the road system
 * @param road Pointer to the road structure
 * @param probability Probability of generating a new vehicle (0-100)
 * @return 1 if a vehicle was generated, 0 otherwise
 */
int generate_vehicle(Road *road, int probability) {
    if (road == NULL || road->vehicles_count >= MAX_VEHICLES) {
        return 0;
    }
    
    // Generate a random number between 0 and 99
    int random = rand() % 100;
    
    // If random number is less than probability, generate a vehicle
    if (random < probability) {
        Vehicle new_vehicle;
        
        // Initialize vehicle properties
        new_vehicle.id = road->total_vehicles_generated + 1;
        new_vehicle.position = 0;
        new_vehicle.speed = 1 + rand() % MAX_SPEED; // Random speed between 1 and MAX_SPEED
        new_vehicle.lane = rand() % road->lanes;    // Random lane
        new_vehicle.type = rand() % 3;              // 0: Car, 1: Truck, 2: Motorcycle
        new_vehicle.waiting_time = 0;
        new_vehicle.status = VEHICLE_ACTIVE;
        
        // Add vehicle to the road
        road->vehicles[road->vehicles_count] = new_vehicle;
        road->vehicles_count++;
        road->total_vehicles_generated++;
        
        return 1;
    }
    
    return 0;
}

/**
 * Initializes traffic generator settings
 * @param seed Random seed for the generator
 * @param entry_probability Probability of a vehicle entering the system
 */
void init_generator(unsigned int seed, int entry_probability) {
    // Seed the random number generator
    if (seed == 0) {
        // Use current time as seed if not specified
        srand((unsigned int)time(NULL));
    } else {
        srand(seed);
    }
    
    // Validate and store entry probability
    if (entry_probability < 0) {
        entry_probability = 0;
    } else if (entry_probability > 100) {
        entry_probability = 100;
    }
    
    printf("Traffic generator initialized with probability: %d%%\n", entry_probability);
}