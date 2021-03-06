#include "MenuUtils.h"

void loadGraph(Graph<coordinates> &graph, Farm &farm, vector<int> &last_path, const string &nodes_file, const string &edges_file, bool grid) {
    graph = parseMap(nodes_file, edges_file, grid);
    farm = Farm();
    last_path.clear();
}

void loadGraph(Graph<coordinates> &graph, Farm &farm, vector<int> &last_path, const string &nodes_file, const string &edges_file, bool grid, const string &farm_file, const string &clients_file) {
    graph = parseMap(nodes_file, edges_file, grid);
    farm = loadContext(graph, farm_file, clients_file);
    last_path.clear();
}

void printPath(const vector<int> &path) {
    for (int i = 0; i < path.size(); i++) {
        if (i == 0) {
            cout << path.at(i);
        } else {
            cout << " -> " << path.at(i);
        }
    }
    cout << endl;
}

void printRoutes(const vector<vector<int>> &routes) {
    for (int i = 0; i < routes.size(); i++) {
        line(20);
        cout << "Route " << i+1 << endl;
        printPath(routes.at(i));
    }
}

void shortestPath(Graph<coordinates> &graph, UI &ui, const function<Path (int, int)> &spAlgorithm) {
    int id_src, id_dest;
    getOption(id_src, "Source node ID: ", [&](int a) { return graph.isValidID(a); });
    getOption(id_dest, "Destination node ID: ", [&](int a) { return graph.isValidID(a); });

    auto start = high_resolution_clock::now();
    Path path = spAlgorithm(id_src, id_dest);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Size: " << path.getLength() << endl;
    cout << "Time to pathfind (micro): " << duration.count() << endl;

    Menu showMenu("Show path?", false);
    showMenu.addOption("No", EXIT);
    showMenu.addOption("Yes, in GraphViewer", [&](){ ui.showPath(path.getPath()); });
    showMenu.addOption("Yes, in console", [&](){ printPath(path.getPath()); enterWait(); });
    showMenu.start();
}

void dijkstra(Graph<coordinates> &graph, UI &ui) {
    shortestPath(graph, ui, [&](int s, int d) { return graph.dijkstraShortestPath(s, d); });
}

void dijkstraBiDir(Graph<coordinates> &graph, UI &ui) {
    shortestPath(graph, ui, [&](int s, int d) { return graph.dijkstraBiDirShortestPath(s, d); });
}

void astar(Graph<coordinates> &graph, UI &ui) {
    shortestPath(graph, ui, [&](int s, int d) { return graph.astarShortestPath(s, d); });
}

void astarBiDir(Graph<coordinates> &graph, UI &ui) {
    shortestPath(graph, ui, [&](int s, int d) { return graph.astarBiDirShortestPath(s, d); });
}

void solveTSPRoute(Graph<coordinates> &graph, UI &ui, const int &start_node, const int &end_node, vector<int> &POIs) {
    function<Path ()> tsp_algorithm;
    Path path;
    vector<int> ord;
    bool cancel = false;

    Menu algorithmSelector("Select algorithm", false);
    algorithmSelector.addOption("Cancel", [&](){ cancel = true; });
    algorithmSelector.addOption("Nearest-neighbor", [&](){ tsp_algorithm = [&](){ return graph.nearestNeighborsSearch(start_node, end_node, POIs, ord, path); }; });
    algorithmSelector.addOption("Nearest-neighbor 2-opt", [&](){ tsp_algorithm = [&](){ graph.nearestNeighborsSearch(start_node, end_node, POIs, ord, path); return graph.twoOpt(ord, path); }; });
    algorithmSelector.addOption("Randomized NN", [&](){ tsp_algorithm = [&](){ return graph.RNNeighborsSearch(start_node, end_node, POIs, ord, path, 3); }; });
    algorithmSelector.addOption("Randomized NN 2-opt", [&](){ tsp_algorithm = [&](){ graph.RNNeighborsSearch(start_node, end_node, POIs, ord, path, 3); return graph.twoOpt(ord, path); }; });
    algorithmSelector.start();
    if (cancel) { return; }

    auto start = high_resolution_clock::now();
    path = tsp_algorithm();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Size: " << path.getLength() << endl;
    cout << "Time to pathfind (micro): " << duration.count() << endl;

    Menu showMenu("Show path?", false);
    showMenu.addOption("No", EXIT);
    showMenu.addOption("Yes, in GraphViewer", [&](){ ui.showDeliveryPath(path.getPath(), ord); });
    showMenu.addOption("Yes, in console", [&](){ printPath(ord); enterWait(); });
    showMenu.start();
}

