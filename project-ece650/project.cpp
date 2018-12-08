#include <minisat/core/Solver.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <list>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <map>
#include <set>

int printFlag = 0;
int countVec = 0;

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static void pclock(char *msg, clockid_t cid)
{
    struct timespec ts;

    printf("%s", msg);
    if (clock_gettime(cid, &ts) == -1)
        handle_error("clock_gettime");

    printf("%4ld.%03ld\n", ts.tv_sec, ts.tv_nsec / 1000000);
}

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

int findFrequentVertex(std::vector<int> intVertices) {

    int max = 0;
    int mostCommon = -1;

    std::map<int,int> m;

    for (auto vi = intVertices.begin(); vi != intVertices.end(); vi++) {
        m[*vi]++;
        if (m[*vi] > max) {
            max = m[*vi];
            mostCommon = *vi;
        }
    }
    return mostCommon;
}

class Graph
{

public:
    int V;
    int VEmpFlag = 0;
    int EEmpFlag = 0;

    std::list<int> *adj; // Pointer to an array containing adjacency lists
    std::vector<int> vertex;
    void addEdge(int v, int w); // function to add an edge to graph
    std::vector<int> buildGraph(std::string allVertice, int num);
    void approxCNF();
    void approxVC1();
    void approxVC2();
    void init(int V);
    void printRes();
    bool isEmpty();

    std::vector<int> resultVC1;
    std::vector<int> resultVC2;
    std::vector<int> resultCNF;

};

void Graph::init(int V) {
    adj = new std::list<int> [V];
}

void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w);
    adj[w].push_back(v);
}

bool Graph::isEmpty()
{
    if(this->EEmpFlag && this->VEmpFlag)
    {
        return false;
    }
    return true;
}

std::vector<int> Graph::buildGraph(std::string allVertice, int num) {
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

    this->vertex = intVertices;

    for (unsigned index = 0; index < vertices.size(); ++index) {
        if (index % 2 == 0) {
            if ((std::stoi(vertices[index]) >= num) || (std::stoi(vertices[index + 1]) >= num)) {
                std::cerr << "Error: vertice ID is larger than the size of graph" << std::endl;
                flag = false;
            } else {
                addEdge(std::stoi(vertices[index]), std::stoi(vertices[index + 1]));
            }

        } else {
            continue;
        }
    }
    return intVertices;
}

struct thread_data{
    Graph graph;
};

//APPROX-VC-1
void Graph::approxVC1() {
    std::vector<int> intVertices = vertex;

    std::vector<int> intVerticesCopy = intVertices;

    while(!intVerticesCopy.empty()) {
        int mostValue = findFrequentVertex(intVertices);

        this->resultVC1.push_back(mostValue);
        intVertices.clear();

        for (unsigned i = 0; i < intVerticesCopy.size() - 1; ++i) {
            if (i % 2 == 0) {
                if (intVerticesCopy[i] == mostValue || intVerticesCopy[i+1] == mostValue) {
                    continue;
                }else {
                    intVertices.push_back(intVerticesCopy[i]);
                    intVertices.push_back(intVerticesCopy[i+1]);
                }
            }else {
                continue;
            }
        }
        intVerticesCopy = intVertices;
    }

    sort(this->resultVC1.begin(), this->resultVC1.end());
    countVec++;
}

//APPROX-VC-2
void Graph::approxVC2() {
    std::vector<int> intVertices = vertex;

    std::vector<int> intVerticesCopy = intVertices;

    while(!intVerticesCopy.empty()) {
        int u = intVerticesCopy[0];
        int v = intVerticesCopy[1];

        this->resultVC2.push_back(u);
        this->resultVC2.push_back(v);

        intVertices.clear();

        for (unsigned i = 0; i < intVerticesCopy.size() - 1; ++i) {
            if (i % 2 == 0) {
                if (intVerticesCopy[i] == u || intVerticesCopy[i+1] == v || intVerticesCopy[i] == v || intVerticesCopy[i+1] == u) {
                    continue;
                }else {
                    intVertices.push_back(intVerticesCopy[i]);
                    intVertices.push_back(intVerticesCopy[i+1]);
                }
            }else {
                continue;
            }
        }
        intVerticesCopy = intVertices;
    }
    sort(resultVC2.begin(), resultVC2.end());
    countVec++;
}

