#ifndef NODE_H
#define NODE_H

struct Node{
  int x,y;
  bool validity;
  double h, g, f;
  Node* parent;
  Node* children[];
};

typedef struct Element Element;
struct Element
{
    Node *node;
    Element *next;
};

typedef struct Liste Liste;
struct Liste
{
    Element *first;
};

void initNode(Node *node, int x, int y);

void displayNode(Node node);

bool isValid(int x, int y, int map[MAP_SIZE*MAP_SIZE]);
bool isDestination(int x, int y, Node node_destination);

double calculateHeuristic(int x, int y, Node node_destination);


void generateNodeTab(int map[][MAP_SIZE], Node* nodeMap[]);

Node * aStar(int map[MAP_SIZE*MAP_SIZE], int xStart, int yStart, int xDest, int yDest);

#endif
