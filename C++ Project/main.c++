#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <string>
using namespace std;
class Airport
{
public:
    string code;
    string name;

    Airport() : code(""), name("") {} 

    Airport(const string &code, const string &name)
        : code(code), name(name) {}
};


class Flight
{
public:
    string from;
    string to;
    int cost;

    Flight(const string &from, const string &to, int cost)
        : from(from), to(to), cost(cost) {}
};
map<string, Airport> airports;
map<string, vector<Flight>> flightNetwork;


// Modify the loadAirportsFromFile function to handle the new format
void loadAirportsFromFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Could not open " << filename << " for reading.\n";
        return;
    }

    string code, name;
    while (getline(file, code, ' ') && getline(file, name))
    {
        airports[code] = Airport(code, name);
    }

    file.close();
}

void loadFlightsFromFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Could not open " << filename << " for reading.\n";
        return;
    }

    string from, to;
    int cost;
    while (file >> from >> to >> cost)
    {
        flightNetwork[from].emplace_back(from, to, cost);
    }

    file.close();
}

void saveAirportsToFile(const string &filename)
{
    ofstream file(filename);
    if (!file.is_open())
    {
        cout << "Could not open " << filename << " for writing.\n";
        return;
    }

    for (const auto &pair : airports)
    {
        file << pair.first << " " << pair.second.name << "\n";
    }

    file.close();
}

void saveFlightsToFile(const string &filename)
{
    ofstream file(filename);
    if (!file.is_open())
    {
        cout << "Could not open " << filename << " for writing.\n";
        return;
    }

    for (const auto &pair : flightNetwork)
    {
        for (const Flight &flight : pair.second)
        {
            file << flight.from << " " << flight.to << " " << flight.cost << "\n";
        }
    }

    file.close();
}
map<string, pair<int, string>> dijkstra(const string &startAirport)
{
    map<string, pair<int, string>> result;  // Changed to pair<int, string>
    priority_queue<pair<int, string>,
                   vector<pair<int, string>>,
                   greater<pair<int, string>>>
        pq;

    // Initialize distances and add startAirport to the priority queue
    for (const auto &pair : airports)
    {
        if (pair.first == startAirport)
        {
            result[pair.first] = make_pair(0, "");  // Initialize previous to empty string
            pq.push({0, pair.first});
        }
        else
        {
            result[pair.first] = make_pair(INT_MAX, "");
            pq.push({INT_MAX, pair.first});
        }
    }

    while (!pq.empty())
    {
        pair<int, string> topPair = pq.top();
        pq.pop();
        int dist = topPair.first;
        string current = topPair.second;

        if (dist > result[current].first)
            continue;

        for (const auto &flight : flightNetwork[current])
        {
            int newDist = result[current].first + flight.cost;
            if (newDist < result[flight.to].first)
            {
                result[flight.to] = make_pair(newDist, current);  // Store the previous airport
                pq.push({newDist, flight.to});
            }
        }
    }

    return result;
}

void addAirport(const string &code, const string &name)
{
    airports[code] = Airport(code, name);
    cout << "Airport " << code << " added.\n";
}

void addFlight(const string &from, const string &to, int cost)
{
    if (airports.find(from) == airports.end() || airports.find(to) == airports.end())
    {
        cout << "One or both airports do not exist.\n";
        return;
    }

    flightNetwork[from].emplace_back(from, to, cost);
    cout << "Flight from " << from << " to " << to << " added.\n";
}

vector<string> getPath(const map<string, pair<int, string>> &distances, const string &endAirport)
{
    vector<string> path;
    string current = endAirport;
    while (!current.empty())
    {
        path.push_back(current);
        current = distances.at(current).second;
    }
    
    vector<string> reversedPath(path.rbegin(), path.rend());  // Create the reversed path
    return reversedPath;
}


void planItinerary(const string &startAirport, const string &endAirport)
{
    if (airports.find(startAirport) == airports.end() || airports.find(endAirport) == airports.end())
    {
        cout << "One or both airports do not exist.\n";
        return;
    }

    map<string, pair<int, string>> distances = dijkstra(startAirport);
    int distance = distances[endAirport].first;

    if (distance == INT_MAX)
    {
        cout << "No route available between " << startAirport << " and " << endAirport << ".\n";
        return;
    }

    vector<string> path = getPath(distances, endAirport);

    cout << "Shortest distance from " << startAirport << " to " << endAirport << ": " << distance << "\n";
    cout << "Path: ";
    for (const string &airport : path)
    {
        cout << airport << " ";
    }
    cout << "\n";
}
void saveUsernamesToFile(const string &filename, const map<string, string> &usernames)
{
    ofstream file(filename);
    if (!file.is_open())
    {
        cout << "Could not open " << filename << " for writing.\n";
        return;
    }

    for (const auto &pair : usernames)
    {
        file << pair.first << " " << pair.second << "\n";
    }

    file.close();
}
map<string, string> loadUsernamesFromFile(const string &filename)
{
    map<string, string> usernames;
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Could not open " << filename << " for reading.\n";
        return usernames;
    }

    string username, password;
    while (file >> username >> password)
    {
        usernames[username] = password;
    }

    file.close();
    return usernames;
}

int main()
{
    loadAirportsFromFile("airports.txt");
    loadFlightsFromFile("flights.txt");

    string username, password;
    cout << "Welcome to the Flight Management System\n";
    cout << "Enter your username: ";
    cin >> username;
    cout << "Enter your password: ";
    cin >> password;

    map<string, string> usernames = loadUsernamesFromFile("usernames.txt");

    if (usernames.find(username) == usernames.end())
    {
        string newUserPassword;
        cout << "User not found. Create a new user.\n";
        cout << "Enter a new password for user " << username << ": ";
        cin >> newUserPassword;

        usernames[username] = newUserPassword;
        saveUsernamesToFile("usernames.txt", usernames);
        cout << "User " << username << " created successfully.\n";
    }
    else if (usernames[username] == password)
    {
        cout << "Logged in as " << username << "\n";

        while (true)
        {
            int userChoice;
            cout << "User Menu:\n"
                 << "1. Plan Itinerary\n"
                 << "2. Add Airport\n"
                 << "3. Add Flight\n"
                 << "4. Exit\n"
                 << "Enter choice: ";
            cin >> userChoice;

            switch (userChoice)
            {
            case 1:
            {
                string start, end;
                cout << "Enter starting airport code: ";
                cin >> start;
                cout << "Enter destination airport code: ";
                cin >> end;

                planItinerary(start, end);
                break;
            }

            case 2:
            {
                string code, name;
                cout << "Enter airport code: ";
                cin >> code;
                cout << "Enter airport name: ";
                cin.ignore();
                getline(cin, name);

                addAirport(code, name);
                break;
            }

            case 3:
            {
                string from, to;
                int cost;
                cout << "Enter departure airport code: ";
                cin >> from;
                cout << "Enter destination airport code: ";
                cin >> to;
                cout << "Enter flight cost: ";
                cin >> cost;

                addFlight(from, to, cost);
                break;
            }

            case 4:
                saveAirportsToFile("airports.txt");
                saveFlightsToFile("flights.txt");
                cout << "Exiting user interface...\n";
                return 0;

            default:
                cout << "Invalid choice\n";
            }
        }
    }
    else
    {
        cout << "Invalid username or password\n";
    }

    return 0;
}
