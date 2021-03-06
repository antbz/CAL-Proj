#ifndef PROJ_FARM_H
#define PROJ_FARM_H

#include <vector>
#include <unordered_map>
#include "Utils/Utils.h"
#include "Client.h"
#include "Basket.h"
#include "Truck.h"
#include "../Graph/Graph.h"
#include "../UI/Menu.h"

class Farm {
private:
    string farm_file, clients_file;
    bool farmFileChanged = false;
    bool clientsFileChanged = false;
    int farmNodeID;
    int garageNodeID;
    vector<Truck> trucks;
    unordered_map<int, Client> clients;
    unordered_map<int,vector<Basket>> baskets;
public:
    Farm();
    Farm(string farm_file, string clients_file);
    Farm(int farmNodeID);

    string getFarmFile();
    string getClientsFile();

    bool isFarmFileChanged() const;
    bool isClientsFileChanged() const;

    void setFarmNodeID(int nodeID);
    int getFarmNodeID() const;

    void setGarageNodeID(int nodeID);
    int getGarageNodeID() const;

    unordered_map<int, vector<Basket>> getBaskets();
    void addBasket(Basket basket);

    unordered_map<int, Client> getClients();
    void addClient(Client client);

    vector<Truck> getTrucks();
    vector<double> getCapacities();
    void addTruck(Truck truck);

    bool searchClientByNIF(int nif);
    void createClient(Graph<coordinates> &graph);
    void editClient(Graph<coordinates> &graph);
    void removeClient(Graph<coordinates> &graph);

    void createBasket();
    void editBasket();
    void removeBasket();
    void clearBaskets();

    bool searchTruckByPlate(string plate);
    void createTruck();
    void editTruck();
    void removeTruck();

};


#endif //PROJ_FARM_H
