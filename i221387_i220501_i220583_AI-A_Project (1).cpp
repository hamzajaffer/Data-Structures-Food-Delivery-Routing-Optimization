// 22i-0501 Shaheer Qadir
// 22i-0583 Hamza Jaffer
// 22i-1387 Huzaifa Aziz
// AI-A Data Structures  FINAL PROJECT

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <climits>

using namespace std;

// Class to represent a restaurant
class Restaurant 
{
public:
    string name;
    int location;
    int num_orders;
    int num_riders;

    Restaurant() {}

    // Constructor to initialize restaurant attributes
    Restaurant(string _name, int _location, int _num_orders) : name(_name), location(_location), num_orders(_num_orders) 
    {
        num_riders = 0;
    }
};

// Class to represent an order
class Order 
{
public:
    string name;
    int location;
    int delivery_time;

    Order() {}

    // Constructor to initialize order attributes
    Order(string _name, int _location, int _delivery_time) : name(_name), location(_location), delivery_time(_delivery_time) {}
};

// Class to represent a grid
class Grid 
{
public:
    int size;
    string** grid;

    Grid() {}

    // Constructor to initialize the grid with '-' in all cells
    Grid(int _size) : size(_size) 
    {
        grid = new string * [size + 2];
        for (int i = 0; i < size + 2; i++) 
        {
            grid[i] = new string[size + 2];
            for (int j = 0; j < size + 2; j++) 
            {
                grid[i][j] = "-";
            }
        }
    }

    // Destructor to deallocate memory
    ~Grid() {
        for (int i = 0; i < size + 2; i++) 
        {
            delete[] grid[i];
        }
        delete[] grid;
    }

    // Place a restaurant on the grid
    void placeRestaurant(string name, int location)
    {
        grid[location / size + 1][location % size + 1] = name;
    }

    // Place an order on the grid
    void placeOrder(int location, string order, int deliveryTime) 
    {
        grid[(location - 1) / size + 1][(location - 1) % size + 1] = order + "(" + to_string(deliveryTime) + ")";
    }

    // Display the grid
    void display() {
        for (int i = 1; i <= size; i++) 
        {
            for (int j = 1; j <= size; j++) 
            {
                if (grid[i][j] == "-") 
                {
                    cout << setw(20) << grid[i][j];
                }
                else 
                {
                    cout << setw(20) << grid[i][j];
                }
            }
            cout << endl;
        }
    }
};

// Function to save restaurant and order information to a file
void saveToFile(int N, int R, Restaurant* restaurants, Order* orders, ofstream& file) 
{
    file << "Number of Test Cases: 1\n";
    file << N << " " << R << endl;
    for (int i = 0; i < R; i++) 
    {
        file << restaurants[i].name << " " << restaurants[i].location << " " << restaurants[i].num_orders << " " << restaurants[i].num_riders << endl;
        for (int j = 0; j < restaurants[i].num_orders; j++)
        {
            file << orders[i * N + j].name << " "
                << orders[i * N + j].location << " "
                << orders[i * N + j].delivery_time << endl;
        }
    }
}

// Function to distribute riders among restaurants
void distributeRiders(Restaurant* restaurants, int R, int numRiders) 
{
    int ridersPerRestaurant = numRiders / R;
    int remainingRiders = numRiders % R;

    for (int i = 0; i < R; i++)
    {
        restaurants[i].num_riders = ridersPerRestaurant + (i < remainingRiders ? 1 : 0);
    }
}

