#include <string>
#include <stdlib.h>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <vector>

using namespace std;

int rgen(int argc, char **argv) {
    argv[0] = (char*) "rgen";
    int mes = execv("rgen", argv);
    if(mes == -1) {
        std::cerr << "Error: from executing rgen" << std::endl;
    }
    return 0;
}

int a1(void) {
    char* argv[3];
    argv[0] = (char*) "python";
    argv[1] = (char*) "a1-ece650.py";
    argv[2] = nullptr;

    int mes = execvp("python", argv);
    if (mes == -1) {
        std::cerr << "Error: from executing a1 file" << std::endl;
    }
    return 0;
}

int a2(void) {
    char* argv2[2];
    argv2[0] = (char*) "a2-ece650";
    argv2[1] = nullptr;
    int mes = execv("a2-ece650", argv2);
    if(mes == -1) {
        std::cerr << "Error: from executing a2 file" << std::endl;
    }
    return 0;

}

int type(void) {
    while (!std::cin.eof()) {
        string line;
        getline(std::cin, line);
        if (line.size() > 0) {
            std::cout << line << std::endl;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    std::vector<pid_t> kids;
    int RgentoA1[2];
    pipe(RgentoA1);

    int A1toA2[2];
    pipe(A1toA2);

    pid_t child_pid;
    child_pid = fork();
    
    if (child_pid == 0) {
        dup2(RgentoA1[1], STDOUT_FILENO);
        close(RgentoA1[0]);
        close(RgentoA1[1]);
        return rgen(argc, argv);

    }else if (child_pid < 0) {
        std::cerr << "Error: could not fork" << std::endl;
        return 1;
    }

    kids.push_back(child_pid);
    child_pid = fork();

    if (child_pid == 0) {
        dup2(RgentoA1[0], STDIN_FILENO);
        close(RgentoA1[0]);
        close(RgentoA1[1]);

        dup2(A1toA2[1], STDOUT_FILENO);
        close(A1toA2[0]);
        close(A1toA2[1]);
        return a1();

    }else if (child_pid < 0) {
        std::cerr << "Error: could not fork" << std::endl;
        return 1;
    }

    kids.push_back(child_pid);
    child_pid = fork();

    if (child_pid == 0) {
        dup2(A1toA2[0], STDIN_FILENO);
        close(A1toA2[1]);
        close(A1toA2[0]);
        return a2();

    }else if (child_pid < 0) {
        std::cerr << "Error: could not fork" << std::endl;
        return 1;
    }

    kids.push_back(child_pid);
    child_pid = 0;

    dup2(A1toA2[1], STDOUT_FILENO);
    close(A1toA2[0]);
    close(A1toA2[1]);

    int res = type();
    
    for (pid_t k : kids) {
        int status;
        kill (k, SIGTERM);
        waitpid(k, &status, 0);
    }

    return res;
}