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

int findFrequentVertex(std::vector<int> intVertices) {
    // std::vector<int> v = {0,1,2,2,3,3,3,4};

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

    //std::cout << "most common is: " << mostCommon << std::endl;
    return mostCommon;
}

typedef struct _thread_data {
    int num;
    std::vector<int> vertex;
} thread_data;

//APPROX-VC-1
void approxVC1(std::vector<int> intVertices) {
    //std::vector<int> intVertices = {0,1,0,2,0,4,0,3,4,5};

    std::vector<int> intVerticesCopy = intVertices;

    std::vector<int> result;

    while(!intVerticesCopy.empty()) {
        int mostValue = findFrequentVertex(intVertices);

        result.push_back(mostValue);
        //std::cout << "mostValue is: " << mostValue;
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

    // std::cout << "algo 1 result is: " << std::endl;

    sort(result.begin(), result.end());

    std::string finalResult = "APPROX-VC-1: ";
    for (unsigned i = 0; i < result.size(); ++i) {
        if (i != result.size() - 1) {
            finalResult.append(std::to_string(result[i]));
            finalResult.append(",");
        } else {
            finalResult.append(std::to_string(result[i]));
        }
        // std::cout << result[i] << std::endl;
    }
    finalResult.append("\n");
    std::cout << finalResult;
    // pthread_exit();
    // return result;
}

//APPROX-VC-2
void approxVC2(std::vector<int> intVertices) {
    // std::vector<int> intVertices = {0,1,0,2,0,4,0,3,4,5};

    std::vector<int> intVerticesCopy = intVertices;

    std::vector<int> result;

    while(!intVerticesCopy.empty()) {
        int u = intVerticesCopy[0];
        int v = intVerticesCopy[1];

        result.push_back(u);
        result.push_back(v);

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

    std::string finalResult = "APPROX-VC-2: ";

    sort(result.begin(), result.end());

    for (unsigned i = 0; i < result.size(); ++i) {
        if (i != result.size() - 1) {
            finalResult.append(std::to_string(result[i]));
            finalResult.append(",");
        } else {
            finalResult.append(std::to_string(result[i]));
        }
        // std::cout << result[i] << std::endl;
    }
    finalResult.append("\n");
    std::cout << finalResult;
    //pthread_exit();
    //return result;
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

//CNF final result
void approxCNF(int num, std::vector<int> intVertices) {

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

    std::string res = "CNF-SAT-VC: ";
    for (unsigned i = 0; i < finalResult.size(); ++i) {
        if (i != finalResult.size() - 1) {
            res.append(std::to_string(finalResult[i]));
            res.append(",");
        } else {
            res.append(std::to_string(finalResult[i]));
        }
        // std::cout << result[i] << std::endl;
    }
    res.append("\n");
    std::cout << res;
    // pthread_exit();
}

void *threadCNF(void *arg) {
    thread_data *data = (thread_data *)arg;
    approxCNF(data->num, data->vertex);
    pthread_exit(NULL);
}

void *threadVC1(void *arg) {
    thread_data *data = (thread_data *)arg;
    approxVC1(data->vertex);
    pthread_exit(NULL);
}

void *threadVC2(void *arg) {
    thread_data *data = (thread_data *)arg;
    approxVC2(data->vertex);
    pthread_exit(NULL);
}




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
        pthread_t th1;
        pthread_t th2;
        pthread_t th3;

        thread_data thr_data;

        //std::vector<int> vertices{1,2,3,4};

        thr_data.num = num;
        thr_data.vertex = intVertices;

        pthread_create(&th1, NULL, threadCNF, &thr_data);
        pthread_create(&th2, NULL, threadVC1, &thr_data);
        pthread_create(&th3, NULL, threadVC2, &thr_data);


        pthread_join(th1, NULL);
        pthread_join(th2, NULL);
        pthread_join(th3, NULL);

        //pthread_t thread;
        clockid_t cid;
        int j, s;

        //s = pthread_create(&thread, NULL, thread_start, NULL);
        //if (s != 0)
            //handle_error_en(s, "pthread_create");

        //printf("Main thread sleeping\n");
        //sleep(1);

        //printf("Main thread consuming some CPU time...\n");
        //for (j = 0; j < 2000000; j++)
            //getppid();

        //pclock("Process total CPU time: ", CLOCK_PROCESS_CPUTIME_ID);

        s = pthread_getcpuclockid(pthread_self(), &cid);
        if (s != 0)
            handle_error_en(s, "pthread_getcpuclockid");
        pclock("Main thread CPU time:   ", cid);


        s = pthread_getcpuclockid(th1, &cid);
        if (s != 0)
            handle_error_en(s, "pthread_getcpuclockid");
        pclock("Thread 1's running time is:        ", cid);

        s = pthread_getcpuclockid(th2, &cid);
        if (s != 0)
            handle_error_en(s, "pthread_getcpuclockid");
        pclock("Thread 2's running time is:        ", cid);

        s = pthread_getcpuclockid(th3, &cid);
        if (s != 0)
            handle_error_en(s, "pthread_getcpuclockid");
        pclock("Thread 3's running time is:        ", cid);

        // exit(EXIT_SUCCESS);

        /*
        pthread_create(&th1, NULL, approxCNF, num, intVertices);
        pthread_create(&th2, NULL, approxVC1, &intVertices);
        pthread_create(&th3, NULL, approxVC2, &intVertices);


        int s1 = getcpuclockid(th1, &cid);
        std::cout << "result of s for th1 is: " << s1 << std::endl;

        int s2 = pthread_getcpuclockid(th2, &cid);
        std::cout << "result of s for th2 is: " << s2 << std::endl;

        int s3 = pthread_getcpuclockid(th3, &cid);
        std::cout << "result of s for th3 is: " << s3 << std::endl;


        pthread_join(&th1, NULL);
        pthread_join(&th2, NULL);
        pthread_join(&th3, NULL);
        std::cout << "i is: " << i << std::endl;
        */
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