void solveTSPMultipleRoutes(Graph<coordinates> &graph, UI &ui, vector<Route> &routes, int start_node, int end_node) {
    function<Path (int &start_node, int &end_node, vector<int> &POIs, vector<int> &ord, Path &path)> tsp_algorithm;
    vector<vector<int>> route_paths;
    vector<vector<int>> route_pois;
    vector<Path> paths;

    bool cancel = false;

    Menu algorithmSelector("Select algorithm", false);
    algorithmSelector.addOption("Cancel", [&](){ cancel = true; });
    algorithmSelector.addOption("Nearest-neighbor", [&](){ tsp_algorithm = [&](int &start_node, int &end_node, vector<int> &POIs, vector<int> &ord, Path &path){ return graph.nearestNeighborsSearch(start_node, end_node, POIs, ord, path); }; });
    algorithmSelector.addOption("Nearest-neighbor 2-opt", [&](){ tsp_algorithm = [&](int &start_node, int &end_node, vector<int> &POIs, vector<int> &ord, Path &path){ graph.nearestNeighborsSearch(start_node, end_node, POIs, ord, path); return graph.twoOpt(ord, path); }; });
    algorithmSelector.addOption("Randomized NN", [&](){ tsp_algorithm = [&](int &start_node, int &end_node, vector<int> &POIs, vector<int> &ord, Path &path){ return graph.RNNeighborsSearch(start_node, end_node, POIs, ord, path, 3); }; });
    algorithmSelector.addOption("Randomized NN 2-opt", [&](){ tsp_algorithm = [&](int &start_node, int &end_node, vector<int> &POIs, vector<int> &ord, Path &path){ graph.RNNeighborsSearch(start_node, end_node, POIs, ord, path, 3); return graph.twoOpt(ord, path); }; });
    algorithmSelector.start();
    if (cancel) { return; }

    auto start = high_resolution_clock::now();
    for (auto &route : routes) {
        vector<int> POIs = route.getRoutePOIs();
        vector<int> ord;
        Path path;
        path = tsp_algorithm(start_node, end_node, POIs, ord, path);
        paths.push_back(path);
        route_paths.push_back(path.getPath());
        route_pois.push_back(ord);
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    double total_size = 0;
    for (int p = 0; p < paths.size(); p++) {
        cout << "Route " << p+1 << " (" << routes.at(p).getTruck() << ")" << " - Size: " << paths.at(p).getLength() << endl;
        total_size += paths.at(p).getLength();
    }
    cout << "Total size: " << total_size << endl;
    cout << "Time to pathfind (micro): " << duration.count() << endl;

    Menu showMenu("Show routes?", false);
    showMenu.addOption("No", EXIT);
    showMenu.addOption("Yes, in GraphViewer", [&](){ ui.showRoutes(route_paths, route_pois); });
    showMenu.addOption("Yes, in console", [&](){ printRoutes(route_pois); enterWait(); });
    showMenu.start();
}

void solveTSPwithContext(Graph<coordinates> &graph, UI &ui, Farm &farm) {
    int total_baskets = 0;
    int start_node = farm.getFarmNodeID(), end_node = farm.getGarageNodeID();
    vector<int> POIs;

    for (const auto& cb_pair : farm.getBaskets()) {
        POIs.push_back(farm.getClients()[cb_pair.first].getClientNodeID());
        total_baskets += cb_pair.second.size();
    }

    cout << "Visiting " << farm.getBaskets().size() << " nodes to deliver " << total_baskets << " baskets." << endl;

    solveTSPRoute(graph, ui, start_node, end_node, POIs);
}

void solveTSPnoContext(Graph<coordinates> &graph, UI &ui) {
    vector<int> POIs;
    int start_node, end_node;

    getOption(start_node, "Source node ID: ", [&](int a) { return graph.isValidID(a); });
    getOption(end_node, "Destination node ID: ", [&](int a) { return graph.isValidID(a); });
    getOptionList(POIs, "Node to visit: ", [&](int a) { return graph.isValidID(a); });

    cout << "Visiting " << POIs.size() << " nodes." << endl;

    solveTSPRoute(graph, ui, start_node, end_node, POIs);
}

void solveVRPsweep(Graph<coordinates> &graph, UI &ui, Farm &farm) {
    int total_baskets = 0;
    double total_weight = 0;
    vector<deliverypoint> POIs;

    for (const auto& cb_pair : farm.getBaskets()) {
        double weight = 0;
        for (auto basket : cb_pair.second) {
            weight += basket.getWeight();
        }
        POIs.emplace_back(farm.getClients()[cb_pair.first].getClientNodeID(), weight);
        total_baskets += cb_pair.second.size();
        total_weight += weight;
    }

    cout << "Visiting " << farm.getBaskets().size() << " nodes to deliver " << total_baskets << " baskets with a total weight of " << total_weight << endl;

    vector<Truck> trucks = farm.getTrucks();
    vector<Route> routes = graph.sweep(farm.getFarmNodeID(), trucks, POIs);

    if (routes.empty()) {
        cout << "Sweep algorithm could not determine a solution!" << endl;
        enterWait();
        return;
    }

    cout << "Calculated " << routes.size() << " routes to fulfill deliveries." << endl;

    solveTSPMultipleRoutes(graph, ui, routes, farm.getFarmNodeID(), farm.getGarageNodeID());
}

vector<int> largestSCC(Graph<coordinates> &graph, UI &ui) {
    auto start = high_resolution_clock::now();
    vector<vector<int>> scc_list = graph.tarjan(0);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time (micro): " << duration.count() << endl;
    line(20);
    cout << "Connectivity analysis" << endl;
    line(20);
    cout << "Total number of strongly connected components: " << scc_list.size() << endl;

    auto largest = max_element(scc_list.begin(), scc_list.end(), [&](vector<int> &scc1, vector<int> &scc2) { return scc1.size() < scc2.size(); });
    double average_scc = 0;

    for (auto scc : scc_list) {
        average_scc += scc.size();
    }

    average_scc /= scc_list.size();

    cout << "Largest SCC size: " << largest->size() << endl;
    cout << "Average SCC size: " << average_scc << endl;

    Menu showMenu("Show largest SCC?", false);
    showMenu.addOption("No", EXIT);
    showMenu.addOption("Yes", [&](){ ui.showPath(*largest); });
    showMenu.start();

    return *largest;
}

bool isContextLoaded(Farm &farm){
    if(farm.getFarmNodeID() == -1){
        cinERR("No context loaded! Please load a graph with problem context before accessing this functionality.");
        enterWait();
        return false;
    }
    return true;
}

void showClientList(Farm &farm) {
    line(20);
    cout << "Clients" << endl;
    line(20);
    for (const auto& cb_pair : farm.getClients()) {
        vector<Basket> baskets = farm.getBaskets()[cb_pair.first];
        double total = 0;
        for (Basket b: baskets){
            total += b.getWeight();
        }
        cout << "NIF: " << cb_pair.first << " | Node ID: " << cb_pair.second.getClientNodeID() << " | Baskets total weight: " << total << endl;
    }
    line(20);
    enterWait();
}

void showBasketList(Farm &farm){
    line(20);
    cout << "Baskets" << endl;
    line(20);
    for (const auto& cb_pair : farm.getBaskets()) {
        vector<Basket> baskets = cb_pair.second;
        for (Basket b: baskets){
            cout << "Client's NIF: " << cb_pair.first << " | Weight: " << b.getWeight() << endl;
        }
    }
    line(20);
    enterWait();
}

void showTruckList(Farm &farm){
    line(20);
    cout << "Baskets" << endl;
    line(20);
    for (Truck t: farm.getTrucks()) {
        cout << "Truck's plate: " << t.getPlate() << " | Capacity: " << t.getCapacity() << endl;
    }
    line(20);
    enterWait();
}

