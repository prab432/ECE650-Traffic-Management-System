#include <iostream> 
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <random>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <algorithm>
  
struct Point 
{ 
    int x; 
    int y; 
}; 
 
bool onSegment(Point p, Point q, Point r) 
{ 
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && 
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y)) 
       return true; 
  
    return false; 
} 

int orientation(Point p, Point q, Point r) 
{ 
    int val = (q.y - p.y) * (r.x - q.x) - 
              (q.x - p.x) * (r.y - q.y); 
  
    if (val == 0) return 0;  // colinear 
    return (val > 0)? 1: 2; // clock or counterclock wise 
} 

bool doIntersect(Point p1, Point q1, Point p2, Point q2) 
{ 
    int o1 = orientation(p1, q1, p2); 
    int o2 = orientation(p1, q1, q2); 
    int o3 = orientation(p2, q2, p1); 
    int o4 = orientation(p2, q2, q1); 

    if (o1 != o2 && o3 != o4) 
        return true; 
  
    // Special Cases 
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1 
    if (o1 == 0 && onSegment(p1, p2, q1)) return true; 
  
    // p1, q1 and q2 are colinear and q2 lies on segment p1q1 
    if (o2 == 0 && onSegment(p1, q2, q1)) return true; 
  
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2 
    if (o3 == 0 && onSegment(p2, p1, q2)) return true; 
  
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2 
    if (o4 == 0 && onSegment(p2, q1, q2)) return true; 
  
    return false; // Doesn't fall in any of the above cases 
}

bool checkSegOverlap(Point p1, Point q1, Point p2, Point q2)
{
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    std::vector<int> x1;
    std::vector<int> x2;

    if (p1.x == p2.x && p2.x == q1.x && q1.x == q2.x) {
        x1.push_back(p1.y);
        x1.push_back(q1.y);
        x2.push_back(p2.y);
        x2.push_back(q2.y);

    } else {
        x1.push_back(p1.x);
        x1.push_back(q1.x);
        x2.push_back(p2.x);
        x2.push_back(q2.x);
    }
    sort(x1.begin(), x1.end());
    sort(x2.begin(), x2.end());

    if (o1 == 0 && o2 == 0 && o3 == 0 && o4 == 0) {
        if ((x1[0] >= x2[1] && x2[0] <= x1[0]) || (x2[0] >= x1[1] && x1[0] <= x2[0])) {
            return false; // no overlap
        }
        return true; // there is overlap
    }
    return false;
}

bool checkIntersection(std::vector<int> xvalue, std::vector<int> yvalue) {
    // return True: no intersection, data valid
    // return True: data valid, no same point
    
    for (unsigned i=1; i<xvalue.size(); i++) {
        if (xvalue[i] == xvalue[i-1] && yvalue[i] == yvalue[i-1]) {
            return false;
        } else {
            continue;
        }
    }

    struct Point p1, q1, p2, q2;
    for (unsigned i=0;i<xvalue.size()-1;i++) {
        
        p1 = {xvalue[i], yvalue[i]};
        q1 = {xvalue[i+1], yvalue[i+1]}; 
        
        for (unsigned j=0;j<i;j++){
            p2 = {xvalue[j], yvalue[j]};
            q2 = {xvalue[j+1], yvalue[j+1]}; 
    
            if (doIntersect(p1, q1, p2, q2) && !checkSegOverlap(p1, q1, p2, q2)) { // q2 != p1
                if (xvalue[j+1] != xvalue[i] && yvalue[j+1] != yvalue[i]) {
                    return false;
                }else {
                    continue;
                    }
            } else{
                continue;
            }
        }
    }
    return true;
}



bool checkOverlap(std::vector<int> xvalue, std::vector<int> yvalue, std::vector< std::vector<int> > allxvalue, std::vector< std::vector<int> > allyvalue) {
    for (unsigned p=1; p<xvalue.size(); p++) {

        struct Point p1 = {xvalue[p], yvalue[p]}, q1 = {xvalue[p-1], yvalue[p-1]};

        for (unsigned i = 0; i < allxvalue.size(); i++) {

            for (unsigned j = 1; j < allxvalue[i].size(); j++) {
                struct Point p2 = {allxvalue[i][j], allyvalue[i][j]}, q2 = {allxvalue[i][j-1], allyvalue[i][j-1]};
                if (checkSegOverlap(p1, q1, p2, q2)) {
                    // yes, there is overlap, invalid line segment, return
                    return true;
                }
            }
        }
    }
    return false;

}

int randomInt() {
    std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);

    // check that it did not fail
    if (urandom.fail()) {
        std::cerr << "Error: cannot open /dev/urandom\n";
        return 1;
    }

    // read a random unsigned int
    unsigned int num = 0;
    unsigned int size = 1; // if 0, 0, if 1, max 256, if 2, max 65536

    urandom.read((char *) &num, size);

    // close random stream
    urandom.close();
    return num;
}

