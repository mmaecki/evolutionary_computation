#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <list>
#include <memory>
#include <map>
#include <coroutine>
#include <algorithm>

using namespace std;


struct Result{
    int bestCost;
    int worstCost;
    int averageCost;
    vector<int> bestSolution;
    vector<int> worstSolution;

    Result(int bc, int wc, int ac, vector<int> bs, vector<int> ws)
        : bestCost(bc), worstCost(wc), averageCost(ac), bestSolution(bs), worstSolution(ws) {}
};


class Algo {
public:
    std::string name;
    virtual Result solve(vector<vector<int>> distances, vector<int> costs) =0;
    int calculate_cost(vector<int> solution, vector<vector<int>> distances, vector<int> costs){
        int cost = 0;
        for(int j=0; j<solution.size()-1; j++){
            cost += distances[solution[j]][solution[j+1]];
        }
        cost+=distances[solution[solution.size()-1]][solution[0]];
        for(int j=0;j<solution.size();j++){
            cost+=costs[solution[j]];
        }
        return cost;
    }
};

class RandomSearch: public Algo {
    int n;
public:
    RandomSearch(int n_param) : n(n_param) {
        name = "RandomSearch";
    }
    
    Result solve(vector<vector<int>> distances, vector<int> costs) {
        vector<int> bestSolution;
        vector<int> worstSolution;
        int bestCost = INT32_MAX;
        int averageCost = 0;
        int worstCost = 0;
        int solution_size = distances.size()/2;
        vector<int> current_solution;
        for(int i=0; i<n; i++){
            //empty vector size solution_size
            current_solution = vector<int>(solution_size);
            vector<int> visited(distances.size());
            //fill with random numbers
            for(int j=0; j<solution_size; j++){
                int next = rand() % distances.size();
                while(visited[next])next = rand() % distances.size();
                current_solution[j] = next;
                visited[next]=true;
            }
            int current_cost = calculate_cost(current_solution, distances, costs);
            if(current_cost < bestCost){
                bestCost = current_cost;
                bestSolution = current_solution;
            }
            if(current_cost > worstCost){
                worstCost = current_cost;
                worstSolution = current_solution;
            }
            averageCost += current_cost;

        }
        return Result(bestCost, worstCost, averageCost/distances.size(), bestSolution, worstSolution);
    }
};

class GreedyCycle: public Algo {
public:
    GreedyCycle() {
        name = "GreedyCycle";
    }




    Result solve(vector<vector<int>> distances, vector<int> costs, int i) {
        vector<int> bestSolution;
        vector<int> worstSolution;
        int bestCost = INT32_MAX;
        int worstCost = 0;
        int averageCost = 0;
        int solution_size = distances.size()/2;
        vector<int> current_solution;
        vector<bool> visited(costs.size());
        current_solution.push_back(i);
        visited[i] = true;

        while(current_solution.size() < solution_size){
            int smallest_increase = INT32_MAX;
            int insert_index = -1;
            int insert_node = -1;


            for(int j=0; j<current_solution.size(); j++){  // Dla każdego nodea z cyklu
                int min_distance = INT32_MAX;
                int min_index = -1;
                for(int k=0; k<distances.size(); k++){ //znajdź najbliższy nieodwiedzony node
                    if(visited[k]) continue;
                    int curr = -distances[current_solution[j == 0 ? current_solution.size() - 1 : j - 1]][current_solution[j]] + distances[current_solution[j == 0 ? current_solution.size() - 1 : j - 1]][k] + distances[k][current_solution[j]] + costs[k];
                    if(curr < min_distance){
                        min_distance = curr;
                        min_index = k;
                    }
                }
                if(min_distance < smallest_increase){
                    smallest_increase = min_distance;
                    insert_index = j;
                    insert_node = min_index;
                }
            } // koniec
            current_solution.insert(current_solution.begin() + insert_index, insert_node);
            visited[insert_node] = true; 
        }
        int current_cost = calculate_cost(current_solution, distances, costs);
        if(current_cost < bestCost){
            bestCost = current_cost;
            bestSolution = current_solution;
        }
        if(current_cost > worstCost){
            worstCost = current_cost;
            worstSolution = current_solution;
        }
        averageCost += current_cost;
        return Result(bestCost, worstCost, averageCost/distances.size(), bestSolution, worstSolution);
    }
};


class Greedy2RegretWieghted: public Algo {
public:
    Greedy2RegretWieghted() {
        name = "Greedy2RegretWeighted";
    }

