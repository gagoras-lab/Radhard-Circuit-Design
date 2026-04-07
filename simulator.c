//Theodoros Lianidis AEM:02919
//Gerasimos Agoras AEM:02947

#include "structs.h"
#include "functions.h"

void next_in_value(in_node *input_nodes, int index)
{ //Computes next input value of circuit's primary inputs
    if (input_nodes[index - 1].in_value)
    {
        input_nodes[index - 1].in_value = 0;
        next_in_value(input_nodes, index - 1);
    }
    else
    {
        input_nodes[index - 1].in_value = 1;
    }
}

void bit_propagate(node *node_addr,bool bit,out_node *output_nodes,int out_counter)
{ //Recursive algorithm, which implements the simulation of the circuit
    
    int i,j;
    bool out;
    gate *gate_addr;

    if(node_addr->type == 2){   //If current node is an output then find it in array output_nodes and write its value 
        for(i=0;i<out_counter;i++){
            if(!strcmp(output_nodes[i].name,node_addr->name)){
                output_nodes[i].out_value = bit;
                break;
            }
        }
    }

    for(i=0;i<node_addr->gate_pointer_size;i++){  //Each node propagates its value to all gates, which are connected

        gate_addr = node_addr->gate_pointer[i];

        gate_addr->input_values[gate_addr->next_index] = bit;    //Put bit to the next available position in input_values array of gate
        gate_addr->next_index++;

        if(gate_addr->next_index == gate_addr->fan_in_size){  //If input_values array of gate is full, then the output of the gate will be computed
            gate_addr->next_index = 0;    //The position to which the next input bit of the gate will be stored is set to zero, for the next simulation

            switch(gate_addr->type){

                case 1:
                case 2:

                    out = 1;   //AND
                    for(j=0;j<gate_addr->fan_in_size;j++){
                        out = out & gate_addr->input_values[j];
                    }

                    if(gate_addr->type == 2){   //NAND
                        out = !out;
                    }

                    break;

                case 3:
                case 4:

                    out = 0;
                    for(j=0;j<gate_addr->fan_in_size;j++){   //OR
                        out = out | gate_addr->input_values[j];
                    }

                    if(gate_addr->type == 4){   //NOR
                        out = !out;
                    }

                    break;

                case 5:    //INV

                    out = !*(gate_addr->input_values);
                    break;

                default:

                printf("Error, invalid gate type: %d in bit_propagate function.\n",gate_addr->type);
                exit(EXIT_FAILURE);
            }
            node *ptr1 = gate_addr->node_pointer;
            if(ptr1->hit){ //If the gate gets hit by a particle, then its output value in inverted (SET)
                out = !out;
            }

            gate_addr->output_value = out;   //Store output value of the gate

            bit_propagate(gate_addr->node_pointer,out,output_nodes,out_counter);  //Repeat for gate's output node
        }
    }
}

double simulator(interface *main_pointer, FILE *sim_file)
{ //Simulates the circuit
    int i, j, k, in_counter = 0, out_counter = 0;
    double in_comb_num;

    node curr_inst;
    out_node *output_nodes = NULL;
    out_node *output_nodes_check;
    in_node *input_nodes = NULL;
    in_node *input_nodes_check;

    for (i = 0; i < main_pointer->size; i++)   
    {
        curr_inst = main_pointer->instance[i];
        if (curr_inst.type == 1)
        { //Create an array of in_node structs for the inputs of the circuit
            in_counter++;  //Calculate the number of the inputs of the circuit
            input_nodes_check = (in_node *)realloc(input_nodes, in_counter * sizeof(in_node));
            if (input_nodes_check == NULL)
            {
                printf("Error, realloc for input_nodes_check failed.\n");
                exit(EXIT_FAILURE);
            }
            input_nodes = input_nodes_check;
            input_nodes[in_counter - 1].name_length = strlen(curr_inst.name);  //Store the length of the input node's name
            input_nodes[in_counter - 1].in_value = 0;    //Make its value 0

            fwrite(curr_inst.name, input_nodes[in_counter - 1].name_length, 1, sim_file); //Write the names of the input nodes to the truth table
            fflush(sim_file);
            fwrite("   ", 3, 1, sim_file);
            fflush(sim_file);
        }
    }

    //Character '|' splits input vector from output vector on each iteration of the simulation in the corresponding txt file that contains the truth table
    fwrite("|   ", 4, 1, sim_file);
    fflush(sim_file);

    for (i = 0; i < main_pointer->size; i++)
    {
        curr_inst = main_pointer->instance[i];
        if (curr_inst.type == 2)
        { //Create an array of out_node structs for the outputs of the circuit
            out_counter++; //Calculate the number of the outputs of the circuit
            output_nodes_check = (out_node *)realloc(output_nodes, out_counter * sizeof(out_node));
            if (output_nodes_check == NULL)
            {
                printf("Error, realloc for output_nodes_check failed.\n");
                exit(EXIT_FAILURE);
            }
            output_nodes = output_nodes_check;
            strcpy(output_nodes[out_counter - 1].name, curr_inst.name);   //Copy the name of the output nodes to the out_node struct
            output_nodes[out_counter - 1].name_length = strlen(curr_inst.name);    //Store the length of the output node's name
            fwrite(curr_inst.name, output_nodes[out_counter - 1].name_length, 1, sim_file); //Write the names of the output nodes to the truth table
            fflush(sim_file);
            fwrite("   ", 3, 1, sim_file);
            fflush(sim_file);
        }
    }

    in_comb_num = pow(2, in_counter);   //Calculate the total number of input combinations of the circuit

    for (i = 0; i < in_comb_num; i++)
    {

        fwrite("\n", 1, 1, sim_file);
        fflush(sim_file);
        for (j = 0; j < in_counter; j++)   //Write to the truth table 1 or 0 for the inputs, according to the array of in_node structs
        {
            if (input_nodes[j].in_value)
            {
                fwrite("1", 1, 1, sim_file);
                fflush(sim_file);
            }
            else
            {
                fwrite("0", 1, 1, sim_file);
                fflush(sim_file);
            }

            for (k = 0; k < input_nodes[j].name_length + 2; k++)   //Leave spaces between the values for correct spacing
            {
                fwrite(" ", 1, 1, sim_file);
                fflush(sim_file);
            }
        }

        k=0;
    

        fwrite("|   ", 4, 1, sim_file);
        fflush(sim_file);

        for (j = 0; j < main_pointer->size; j++)
        {
           
           if (main_pointer->instance[j].type == 1)
           {  //For each input of the cricuit call bit_propagate with the appropriate value
            bit_propagate(&(main_pointer->instance[j]),input_nodes[k].in_value,output_nodes,out_counter);
            k++;

           }
        
        }

        for (j = 0; j < out_counter; j++)    //Write to the truth table 1 or 0 for the inputs, according to the array of out_node structs
        {
            if (output_nodes[j].out_value)
            {
                fwrite("1", 1, 1, sim_file);
                fflush(sim_file);
            }
            else
            {
                fwrite("0", 1, 1, sim_file);
                fflush(sim_file);
            }

            for (k = 0; k < output_nodes[j].name_length + 2; k++)    //Leave spaces between the values for correct spacing
            {
                fwrite(" ", 1, 1, sim_file);
                fflush(sim_file);
            }
        }

        
        if (i != in_comb_num - 1)   
        {  //Calculate next input value for the primary inputs of the circuit / Move to next iteration
            next_in_value(input_nodes, in_counter);
        }
    }

    return(in_comb_num);    //Return the calculated total number of input combinations
}