std::vector<int> generator(int lineSegNum, int valueRange, std::vector< std::vector<int> > allxvalue, std::vector< std::vector<int> > allyvalue) {
    
    int countLineSeg = 0;
    
    std::vector<int> streetInfo; 
    int vertex;
    while (countLineSeg < (lineSegNum + 1) * 2) {
        vertex = randomInt(); // this is coordinators not the id of vertex
        vertex = floor(vertex * (2 * valueRange) / 255) - valueRange;

        if (streetInfo.size() > 0){
            while (vertex == streetInfo.back()){
                vertex = randomInt();
                vertex = floor(vertex * (2 * valueRange) / 255) - valueRange;
            }
        }
        streetInfo.push_back(vertex);
        countLineSeg++;
    }
    
    std::vector<int> xvalue;
    std::vector<int> yvalue;
    
    int numTryGenerate = 0;
    
    while(numTryGenerate < 25) {

        numTryGenerate++;
        
        for(unsigned i=0; i<streetInfo.size(); i++) {
            if (i%2==0) {
                xvalue.push_back(streetInfo[i]);
            }else{
                yvalue.push_back(streetInfo[i]);
            } 
        }  
        // checkOverlap invalid return true
        if (checkIntersection(xvalue, yvalue) && !checkOverlap(xvalue, yvalue, allxvalue, allyvalue)) { // no intersection and no overlap
            return streetInfo;
        } else {
            continue;
        }
    }
    
    if(numTryGenerate >= 25) {
        std::cerr << "Error: failed to generate valid input for 25 simultaneous attemps" << std::endl;
    }
    
    return streetInfo; 
}

std::string random_string(std::string::size_type length)
{
    static auto& chrs = "abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{std::random_device{}()};
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string s;

    s.reserve(length);

    while(length--)
        s += chrs[pick(rg)];

    return s;
}

int main(int argc, char **argv) {

    int streetNum = 10; // s
    int lineSegNum = 5; // n
    int valueRange = 20; // c
    int waitSecond = 5; // l  why do i need to wait for a few seconds?

    int index;
    int c;
    opterr = 0;

    while ((c = getopt(argc, argv, "s:n:c:l:")) != -1)
        switch (c) {
            case 's':
                streetNum = std::atoi(optarg);
                break;
            case 'n':
                lineSegNum = std::atoi(optarg);
                break;
            case 'c':
                valueRange = std::atoi(optarg);
                break;
            case 'l':
                waitSecond = std::atoi(optarg);
                break;
            default:
                return 0;
        }

    if (optind < argc) {
        std::cout << "Found positional arguments\n";
        for (index = optind; index < argc; index++)
            std::cout << "Non-option argument: " << argv[index] << "\n";
    }

    int count = 0;
    std::string streetName;

    std::vector<std::string> allStreetName;
    std::string res;
    std::vector<int> xvalue;
    std::vector<int> yvalue;
    std::vector<int> streetInfo;

    while (true) {
        int realStreetNum = randomInt(); // don't know if we need to check if streetNum is larger or equal to 2?
        realStreetNum = floor(realStreetNum * (streetNum - 2) / 255) + 2;

        int realWaitSecond = randomInt();
        realWaitSecond = floor(realWaitSecond * (waitSecond - 5) / 255) + 5;

        std::vector< std::vector<int> > allxvalue;
        std::vector< std::vector<int> > allyvalue;

        while (count < realStreetNum) {

            int realLineSegNum = randomInt();
            realLineSegNum = floor(realLineSegNum * (lineSegNum - 1) / 255) + 1;

            streetInfo = generator(realLineSegNum, valueRange, allxvalue, allyvalue);

            for (unsigned i = 0; i < streetInfo.size(); i++) {
                if (i % 2 == 0) {
                    xvalue.push_back(streetInfo[i]);
                } else {
                    yvalue.push_back(streetInfo[i]);
                }
            }

            for (unsigned i = 0; i < xvalue.size(); i++) {
                res.append("(");
                res.append(std::to_string(xvalue[i]));
                res.append(",");
                res.append(std::to_string(yvalue[i]));
                if (i != xvalue.size() - 1) {
                    res.append(") ");
                } else {
                    res.append(")");
                }
            }

            streetName = random_string(5);
            allStreetName.push_back(streetName);

            streetName = "\"" + streetName + "\" ";
            std::cout << "a " << streetName << res << std::endl;

            res.clear();

            allxvalue.push_back(xvalue);
            allyvalue.push_back(yvalue);

            xvalue.clear();
            yvalue.clear();
            streetInfo.clear();

            count++;
        }

        std::cout << "g" << std::endl;

        sleep(realWaitSecond);
        // need to remove the dic in python

        for (unsigned i = 0; i < allStreetName.size(); i++) {
            std::cout << "r " << "\"" << allStreetName[i] << "\"" << std::endl;
        }

        allStreetName.clear();
        count = 0;
    }
    return 0;
}