// Function to create a graph representing distances between restaurants and orders
int** createGraph(int N, Restaurant* restaurants, Order* orders, int R, int numOrders)
{
    // Allocate memory for the graph
    int** graph = new int* [R + 2];
    for (int i = 0; i < R + 2; i++) 
    {
        graph[i] = new int[R + 2];
        // Initialize all distances to infinity except distances from a vertex to itself (0)
        fill(graph[i], graph[i] + R + 2, INT_MAX);
        graph[i][i] = 0; // Distance from a vertex to itself is 0
    }

    // Calculate distances between restaurants
    for (int i = 0; i < R; i++) 
    {
        for (int j = i + 1; j < R; j++) 
        {
            // Calculate Manhattan distance between restaurants and convert to grid distance
            int distance = abs(restaurants[i].location - restaurants[j].location) / N + abs(restaurants[i].location - restaurants[j].location) % N;
            // Store the distance in the graph (undirected edge)
            graph[i + 1][j + 1] = distance;
            graph[j + 1][i + 1] = distance;
        }
    }

    // Calculate distances between restaurants and orders
    for (int i = 0; i < R; i++) 
    {
        for (int j = 0; j < restaurants[i].num_orders; j++) 
        {
            int orderIndex = i * numOrders + j;
            // Calculate distance from each restaurant to its orders, considering delivery time
            int restaurantOrderDistance = abs(restaurants[i].location - orders[orderIndex].location) / N + abs(restaurants[i].location - orders[orderIndex].location) % N + orders[orderIndex].delivery_time;
            // Update the graph with the minimum distance from each restaurant to its orders
            graph[0][i + 1] = min(graph[0][i + 1], restaurantOrderDistance);
            graph[i + 1][R + 1] = min(graph[i + 1][R + 1], restaurantOrderDistance);
        }
    }

    // Calculate distances between orders
    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < restaurants[i].num_orders; j++)
        {
            int orderIndex1 = i * numOrders + j;
            for (int k = j + 1; k < restaurants[i].num_orders; ++k) 
            {
                int orderIndex2 = i * numOrders + k;
                // Calculate Manhattan distance between orders and convert to grid distance
                int distance = abs(orders[orderIndex1].location - orders[orderIndex2].location) / N + abs(orders[orderIndex1].location - orders[orderIndex2].location) % N;
                // Store the distance in the graph (undirected edge)
                graph[orderIndex1 + 1][orderIndex2 + 1] = distance;
                graph[orderIndex2 + 1][orderIndex1 + 1] = distance;
            }
        }
    }

    return graph; // Return the created graph
}


// Function to perform Dijkstra's algorithm for finding shortest paths
void dijkstra(int** graph, int vertices, int*& shortestPath)
{
    // Allocate memory for storing shortest paths
    shortestPath = new int[vertices];
    // Initialize shortest paths to infinity for all vertices
    fill(shortestPath, shortestPath + vertices, INT_MAX);
    // Distance from source vertex (index 0) to itself is 0
    shortestPath[0] = 0;

    // Array to track visited vertices during Dijkstra's algorithm
    bool* visited = new bool[vertices];
    fill(visited, visited + vertices, false);

    // Loop through all vertices except the last one (sink vertex)
    for (int i = 0; i < vertices - 1; i++) 
    {
        // Find the vertex with the minimum distance from the source among unvisited vertices
        int minDistance = INT_MAX, minVertex;
        for (int j = 0; j < vertices; j++) 
        {
            if (!visited[j] && shortestPath[j] <= minDistance)
            {
                minDistance = shortestPath[j];
                minVertex = j;
            }
        }

        // Mark the selected vertex as visited
        visited[minVertex] = true;

        // Update shortest paths to neighboring vertices through the selected vertex
        for (int j = 0; j < vertices; j++)
        {
            // Check if the vertex is unvisited, there is an edge to it, and updating the path via the selected vertex reduces the distance
            if (!visited[j] && graph[minVertex][j] != INT_MAX && shortestPath[minVertex] != INT_MAX && shortestPath[minVertex] + graph[minVertex][j] < shortestPath[j]) 
            {
                // Update the shortest path to the neighboring vertex
                shortestPath[j] = shortestPath[minVertex] + graph[minVertex][j];
            }
        }
    }

    // Free memory allocated for the visited array
    delete[] visited;
}


