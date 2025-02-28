#ifndef TRAFFIC_SIMULATOR_H
#define TRAFFIC_SIMULATOR_H

// Constants
#define MAX_VEHICLES 500
#define MAX_LANES 10
#define MAX_ROAD_LENGTH 1000
#define MAX_SPEED 5

// Vehicle status
#define VEHICLE_ACTIVE 0
#define VEHICLE_EXITED 1

/**
 * Vehicle structure representing cars, trucks, etc.
 */
typedef struct {
    int id;           // Unique identifier
    int position;     // Current position on road (0 to length-1)
    int speed;        // Current speed (positions per time step)
    int lane;         // Current lane (0 to lanes-1)
    int type;         // Vehicle type (0: Car, 1: Truck, 2: Motorcycle)
    int waiting_time; // Time spent waiting/slow
    int status;       // Status flag (active, exited, etc.)
} Vehicle;

/**
 * Road structure representing the traffic system
 */
typedef struct {
    int lanes;                         // Number of lanes
    int length;                        // Length of the road
    int vehicles_count;                // Current number of vehicles on the road
    int total_vehicles_generated;      // Total vehicles created
    int total_vehicles_exited;         // Total vehicles that exited
    Vehicle vehicles[MAX_VEHICLES];    // Array of vehicles
    int grid[MAX_LANES][MAX_ROAD_LENGTH]; // Grid representation of vehicle positions
} Road;

// Function declarations for traffic_simulator.c
int init_road(Road *road, int lanes, int length);
int is_position_occupied(Road *road, int lane, int position);
void update_traffic(Road *road);
void display_road(Road *road);

// Function declarations for generator.c
int generate_vehicle(Road *road, int probability);
void init_generator(unsigned int seed, int entry_probability);

// Function declarations for traffic_gui.c
int init_graphics(Road *road);
void cleanup_graphics();
void render_traffic(Road *road, int time_step);

#endif /* TRAFFIC_SIMULATOR_H */