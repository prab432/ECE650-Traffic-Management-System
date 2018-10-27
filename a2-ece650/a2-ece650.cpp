#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <list>

// for split the string into tokens with delimeter
std::vector<std::string> split(std::string str, std::string token){
    std::vector<std::string>result;
    while(str.size()){
        int index = str.find(token);
        if(index!=std::string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

// replace specific character into a specific character
std::string replaceStrChar(std::string str, const std::string& replace, char ch) {
    size_t found = str.find_first_of(replace);

    while (found != std::string::npos) {
        str[found] = ch;
        found = str.find_first_of(replace, found+1);
    }
    return str;
}

class Graph
{
    int V;
public:
    std::list<int> *adj; // Pointer to an array containing adjacency lists
    Graph(int V); // Constructor
    void addEdge(int v, int w); // function to add an edge to graph
    std::list<int> shortestPath(int s, int e);
};

Graph::Graph(int V)
{
    this->V = V;
    adj = new std::list<int> [V];
}

void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w);
    adj[w].push_back(v);
}

std::list<int> Graph::shortestPath(int s, int e)
{
    bool *visited = new bool[V];
    std::fill_n(visited, V, 0);

    std::list<int> result;
    std::list<int> path;
    std::list<int> new_path;
    std::list<int> neighbours;

    path.push_back(s);

    std::vector<std::list<int> > queue;
    int node;
    int tmp;

    if (s == e)
        return result;

    queue.push_back(path);

    std::list<int>::iterator i;

    while (!queue.empty()) // queue is a vector
    {
        path = queue.front();
        queue.erase(queue.begin());

        node = path.back();

        if (!visited[node])
        {
            neighbours = adj[node];

            for (auto& neighbour : neighbours)
            {
                new_path = path;
                tmp = neighbour;
                new_path.push_back(tmp);
                queue.push_back(new_path);

                if (tmp == e)
                {
                    return new_path;
                }
            }
            visited[node] = 1;
        }
    }
    return result;
}

void buildGraph(std::string allVertice, int num, Graph graph) {
    std::string edges;
    edges = replaceStrChar(allVertice, "<", ' ');
    edges = replaceStrChar(edges, ">", ' ');
    edges = replaceStrChar(edges, "{", ' ');
    edges = replaceStrChar(edges, "}", ' ');
    edges.erase(remove_if(edges.begin(), edges.end(), isspace), edges.end());
    
    std::vector<std::string> vertices = split(edges, ","); // ==> ["1","2","3"]

    for (int index = 0; index < vertices.size(); ++index) {
        if (index % 2 == 0) {
            if ((std::stoi(vertices[index]) >= num) || (std::stoi(vertices[index + 1]) >= num)) {
                std::cout << "Error: vertice ID is larger than the size of graph" << std::endl;
            } else {
                    graph.addEdge(std::stoi(vertices[index]), std::stoi(vertices[index + 1]));
                }

        } else {
                continue;
            }
        }
}

void generatePath(std::vector<std::string> tokens, Graph g) {
    std::list<int> result_path;
    std::list<int>::const_iterator iterator;
    int start;
    int end;
    
    start = std::stoi(tokens[1]);
    end = std::stoi(tokens[2]);
    result_path = g.shortestPath(start, end);

    
    for (iterator = result_path.begin(); iterator != result_path.end(); ++iterator) {
        if (iterator != result_path.begin()) {
            std::cout << "-" << *iterator;
        } else {
            std::cout << *iterator;
            }
    }
    std::cout << "\n";
            
    if (result_path.empty()) {
        std::cout << "Error: no path exists between these two vertices or vertex does not exist" << std::endl;
    }    
}

int main() {
    Graph g(0);
    
    // crtl + c to exist the program
    while (std::cin) {
        char input[256];
        std::string input_string;
        std::vector<std::string> tokens;
        int num;
        
        std::list<int> mylist;

        std::cin.getline(input, 256);

        input_string = std::string(input); // convert input into string

        tokens = split(input_string, " ");

        if (tokens.size() == 2) {
            if (tokens[0] == "V") {
                num = std::stoi(tokens[1]);
                g = Graph(num);
            } else if (tokens[0] == "E") {
                
                buildGraph(tokens[1], num, g);
            } else {
                std::cout << "Error: invalid input!" << std::endl;
            }

        } else if (tokens.size() == 3 && tokens[0] == "s") {
            generatePath(tokens, g);
        } else {
            std::cout << "Error: invalid input!" << std::endl;
        }
    }
    return 0;
}