// REDUCTION-TO-CNF-SAT
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

    for (unsigned i = 0; i < intVertices.size(); ++i) {
        if (i % 2 == 0) {
            former.push_back(intVertices[i]);
        } else {
            latter.push_back(intVertices[i]);
        }
    }

    //Rule 4: every edge is incident to at least one vertex in the vertex cover
    for (unsigned i = 0; i < former.size(); ++i) {
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

        for (unsigned i = 0; i < numVertices; i++) {
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

//CNF final result
void Graph::approxCNF() {
    std::vector<int> intVertices = vertex;
    int numVertices = this->V;
    std::vector<int> result;
    std::vector<int> finalResult;

    std::vector<int> tmp;
    tmp = {-1};

    int lo = 1;
    int hi = numVertices;

    while (lo <= hi && hi <= 50) {
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

    for (unsigned i = 0; i < finalResult.size(); i++) {
        this->resultCNF.push_back(finalResult[i]);
    }

    sort(this->resultCNF.begin(), this->resultCNF.end());
    countVec++;
}

void Graph::printRes() {
    std::string resCNF = "CNF-SAT-VC: ";
    std::string resVC1 = "APPROX-VC-1: ";
    std::string resVC2 = "APPROX-VC-2: ";

    for (unsigned i = 0; i < this->resultCNF.size(); i++) {
        if (i == this->resultCNF.size() - 1) {
            resCNF.append(std::to_string(this->resultCNF[i]));
        }else {
            resCNF.append(std::to_string(this->resultCNF[i]));
            resCNF.append(",");
        }
    }

    for (unsigned i = 0; i < this->resultVC1.size(); i++) {
        if (i == this->resultVC1.size() - 1) {
            resVC1.append(std::to_string(this->resultVC1[i]));
        }else {
            resVC1.append(std::to_string(this->resultVC1[i]));
            resVC1.append(",");
        }
    }

    for (unsigned i = 0; i < this->resultVC2.size(); i++) {
        if (i == this->resultVC2.size() - 1) {
            resVC2.append(std::to_string(this->resultVC2[i]));
        }else {
            resVC2.append(std::to_string(this->resultVC2[i]));
            resVC2.append(",");
        }
    }

    std::cout << resCNF << std::endl;
    std::cout << resVC1 << std::endl;
    std::cout << resVC2 << std::endl;

    this->resultCNF.clear();
    this->resultVC1.clear();
    this->resultVC2.clear();
}

void *threadCNF(void *arg) {

    struct thread_data *g = (struct thread_data *)arg;
    if(g->graph.isEmpty())
    {
        return NULL;
    }
    g->graph.approxCNF();
    return  NULL;
}

void *threadVC1(void *arg) {
    struct thread_data *g = (struct thread_data *)arg;
    if(g->graph.isEmpty())
    {
        return NULL;
    }
    g->graph.approxVC1();
    return  NULL;
}

void *threadVC2(void *arg) {
    struct thread_data *g = (struct thread_data *)arg;
    if(g->graph.isEmpty())
    {
        return NULL;
    }
    g->graph.approxVC2();
    return  NULL;
}

void *threadIO(void *arg) {
    struct thread_data *g = (struct thread_data *)arg;
    using Minisat::mkLit;
    using Minisat::lbool;
    int num = 0;
    int flag = 0;
    int Vflag= 0;
    int Eflag = 0;
    /*
    if (printFlag != 0) {
        g->graph.printRes();
    }
     */

    if(!g->graph.isEmpty())
    {
        while(true)
        {
            if(countVec == 3)
            {
                countVec = 0;
                break;
            }
        }
        g->graph.printRes();
    }

    while (flag == 0) {
        if(std::cin.eof())
        {
            exit(0);
        }

        while (!std::cin.eof()) {

            std::vector <std::string> tokens;

            std::string input_string;
            std::getline(std::cin, input_string);

            tokens = split(input_string, " ");


            if (tokens.size() == 2) {
                if (tokens[0] == "V") {
                    num = std::stoi(tokens[1]);
                    g->graph.init(num);
                    Vflag = 1;
                    g->graph.V = num;
                    g->graph.VEmpFlag = 1;
                } else if (tokens[0] == "E") {
                    g->graph.vertex = g->graph.buildGraph(tokens[1], num);
                    Eflag = 1;
                    printFlag = 1;
                    g->graph.EEmpFlag = 1;

                } else {
                    std::cout << "invalid input!" << std::endl;
                }
            } else {
                continue;
            }

            if (Vflag == 1 && Eflag == 1) {
                flag = 1;
                break;
            }

        }
    }

    return  NULL;

}

int main(int argc, char** argv) {
    struct thread_data graph;
    int status;
    clockid_t cid;
    int j, s;

    pthread_t thIO;
    pthread_t thCNF;
    pthread_t thVC1;
    pthread_t thVC2;

    while (true) {

        status = pthread_create(&thIO, NULL, threadIO, &graph);
        if (status != 0) {
            std::cerr << "Error when creating thread IO!" << std::endl;
        }

        status = pthread_create(&thCNF, NULL, threadCNF, &graph);
        if (status != 0) {
            std::cerr << "Error when creating thread CNF!" << std::endl;
        }

        status = pthread_create(&thVC1, NULL, threadVC1, &graph);
        if (status != 0) {
            std::cerr << "Error when creating thread VC1!" << std::endl;
        }

        status = pthread_create(&thVC2, NULL, threadVC2, &graph);
        if (status != 0) {
            std::cerr << "Error when creating thread VC2!" << std::endl;
        }

       /* 
        s = pthread_getcpuclockid(pthread_self(), &cid);
        // std::cout << "pthread cid is: " << cid << std::endl;
        
        if (s != 0)
            handle_error_en(s, "pthread_getcpuclockid");
        pclock("thread main CPU time:    ", cid);
        */
        s = pthread_getcpuclockid(thCNF, &cid);
        // std::cout << "thvc1 cid is: " << cid << std::endl;
        
        if (s != 0) {
            std::cout << "error with thCNF" << std::endl;
	    continue;
            // handle_error_en(s, "pthread_getcpuclockid");
        }else{
            pclock("thread CNF CPU time:    ", cid);
        }
       
        s = pthread_getcpuclockid(thVC1, &cid);
        // std::cout << "thcnf cid is: " << cid << std::endl;
        
        if (s != 0) {
	    std::cout << "error with thVC1" << std::endl;
	    continue;
            // handle_error_en(s, "pthread_getcpuclockid");
        }else{
            pclock("thread VC1 CPU time:    ", cid);
        }

        s = pthread_getcpuclockid(thVC2, &cid);
        // std::cout << "thcnf cid is: " << cid << std::endl;
        //         
        if (s != 0) {
	    std::cout << "error with thvc2" << std::endl;
	    continue;
            // handle_error_en(s, "pthread_getcpuclockid");
        }else{
	    pclock("thread VC2 CPU time:    ", cid);
        }
        pthread_join(thIO, NULL);
        pthread_join(thCNF, NULL);
        pthread_join(thVC1, NULL);
        pthread_join(thVC2, NULL);

    }
    return 0;
}
