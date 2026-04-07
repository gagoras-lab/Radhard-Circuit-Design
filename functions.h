//Theodoros Lianidis AEM:02919
//Gerasimos Agoras AEM:02947

#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

extern void clean_str(char str[]); //Initialize the given string with zeros

extern void node_creator(interface *inouwi, char name[], int type); //Initialization of a node

extern gate *gate_creator(int fan_in_size, char type[]); // Initialization of a gate

extern void node_to_gate(gate *gate, node *node);  //Connects a node with the appropriate gate(s)

extern interface *circuit_parser(FILE *parser_file);  //Parses post-synthesis Verilog file 

extern void next_in_value(in_node *input_nodes, int index);  //Computes next input value for the simulation of the circuit

extern void bit_propagate(node *node_addr,bool bit,out_node *output_nodes,int out_counter);   //Recursive algorithm, which implements the simulation of the circuit

extern double simulator(interface *main_pointer, FILE *sim_file);  //Simulates the circuit

extern int compareFiles(FILE *file1, FILE *file2);   //Compares golden truth table and fault injected one

extern void logical_masking(interface* main_pointer, FILE* sim_file);  //Computes soft error rate of the circuit

#endif