    Result solve(vector<vector<int>> distances, vector<int> costs, int i) {
        vector<int> bestSolution;
        vector<int> worstSolution;
        int bestCost = INT32_MAX;
        int worstCost = 0;
        int averageCost = 0;
        int solution_size = distances.size()/2;
        vector<int> current_solution;
        current_solution.push_back(i);
        vector<bool> visited(costs.size());
        visited[i] = true;

        while(current_solution.size() < solution_size){
            
            int smallest_increase = INT32_MAX;
            int insert_index = -1;
            int insert_node = -1;
            int max_score = -INT32_MAX;


            for(int k=0; k<distances.size(); k++){ // dla wszystkich nieodwiedzonych nodeów
                if(visited[k]) continue;
                vector<int> insertion_cost_for_j;
                for(int j=0; j<current_solution.size(); j++){ // dla każdego nodea z cyklu
                    int curr = -distances[current_solution[j == 0 ? current_solution.size() - 1 : j - 1]][current_solution[j]] + distances[current_solution[j == 0 ? current_solution.size() - 1 : j - 1]][k] + distances[k][current_solution[j]] + costs[k];
                    insertion_cost_for_j.push_back(curr);
                }
                int smallest_index = -1;
                int smallest_value = INT32_MAX;
                int second_smallest_value = INT32_MAX;

                for (int h = 0; h < insertion_cost_for_j.size(); h++) {
                    if (insertion_cost_for_j[h] < smallest_value) {
                        second_smallest_value = smallest_value;
                        smallest_value = insertion_cost_for_j[h];
                        smallest_index = h;
                    } else if (insertion_cost_for_j[h] < second_smallest_value) {
                        second_smallest_value = insertion_cost_for_j[h];
                    }
                }
                int regret = second_smallest_value - smallest_value;
                int left_node_idx = smallest_index == 0 ? current_solution.size() -1 : smallest_index -1;
                int insertion_cost = - distances[current_solution[left_node_idx]][current_solution[smallest_index]] + distances[current_solution[left_node_idx]][k] + distances[k][current_solution[smallest_index]] + costs[k];
                int score = regret - insertion_cost;
                if(score> max_score){
                    max_score = score;
                    insert_index = smallest_index;
                    insert_node = k;
                }
            }

            current_solution.insert(current_solution.begin() + insert_index, insert_node);
            visited[insert_node] = true; 
        }
        int current_cost = calculate_cost(current_solution, distances, costs);
        if(current_cost < bestCost){
            bestCost = current_cost;
            bestSolution = current_solution;
        }
        if(current_cost > worstCost){
            worstCost = current_cost;
            worstSolution = current_solution;
        }
        averageCost += current_cost;
        return Result(bestCost, worstCost, averageCost/distances.size(), bestSolution, worstSolution);
    }
};



template <typename T>
struct generator {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    
    struct promise_type {
        T value;
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        generator get_return_object() { return generator{handle_type::from_promise(*this)}; }
        void unhandled_exception() { std::terminate(); }
        std::suspend_always yield_value(T val) {
            value = val;
            return {};
        }
    };

    bool move_next() { return coro ? (coro.resume(), !coro.done()) : false; }
    T current_value() { return coro.promise().value; }

    generator(generator const&) = delete;
    generator(generator&& other) : coro(other.coro) { other.coro = {}; }
    ~generator() { if (coro) coro.destroy(); }

private:
    generator(handle_type h) : coro(h) {}
    handle_type coro;
};


generator<unsigned int> iota(unsigned int n = 0)
{
    while (n<20)
        co_yield n++;
}





enum SearchType { greedy, steepest };
enum InitialSolutionType {random, GC, G2Rw};
enum NeighbourhoodType {intra, inter};
enum InterNeighbourhoodType {twoNode, twoEdges};
enum ProblemInstance {TSPA, TSPB, TSPC, TSPD};

std::map<SearchType, std::string> SearchTypeStrings = {
    {greedy, "greedy"},
    {steepest, "steepest"}
};

std::map<InitialSolutionType, std::string> InitialSolutionTypeStrings = {
    {random, "random"},
    {GC, "GreedyCycle"},
    {G2Rw, "Greedy2RegretWeighted"}
};

std::map<NeighbourhoodType, std::string> NeighbourhoodTypeStrings = {
    {intra, "intra"},
    {inter, "inter"}
};

std::map<InterNeighbourhoodType, std::string> InterNeighbourhoodTypeStrings = {
    {twoNode, "twoNode"},
    {twoEdges, "twoEdges"}
};

std::map<ProblemInstance, InitialSolutionType> BestInitialForInstance = {
    {TSPA, G2Rw},
    {TSPB, G2Rw},
    {TSPC, GC},
    {TSPD, G2Rw}
};


