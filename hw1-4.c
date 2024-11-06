#include <stdio.h>
#include <stdlib.h>

#define MAX 100 //maximum number of vertices in the graph

int queue[MAX], front = -1, rear = -1;

//enqueue an element
void enqueue(int vertex) {
    if (rear == MAX - 1) return;//queue overflow
    if (front == -1) front = 0;
    queue[++rear] = vertex;
}

//dequeue an element
int dequeue() {
    if (front == -1 || front > rear) return -1;//queue underflow
    return queue[front++];
}

//check if the queue is empty
int isEmpty() {
    return front == -1 || front > rear;
}

//perform BFS traversal on the graph
void bfs(int m, int adjMatrix[MAX][MAX], int startVertex) {
    int visited[MAX] = {0};//array to track visited vertices
    int traversal[MAX];//store the BFS visit order
    int count = 0;

    enqueue(startVertex);
    visited[startVertex] = 1;

    while (!isEmpty()) {
        int currentVertex = dequeue();
        traversal[count++] = currentVertex + 1;//add the 1-based index of the current vertex to the access order

        //check adjacent vertices of the current vertex
        for (int i = 0; i < m; i++) {
            if (adjMatrix[currentVertex][i] == 1 && !visited[i]) {
                enqueue(i);
                visited[i] = 1;//set the vertex 1
            }
        }
    }
    
    //print the visit order
    for (int i = 0; i < count; i++) {
        if (i != 0) printf(" ");
        printf("%d", traversal[i]);
    }
    printf("\n");
}

int main() {
    int vertices;
    scanf("%d", &vertices);

    int adjMatrix[MAX][MAX];
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            scanf("%d", &adjMatrix[i][j]);
        }
    }

    bfs(vertices, adjMatrix, 0);  //start BFS from vertex 1 (index 0 in array)

    return 0;
}