int main() 
{
    int T;
    cout << "Enter the number of test cases: ";
    cin >> T;

    // Loop through each test case
    for (int t = 0; t < T; ++t) 
    {
        int N, R, numRiders;
        bool retry = false;

        // Get input for grid size, number of restaurants, and number of riders
        do {
            cout << "Enter grid size (N) for test case " << t + 1 << ": ";
            cin >> N;
            cout << "Enter number of restaurants (R) for test case " << t + 1 << ": ";
            cin >> R;
            cout << "Enter number of riders: ";
            cin >> numRiders;

            if (N <= 0 || R <= 0 || numRiders <= 0) 
            {
                cout << "Grid size, number of restaurants, and number of riders must be positive integers. Please try again." << endl;
                retry = true;
            }
            else 
            {
                retry = false;
            }
        } while (retry);

        // Allocate memory for restaurants and orders
        Restaurant* restaurants = new Restaurant[R];
        Order* orders = new Order[R * N];

        // Get input for each restaurant and its orders
        for (int i = 0; i < R; i++) 
        {
            string name;
            int location, num_orders;
            cout << "Enter restaurant name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter restaurant location (from 1 to " << N * N << "): ";
            cin >> location;
            if (location < 1 || location > N * N) 
            {
                cout << "Invalid location. Please enter a number between 1 and " << N * N << "." << endl;
                --i;
                continue;
            }
            cout << "Enter number of orders for " << name << ": ";
            cin >> num_orders;

            if (num_orders <= 0) 
            {
                cout << "Number of orders must be a positive integer. Please try again." << endl;
                --i;
                continue;
            }

            restaurants[i] = Restaurant(name, location, num_orders);

            for (int j = 0; j < num_orders; j++) 
            {
                string orderName;
                int orderLocation, deliveryTime;
                cout << "Enter order name for " << name << "'s order " << j + 1 << ": ";
                cin >> orderName;
                cout << "Enter order location (from 1 to " << N * N << "): ";
                cin >> orderLocation;
                if (orderLocation < 1 || orderLocation > N * N) 
                {
                    cout << "Invalid location. Please enter a number between 1 and " << N * N << "." << endl;
                    --j;
                    continue;
                }
                cout << "Enter delivery time for " << name << "'s order " << j + 1 << ": ";
                cin >> deliveryTime;
                if (deliveryTime <= 0) 
                {
                    cout << "Delivery time must be a positive integer. Please try again." << endl;
                    --j;
                    continue;
                }
                orders[i * N + j] = Order(orderName, orderLocation, deliveryTime);
            }
        }

        // Distribute riders among restaurants
        distributeRiders(restaurants, R, numRiders);

        // Create a grid and place restaurants and orders on it
        Grid grid(N);
        for (int i = 0; i < R; i++) 
        {
            grid.placeRestaurant(restaurants[i].name, restaurants[i].location);
        }
        for (int i = 0; i < R; i++) 
        {
            for (int j = 0; j < restaurants[i].num_orders; j++) 
            {
                grid.placeOrder(orders[i * N + j].location, orders[i * N + j].name, orders[i * N + j].delivery_time);
            }
        }

        // Display the grid
        cout << "Grid with restaurants and orders for test case " << t + 1 << ":" << endl;
        grid.display();

        // Create a graph representing distances between restaurants and orders
        int** graph = createGraph(N, restaurants, orders, R, N);

        // Perform Dijkstra's algorithm to find shortest paths
        int* shortestPath;
        dijkstra(graph, R + 2, shortestPath);

        // Display the shortest path
        cout << "Shortest path: ";
        for (int i = 0; i < R + 2; i++) 
        {
            cout << shortestPath[i] << " ";
        }
        cout << endl;

        delete[] shortestPath;

        // Save information to a file
        ofstream file("input.txt");
        if (file.is_open()) 
        {
            saveToFile(N, R, restaurants, orders, file);
            file.close();
            cout << "Information saved to input.txt" << endl;
        }
        else 
        {
            cout << "Unable to open file!" << endl;
        }

        // Deallocate memory
        for (int i = 0; i < R + 2; i++) 
        {
            delete[] graph[i];
        }
        delete[] graph;
        delete[] restaurants;
        delete[] orders;
    }

}