class LocalSearch {
public:
    std::string name;
    SearchType searchType;
    InitialSolutionType initialSolutionType;
    InterNeighbourhoodType intraNeighbourhoodType;
    vector<vector<int>> distances;
    vector<int> costs;
    vector<bool> visited;
    int nPoints;
    LocalSearch(SearchType st, InitialSolutionType ist, InterNeighbourhoodType nt, vector<vector<int>> d, vector<int> c) {
        searchType = st;
        initialSolutionType = ist;
        intraNeighbourhoodType = nt;
        name = "LocalSearch";
        name += "_" + SearchTypeStrings[searchType];
        name += "_" + InitialSolutionTypeStrings[initialSolutionType];
        name += "_" + InterNeighbourhoodTypeStrings[intraNeighbourhoodType];
        distances = d;
        costs = c;
        visited = vector<bool>(distances.size());
        nPoints = distances.size();
    }

    int calculate_cost(vector<int> solution){
        int cost = 0;
        for(int j=0; j<solution.size()-1; j++){
            cost += this->distances[solution[j]][solution[j+1]];
        }
        cost+=this->distances[solution[solution.size()-1]][solution[0]];
        for(int j=0;j<solution.size();j++){
            cost+=this->costs[solution[j]];
        }
        return cost;
    }

    // Result solve() {
    //     return
    // }

    vector<int> subSolve() {
        vector<int> solution = getInitialSolution(this->initialSolutionType);
        int solutionCost = calculate_cost(solution);
        while(true){
            vector<int> newSolution = search(solution, solutionCost);
            int newSolutionCost = calculate_cost(newSolution);
            if(newSolutionCost == solutionCost) break;
            solution = newSolution;
            solutionCost = newSolutionCost;
        }

        return solution;
    }

    vector<int> getInitialSolution(InitialSolutionType ist){
        if(ist == random){
            RandomSearch rs = RandomSearch(1);
            return rs.solve(this->distances, this->costs).bestSolution;
        }
        if (ist == GC)
        {
            GreedyCycle gc = GreedyCycle();
            return gc.solve(this->distances, this->costs).bestSolution;
        }
        if (ist == G2Rw)
        {
            Greedy2RegretWieghted g2rw = Greedy2RegretWieghted();
            return g2rw.solve(this->distances, this->costs).bestSolution;
        }
    }

    vector<int> search(vector<int> currentSolution, int currentSolutionCost){
        if(searchType == greedy) return greedySearch(currentSolution, currentSolutionCost);
        return steepestSearch(currentSolution, currentSolutionCost);
    }
    vector<int> greedySearch(vector<int> currentSolution, int currentSolutionCost){
        auto neigbourhoodIterator = neighbourhoodGenerator(currentSolution);
        while(neigbourhoodIterator.move_next()){
            auto neighbour = neigbourhoodIterator.current_value();
            int neighbourCost = calculate_cost(neighbour);
            if(neighbourCost < currentSolutionCost){
                return neighbour;
            }

        }
        return currentSolution;
    }

    vector<int> steepestSearch(vector<int> currentSolution, int currentSolutionCost){
        auto neigbourhoodIterator = neighbourhoodGenerator(currentSolution);
        vector<int> bestNeighbour = currentSolution;
        int bestNeighbourCost = currentSolutionCost;
        while(neigbourhoodIterator.move_next()){
            auto neighbour = neigbourhoodIterator.current_value();
            int neighbourCost = calculate_cost(neighbour);
            if(neighbourCost < bestNeighbourCost){
                bestNeighbour = neighbour;
                bestNeighbourCost = neighbourCost;
            }

        }
        return bestNeighbour;
    }

    generator<vector<int>> neighbourhoodGenerator(vector<int> currentSolution){
        vector<NeighbourhoodType> nTypeOrder = {intray, inter};
        shuffle(nTypeOrder.begin(), nTypeOrder.end(), default_random_engine(time(0)));
        for(auto nType: nTypeOrder){
            if(nType == intra){
                auto intraNeighbourhoodIterator = intraNeighbourhoodGenerator(currentSolution);
                while(intraNeighbourhoodIterator.move_next()){
                    co_yield intraNeighbourhoodIterator.current_value();
                }
            } else {
                auto interNeighbourhoodIterator = interNeighbourhoodGenerator(currentSolution);
                while(interNeighbourhoodIterator.move_next()){
                    co_yield interNeighbourhoodIterator.current_value();
                }
            }
        }

    }

    generator<vector<int>> interNeighbourhoodGenerator(vector<int> currentSolution){
        vector<int> neighbour = currentSolution;//copy solution neighbour only once to save time copying memory
        vector<pair<int, int>> moves;
    
        for (int i = 0; i < currentSolution.size(); i++) {
            for (int j = 0; j < costs.size(); j++) {
                if (!visited[j]) {
                    moves.push_back({i, j});
                }
            }
        }

        auto rng = std::default_random_engine {};
        std::shuffle(std::begin(moves), std::end(moves), rng);

        for (const auto& [i, j] : moves) {
            auto tmp = neighbour[i];
            neighbour[i] = j;
            co_yield neighbour;
            neighbour[i] = tmp; //reversing change to save time copying memory
        }
    }

