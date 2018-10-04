#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Sep 27 14:32:11 2018

@author: yumi.zhang
"""

import sys
import re

# YOUR CODE GOES HERE

class Point(object):
    def __init__ (self, x, y):
        self.x = float(x)
        self.y = float(y)
    def __str__ (self):
        return '(' + str(self.x) + ',' + str(self.y) + ')'

class Line(object):
    def __init__ (self, src, dst):
        self.src = src
        self.dst = dst

    def __str__(self):
        return str(self.src) + '-->' + str(self.dst)

def intersect (l1, l2):
    x1, y1 = l1.src.x, l1.src.y
    x2, y2 = l1.dst.x, l1.dst.y
    x3, y3 = l2.src.x, l2.src.y
    x4, y4 = l2.dst.x, l2.dst.y

    xnum = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4))
    xden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4))
    
    ynum = (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4)
    yden = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4)
    
    if xnum == -0.0:
        xnum = 0.0
        
    if ynum == -0.0:
        ynum = 0.0

    if xden != 0 and yden != 0:
        xcoor =  xnum / xden
        ycoor = ynum / yden
        if onSegment(x1, x2, xcoor, y1, y2, ycoor) and onSegment(x3, x4, xcoor, y3, y4, ycoor):
            return (xcoor,ycoor)
    
    if xden == 0 and yden == 0:
        xden_new = (x3-x1)*(y2-y4) - (x2-x4)*(y3-y1)
        if xden_new == 0:
            return 'parellel'
        
    else:
        return None

def onSegment(x1, x2, xcoor, y1, y2, ycoor):
    if min(x1, x2) <= xcoor and xcoor <= max(x1, x2) and min(y1, y2) <= ycoor and ycoor <= max(y1, y2):
        return True
    else:
        return False    

def isValidParentheses(s):
        if "(" not in s or ")" not in s:
            return False
        count = 0
        for i in s:
            if i == "(":
                count += 1
            elif i == ")":
                if count == 0:
                    return False
                count -= 1
        return count == 0

def isValidNumberinParentheses(s):
    for segment in re.findall("[(][^)]*[)]", s):
        if len(re.findall("[-+]?\d+[\.]?\d*[eE]?[-+]?\d*", segment)) == 2:
            continue
        else:
            return False
    return True

def isValidStreetName(s):
    if s == "":
        return False
    if all(x.isalpha() or x.isspace() for x in s):
        return True
    return False

def changeParenthese(s):
    s_new = ""
    for i in range(len(s)-1):
        if s[i+1] == "(" and s[i] == ")":
            s_new += s[i]
            s_new += " "
        else:
            s_new += s[i]
    s_new += s[-1]
    return s_new
  
def get_points(s):
    new_vertex = []
    
    s = s.replace(" ", "")
    s = s.replace(")(", ",")
    s = s.replace(")", "")
    s = s.replace("(", "")
    s = s.replace(",", " ")
    s_new = []
    #s = s[1:]
    s = s.split(' ')
    s_new = [x for x in s if x != ""]
    #vertex = []
    #tmp = []
    
    vertex_even = s_new[0::2]
    vertex_odd = s_new[1::2]
    
    for i in range(len(vertex_even)):
        new_vertex.append((int(vertex_even[i]),int(vertex_odd[i])))
    '''
    for i in range(len(s_new)):
        if i % 2 == 0:
            tmp.append(int(s_new[i]))
        elif i % 2 == 1:
            tmp.append(int(s_new[i]))
            vertex.append(tmp)
            tmp = []
    
    for i in vertex:
        new_vertex.append((i[0],i[1]))
    '''    
    #print "vertex is: ", new_vertex
    return new_vertex

    
dic = {}
vertex_id = []


def main():
    ### YOUR MAIN CODE GOES HERE

    ### sample code to read from stdin.
    ### make sure to remove all spurious print statements as required
    ### by the assignment
    
    '''
    while True:
        line = sys.stdin.readline()
        if line == '':
            break
        print('read a line:', line)

    print('Finished reading input')
    # return exit code 0 on successful termination
    sys.exit(0)
    '''
    while True:
        #in Python 2.x, remember to change!
        user_input = raw_input("Please input your command: ")
        
        if user_input == '':
            break
        
        elif 'g' == user_input.strip(' '):
            #if len(dic) < 1:
                #print "Error: no street information is entered!"
                
            #print "calling the generated graph for you!"
            get_vertex_edges() 
        
        elif '"' not in user_input:
            print "Error: invalid input! Please check again!"
        
        elif '"' in user_input and len(user_input.split('"'))==3:
            user_input_list = user_input.split('"')
            
            #remember here add the command limit: limit the character number to 2
            #len(user_input_list[0]) == 2
            
            if isValidParentheses(user_input_list[2]) and isValidNumberinParentheses(user_input_list[2]) and user_input_list[2].split(' ')[0] == "" and isValidStreetName(user_input_list[1]):
                #print "valid parenthess and space"
                vertex = get_points(user_input_list[2])
                
                for i in vertex:
                    if i not in vertex_id:
                        vertex_id.append(i)
                        
                if 'a' == user_input_list[0].strip(' ') and len(user_input_list) == 3 and 'a ' in user_input_list[0]:
                    if user_input_list[1].lower() not in dic:
                        dic[user_input_list[1].lower()] = vertex
                    else:
                        print "Error: street name already exists, you might change 'a' into 'c' for changing information!"
                        
                elif 'c' in user_input_list[0].strip(' ') and len(user_input_list) == 3 and 'c ' in user_input_list[0]:
                    if user_input_list[1].lower() in dic:
                        dic[user_input_list[1].lower()] = vertex
                    else:
                        print "Error: 'c' specified for a street that does not exist!"
                
                else:
                    print "Error: Command is not valid!"
                    
                #print "Your dictionary information is: ", dic
                
            elif 'r' in user_input_list[0].strip(' ') and isValidStreetName(user_input_list[1]) and len(user_input_list[0]) == 2 and 'r ' in user_input_list[0]:
                if user_input_list[1].lower() in dic:
                    del dic[user_input_list[1].lower()]
                else:
                    print "Error: 'r' specified for a street that does not exist!"
                 
            else:
                print "Error: invalid input!"
                
        else:
            print "Error: Invalid format, please check again!"
            
        #sys.exit(0)
        
def get_vertex_edges():
    input_dic = dic
    #print "input_dic is: ", input_dic
    
    all_points = []
    for i in input_dic:
        all_points.append(input_dic[i])
    #print "all_points is :", all_points
    
    intersection_line = []
    for i in range(len(all_points)):
        for j in range(i+1, len(all_points)):
            for m in range(len(all_points[i]) - 1):
                l1 = Line(Point(all_points[i][m][0], all_points[i][m][1]), Point(all_points[i][m+1][0], all_points[i][m+1][1]))
                for n in range(len(all_points[j])-1):
                    l2 = Line(Point(all_points[j][n][0], all_points[j][n][1]), Point(all_points[j][n+1][0], all_points[j][n+1][1]))
                    intersect_result = intersect(l1, l2)
                    
                    if intersect_result and intersect_result != 'parellel':
                        if [all_points[i][m], intersect(l1, l2), all_points[i][m+1]] not in intersection_line:
                            intersection_line.append([all_points[i][m], intersect(l1, l2), all_points[i][m+1]])
                        if [all_points[j][n], intersect(l1, l2), all_points[j][n+1]] not in intersection_line:
                            intersection_line.append([all_points[j][n], intersect(l1, l2), all_points[j][n+1]])
                        
                        if intersect_result not in vertex_id:
                            vertex_id.append(intersect_result)
                            
                    elif intersect_result == 'parellel':
                        four_points = [all_points[i][m], all_points[i][m+1], all_points[j][n], all_points[j][n+1]]
                        four_points = sorted(four_points)
                    
                        intersection_line.append([four_points[0], four_points[1], four_points[2]])
                        intersection_line.append([four_points[1], four_points[2], four_points[3]])   
                    
                    else:
                        continue  
                    
    #print "intersection line is: ", intersection_line
    new_intersection_line = []
    tmp = []
    for j in range(len(intersection_line) - 1):
        for i in range(j+1, len(intersection_line)):
                
            if intersection_line[j][0] ==  intersection_line[i][0] and intersection_line[j][-1] == intersection_line[i][-1]:
                for p in intersection_line[j]:
                    tmp.append(p)
                for q in intersection_line[i]:
                    if q not in tmp:
                        tmp.append(q)
                    else:
                        continue
                
                del intersection_line[i]
                del intersection_line[j]
                
                new_intersection_line.append(sorted(tmp))
                tmp = []
                break
            
        if j == len(intersection_line) - 2:
            break
    #print "new intersection line is: ", new_intersection_line
    
    for i in new_intersection_line:
        intersection_line.append(i)

    pos = []
    for i in intersection_line:
        for j in i:
            if j not in pos:
                pos.append(j)
            
    #key = list(range(len(pos)))
    #output_v = dict(zip(key, pos))
        
    output_v = {}
    for i in pos:
        output_v[vertex_id.index(i)] = i
        
    print "V = {"
    for key, value in output_v.items():
        print ' ' + str(key) + ': '+'('+ str(round(value[0], 2)) + ',' + str(round(value[1], 2)) + ')'
    print "}"
            
    final_edges = []
    
    for i in range(len(intersection_line)):
        for j in range(len(intersection_line[i])-1): 
            first = list(output_v.keys())[list(output_v.values()).index(intersection_line[i][j])]
            second = list(output_v.keys())[list(output_v.values()).index(intersection_line[i][j+1])]
            if first != second:
                final_edges.append('<' + str(first) + ',' + str(second) + '>')
            
    final_edges = list(set(final_edges))
    print "E = {"
    for i in range(len(final_edges)): 
        if i == len(final_edges)-1:
            print ' ' + final_edges[i]
        else:
            print ' ' + final_edges[i] + ','
    print '}'
    
if __name__ == '__main__':
    main()
