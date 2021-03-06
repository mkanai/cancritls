#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <vector>
#include <queue>

// Large File Support
#define _FILE_OFFSET_BITS 64

#define MAX_V 2048
#define INF numeric_limits<double>::infinity()

using namespace std;

typedef pair<double, int> Pair;
struct edge
{
    int to;
    int capacity;
    double cost;
    int rev;
};

int V; // number of vertices
vector<edge> graph[MAX_V]; // adjacency list of graph
double h[MAX_V]; // potential
double dist[MAX_V]; // minimum distance from s
int prevv[MAX_V]; // previous vertex
int preve[MAX_V]; // previous edge

// add edge to graph
void add_edge(int from, int to, int capacity, double cost)
{
    graph[from].push_back((edge){to, capacity, cost, graph[to].size()});
    graph[to].push_back((edge){from, 0, -cost, graph[from].size() - 1});
}


// solve minimun cost flow problem from s to t to flow f
double min_cost_flow(int s, int t, int f)
{
    double ret = 0;
    fill(h, h + V, 0); // initialize h

    while (f > 0)
    {
        // Dijkstra algorithm
        priority_queue<Pair, vector<Pair>, greater<Pair> > que;
        fill(dist, dist + V, INF);
        dist[s] = 0;
        que.push(Pair(0, s));
        while (!que.empty())
        {
            Pair p = que.top();
            que.pop();
            int v = p.second;
            if (dist[v] < p.first)
                continue;
            for (int i = 0; i < graph[v].size(); i++)
            {
                edge &e = graph[v][i];
                double d = dist[v] + e.cost + h[v] - h[e.to];
                if (dist[v] > d) d = dist[v]; // for rounding error
                if (e.capacity > 0 && dist[e.to] > d)
                {
                    dist[e.to] = d;
                    prevv[e.to] = v;
                    preve[e.to] = i;
                    que.push(Pair(dist[e.to], e.to));
                }
            }
        }

        // no solution
        if (dist[t] == INF)
            return -1;

        // update potential
        for (int v = 0; v < V; v++)
            h[v] += dist[v];

        int d = f;        
        for (int v = t; v != s; v = prevv[v])
            d = min(d, graph[prevv[v]][preve[v]].capacity);
        f -= d;
        ret += d * h[t];
        for (int v = t; v != s; v = prevv[v])
        {
            edge &e = graph[prevv[v]][preve[v]];
            e.capacity -= d;
            graph[v][e.rev].capacity += d;
        }

        printf("Progress... %d\r", f);
    }
    printf("\r\nDone.\n");
    return ret;
}


char* load_file(char* path)
{
    FILE* fp = fopen(path, "rb");
    off_t filesize;
    char* buf;

    if(!fp) goto LOAD_ERROR; //FILE NOT FOUND.

    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    buf = (char*)malloc(filesize);
    fseek(fp, 0, SEEK_SET);
    if(fread(buf, filesize, 1, fp) != 1) goto LOAD_ERROR; //FILE READ ERROR. OR MEMORY ALLOCATION ERROR.
    fclose(fp);

    return buf;

    LOAD_ERROR:
        // log_error("LOAD ERROR.\n");
        return NULL;
}

int strcnt(char* buf, char* str, char eos)
{
    char* bufptr;
    int count = 0;
    int n = strlen(str);
    for (bufptr = buf; *bufptr != eos; bufptr++)
        if (strncmp(bufptr, str, n) == 0)
            count++;

    return count;
}

void load_distance(double* distance, char* buf, int n, int m, char sep)
{
    char* bufptr = buf;
    char* bufptr2 = buf;
    for (int i = 0; i < n; i++)
    {
        distance[i * m] = atof(++bufptr2);
        bufptr2 = strchr(bufptr2, '\n');        
        for (int j = 1; j < m ; j++)
        {
            if ((bufptr = strchr(bufptr, sep)))
                distance[i * m + j] = atof(++bufptr);
        }
    }
}

int main(int argc, char* argv[])
{
    char* loadbuf = load_file(argv[1]);
    int ncase = strcnt(loadbuf, (char*)"\n", '\0'); // number of cases
    int ncontrol = strcnt(loadbuf, (char*)"\t", '\n') + 1; // number of controls

    int N = atoi(argv[2]);

    printf("Case:    %d\n", ncase);
    printf("Control: %d\n", ncontrol);

    double* distance = new double[ncase * ncontrol];
    load_distance(distance, loadbuf, ncase, ncontrol, '\t');

    free(loadbuf);

    int s = ncase + ncontrol, t = s + 1;
    V = t + 1;
    for (int i = 0; i < ncase; i++)
    {
        add_edge(s, i, N, 0);
        for (int j = 0; j < ncontrol; j++)
            add_edge(i, ncase + j, 1, distance[i * ncontrol + j]);
    }
    for (int j = 0; j < ncontrol; j++)
        add_edge(ncase + j, t, 1, 0);

    double mincost = min_cost_flow(s, t, ncase * N);
    printf("Sum of Distances: %f\n", mincost);
   
    string output(argv[1]); 
    output += ".ccmatch";
    FILE* fp = fopen(output.c_str(), "w");
    for (int i = 0; i < ncase; i++)
    {
        fprintf(fp, "%d\t", i + 1);
        double sum = 0;
        for (int j = 0; j < ncontrol; j++)
        {
            if (graph[ncase + j][i].capacity  == 1)
            {
                fprintf(fp, "%d\t", j + 1);
                sum += distance[i * ncontrol + j];
            }
        }
        fprintf(fp, "%f\n", sum);
    }

    fclose(fp);
    delete [] distance;

    return 0;
}