    generator<vector<int>> intraNeighbourhoodGenerator(vector<int> currentSolution){
        if(intraNeighbourhoodType == twoNode) return intraNodesNeighbourhoodGenerator(currentSolution);
        return intraEdgesNeighbourhoodGenerator(currentSolution);
    }

    generator<vector<int>> intraNodesNeighbourhoodGenerator(vector<int> currentSolution){  // TODO: do not generate same neighbour twice (swap 1-2 and 2-1)
        vector<int> neighbour = currentSolution;//copy solution neighbour only once to save time copying memory
        vector<pair<int, int>> moves;
    
        for (int i = 0; i < currentSolution.size(); i++) {
            for(int j=0; j < currentSolution.size(); j++){
                if (i == j) continue;
                moves.push_back({i, j});
            }
        }

        auto rng = std::default_random_engine {};
        std::shuffle(std::begin(moves), std::end(moves), rng);

        for (const auto& [i, j] : moves) {
            auto tmp = neighbour[i];
            neighbour[i] = neighbour[j];
            neighbour[j] = tmp;
            co_yield neighbour;
            neighbour[j] = neighbour[i]; //reversing change to save time copying memory
            neighbour[i] = tmp;
        }
    }
    
    generator<vector<int>> intraEdgesNeighbourhoodGenerator(vector<int> currentSolution){
        vector<int> neighbour = currentSolution;//copy solution neighbour only once to save time copying memory
        vector<pair<pair<int, int>, pair<int, int>>> moves;
        for (int i = 0; i < neighbour.size(); i++) {
            for (int j = i + 1; j < neighbour.size(); j++) {
                if (i == j) continue;
                moves.push_back({{i, i + 1}, {j, j == currentSolution.size() - 1 ? 0 : j + 1}});
            }
        }
        auto rng = std::default_random_engine {};
        std::shuffle(std::begin(moves), std::end(moves), rng);
        
        for (const auto& [edge1, edge2] : moves) {
            if (edge1.second >= 0 && edge2.first < neighbour.size() && edge1.second < edge2.first) {
            reverse(neighbour.begin() + edge1.second, neighbour.begin() + edge2.first + 1);
            co_yield neighbour;
            reverse(neighbour.begin() + edge1.second, neighbour.begin() + edge2.first + 1);
            // to save time copying memory
            }
            else{
                throw "Incorrect edge indices";
            }
        }
    }
};

vector<vector<int>> read_file(string filename) {
    vector<vector<int>> result;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        vector<int> row;
        stringstream ss(line);
        string cell;
        while (getline(ss, cell, ';')) {
            row.push_back(stoi(cell));
        }
        result.push_back(row);
    }
    return result;
}


vector<vector<int>> calcDistances(vector<vector<int>> data){
    vector<vector<int>> distances;
    for (int i = 0; i < data.size(); i++){
        vector<int> row;
        for (int j = 0; j < data.size(); j++){
            int x = data[i][0] - data[j][0];
            int y = data[i][1] - data[j][1];
            //round to nearest int
            float distance = round(sqrt(x*x + y*y));
            row.push_back(distance);
        }
        distances.push_back(row);
    }
    return distances;
}


void write_solution_to_file(vector<int> sol, string algo_name, string data_name){
        cout << "Writing to: " << algo_name + "_"+ data_name + ".csv" << endl;
        string filename = "results/" + algo_name + "_"+ data_name + ".csv";
        ofstream file;
        file.open(filename);
        for(int i=0; i<sol.size(); i++){
            file << sol[i] << endl;
        }
        file.close();
    }



int main(){
    // srand(static_cast<unsigned>(time(0)));
    vector<Algo*> algorithms;
    algorithms.push_back(new Greedy2Regret());
    algorithms.push_back(new Greedy2RegretWieghted());

    string files[] = {"../data/TSPA.csv", "../data/TSPB.csv", "../data/TSPC.csv", "../data/TSPD.csv"};

    for(auto algo: algorithms){
        cout<<"#Algorithm: "<< algo->name << endl;
        for(string file: files){
            cout<<"##File: "<< file << endl;
            auto data = read_file(file);
            auto distances = calcDistances(data);
            vector<int> costs;
            for(int i=0; i< data.size(); i++){
                costs.push_back(data[i][2]);
            }
            auto result = algo->solve(distances, costs);
            cout << "Best cost: " << result.bestCost << endl;
            cout << "Worst cost: " << result.worstCost << endl;
            cout << "Average cost: " << result.averageCost << endl;
            cout << "Best solution: " << endl;
            for(int i=0; i<result.bestSolution.size(); i++){
                cout<<result.bestSolution[i]<<" ";
            }
            cout<<endl;
            // write_solution_to_file(result.bestSolution, algo->name, file.substr(file.size() - 8));
        }
    }
    return 0;
}