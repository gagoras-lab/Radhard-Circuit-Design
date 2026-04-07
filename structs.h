//Theodoros Lianidis AEM:02919
//Gerasimos Agoras AEM:02947

#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <stdio.h>   //File opening, fread
#include <stdbool.h> //Because we make use of boolean variables
#include <string.h>  //String functions e.g. strcpy, strcmp
#include <stdlib.h>  //Dynamic memory e.g. malloc, realloc
#include <math.h>    //For pow function

struct gate
{
    int type; // AND = 1, NAND = 2, OR = 3, NOR = 4, INV = 5
    int fan_in_size;
    bool *input_values; // Pointer to a boolean array, in which input values of gate are stored
    int next_index;     // Index in the boolean array, to which next input value will be stored
    bool output_value;
    struct node *node_pointer; // Output of the gate to a node
};

struct node
{
    char name[30];
    int type;              // input = 1, output = 2, wire = 3
    struct gate **gate_pointer;   // Pointer to a dynamic array of pointers to the gates, which the node is connected
    int gate_pointer_size; // Size of the dynamic array of pointers to gates
    bool hit;      //Indicator of particle hit
};

typedef struct gate gate;
typedef struct node node;

typedef struct
{                   // Interface node for access to the nodes of the circuit
    node *instance; // Pointer to a dynamic array of nodes
    int size;       // Size of the dynamic array of nodes
} interface;

typedef struct
{ // Struct for each output node
    char name[30];
    int name_length;
    bool out_value; // Output value in simulation
} out_node;

typedef struct
{ // Struct for each input node
    int name_length;
    bool in_value; // Input value in simulation
} in_node;

#endif