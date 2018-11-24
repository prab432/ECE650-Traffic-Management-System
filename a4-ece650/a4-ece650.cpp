#include <minisat/core/Solver.h>
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

std::vector<int> findVetexCover(int numVertices, int numVertexCover, std::vector<int> intVertices) {
    Minisat::Solver solver;

    std::vector <std::vector<Minisat::Lit>> matrix(numVertices);


    for (int n = 0; n < numVertices; ++n) {
        for (int k = 0; k < numVertexCover; ++k) {
            Minisat::Lit literal = Minisat::mkLit(solver.newVar());
            matrix[n].push_back(literal);
        }
    }

    // Rule 1: at least (exactly only one) one vertex is the ith vertex in the vertex cover, i in [1,k]
    for (int k = 0; k < numVertexCover; ++k) {
        Minisat::vec<Minisat::Lit> literals;
        for (int n = 0; n < numVertices; ++n) { // n is changing first
            literals.push(matrix[n][k]);
        }
        solver.addClause(literals);
        literals.clear();
    }

    // Rule 2: no one vertex can appear twice in a vertex cover
    for (int n = 0; n < numVertices; ++n) {
        for (int p = 0; p < numVertexCover - 1; ++p) {
            for (int q = p + 1; q < numVertexCover; ++q) {
                solver.addClause(~matrix[n][p], ~matrix[n][q]);
            }
        }
    }

    // Rule 3: no more than one vertex appears in the mth position of the vertex cover
    for (int m = 0; m < numVertexCover; ++m) {
        for (int p = 0; p < numVertices - 1; ++p) {
            for (int q = p + 1; q < numVertices; ++q) {
                solver.addClause(~matrix[p][m], ~matrix[q][m]);
            }
        }
    }

    std::vector<int> former;
    std::vector<int> latter;

    for (int i = 0; i < intVertices.size(); ++i) {
        if (i % 2 == 0) {
            former.push_back(intVertices[i]);
        } else {
            latter.push_back(intVertices[i]);
        }
    }

    //Rule 4: every edge is incident to at least one vertex in the vertex cover
    for (int i = 0; i < former.size(); ++i) {
        Minisat::vec<Minisat::Lit> literals;
        for (int k = 0; k < numVertexCover; ++k) {
            literals.push(matrix[former[i]][k]);
            literals.push(matrix[latter[i]][k]);
        }
        solver.addClause(literals);
        literals.clear();
    }

    auto sat = solver.solve();

    if (sat) {
        std::vector<int> result;

        for (int i = 0; i < numVertices; i++) {
            for (int j = 0; j < numVertexCover; ++j) {
                if (Minisat::toInt(solver.modelValue(matrix[i][j])) == 0) {
                    result.push_back(i);
                }
            }
        }
        return result;


    } else {
        return {-1};
    }

}

// std::vector<int> buildGraph(std::string allVertice, int num, Graph graph) {
void buildGraph(std::string allVertice, int num, Graph graph) {
    std::string edges;
    bool flag = true;
    edges = replaceStrChar(allVertice, "<", ' ');
    edges = replaceStrChar(edges, ">", ' ');
    edges = replaceStrChar(edges, "{", ' ');
    edges = replaceStrChar(edges, "}", ' ');
    edges.erase(remove_if(edges.begin(), edges.end(), isspace), edges.end());

    std::vector<std::string> vertices = split(edges, ","); // ==> ["1","2","3"]

    std::vector<int> intVertices;

    for (auto &s : vertices) {
        std::stringstream parser(s);
        int x = 0;

        parser >> x;

        intVertices.push_back(x);
    }

    for (unsigned index = 0; index < vertices.size(); ++index) {
        if (index % 2 == 0) {
            if ((std::stoi(vertices[index]) >= num) || (std::stoi(vertices[index + 1]) >= num)) {
                std::cerr << "Error: vertice ID is larger than the size of graph" << std::endl;
                flag = false;
            } else {
                graph.addEdge(std::stoi(vertices[index]), std::stoi(vertices[index + 1]));
            }

        } else {
            continue;
        }
    }

    if (flag) {
        int numVertices = num;

        std::vector<int> result;
        std::vector<int> finalResult;

        std::vector<int> tmp;
        tmp = {-1};

        int lo = 1;
        int hi = numVertices;

        while (lo <= hi) {

            int mid = floor((lo + hi) / 2);

            result = findVetexCover(numVertices, mid, intVertices);

            bool checkResult = std::equal(result.begin(), result.end(), tmp.begin());

            if (not checkResult) { // can find the vertex cover, reduce the mid to try
                hi = mid - 1;
                finalResult.clear();
                finalResult = result;
            } else {
                lo = mid + 1;
            }
        }

        sort(finalResult.begin(), finalResult.end());

        for (int i = 0; i < finalResult.size(); ++i) {
            std::cout << finalResult[i] << " ";
        }
        std::cout << "\n";
    }

}

int main() {
    Graph g(0);
    using Minisat::mkLit;
    using Minisat::lbool;
    int num = 0;

    while (!std::cin.eof()) {
        std::vector<std::string> tokens;

        std::string input_string;
        std::getline(std::cin, input_string);

        tokens = split(input_string, " ");


        if (tokens.size() == 2) {
            if (tokens[0] == "V") {
                num = std::stoi(tokens[1]);
                g = Graph(num);
            } else if (tokens[0] == "E") {
                buildGraph(tokens[1], num, g);
            } else {
                std::cout << "invalid input!" << std::endl;
            }
        } else {
            continue;
        }

    }
    return 0;
}