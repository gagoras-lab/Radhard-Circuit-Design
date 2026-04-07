//Theodoros Lianidis AEM:02919
//Gerasimos Agoras AEM:02947

#include "structs.h"
#include "functions.h"

void clean_str(char str[])
{ // General function for cleaning strings
    int i = 0;

    while (str[i] != '\0')
    {

        str[i] = '\0';
        i++;
    }
}

void node_creator(interface *inouwi, char name[], int type)
{ //Initialization of a node
    node *instance_check;

    instance_check = (node *)realloc(inouwi->instance, (inouwi->size + 1) * sizeof(node));

    if (instance_check == NULL)
    {
        printf("Error: node_creator realloc failed.\n");
        exit(EXIT_FAILURE);
    }

    inouwi->instance = instance_check;
    inouwi->size = inouwi->size + 1; // Update the number of nodes that exist in the circuit

    // Initialization of the new instance of node
    strcpy(inouwi->instance[inouwi->size - 1].name, name);
    inouwi->instance[inouwi->size - 1].type = type;
    inouwi->instance[inouwi->size - 1].gate_pointer = NULL;
    inouwi->instance[inouwi->size - 1].gate_pointer_size = 0;
    inouwi->instance[inouwi->size - 1].hit = 0;
}

gate *gate_creator(int fan_in_size, char type[])
{ //Initialization of a gate
    gate *gate_instance;
    int i;

    gate_instance = (gate *)malloc(sizeof(gate));
    if (gate_instance == NULL)
    {
        printf("Error: gate_creator malloc failed.\n");
        exit(EXIT_FAILURE);
    }

    // Initialization of the new instance of gate
    gate_instance->fan_in_size = fan_in_size;

    if (!strcmp(type, "AND"))
        gate_instance->type = 1;
    else if (!strcmp(type, "NAND"))
        gate_instance->type = 2;
    else if (!strcmp(type, "OR"))
        gate_instance->type = 3;
    else if (!strcmp(type, "NOR"))
        gate_instance->type = 4;
    else if (!strcmp(type, "INV"))
        gate_instance->type = 5;
    else
    {
        printf("Error: Invalid type of gate: %s\n", type);
        exit(EXIT_FAILURE);
    }

    gate_instance->input_values = (bool *)malloc(fan_in_size * sizeof(bool));
    for (i = 0; i < fan_in_size; i++)
    {
        gate_instance->input_values[i] = 0;
    }
    gate_instance->next_index = 0;
    gate_instance->output_value = 0;
    gate_instance->node_pointer = NULL;

    return (gate_instance);
}

void node_to_gate(gate *gate, node *node)
{ // Node is an input of the gate, so they must be connected.

    int curr_gate_pointer_size = node->gate_pointer_size + 1;
    node->gate_pointer = realloc(node->gate_pointer, curr_gate_pointer_size * sizeof(*gate));
    if (node->gate_pointer == NULL)
    {
        printf("Error: gate_check realloc failed.\n");
        exit(EXIT_FAILURE);
    }

    node->gate_pointer[node->gate_pointer_size] = gate;
    node->gate_pointer_size++;
}

interface *circuit_parser(FILE *parser_file)
{ // Main function that handles the circuit parser

    char str[30] = {'\0'};
    char node_type[7] = {'\0'};
    char single = '\0';
    interface *inouwi;
    int i = 0;
    int j = 0;
    int type_parsing;  // 1: input   2: output   3: wire
    int type_size;
    int fan_in;
    char gate_type[6] = {'\0'};

    printf("Circuit_parser function is called.\n");

    inouwi = (interface *)malloc(sizeof(interface));
    if (inouwi == NULL)
    {
        printf("Error: Malloc for interface *inouwi pointer failed.\n");
        exit(EXIT_FAILURE);
    }
    inouwi->size = 0;
    inouwi->instance = NULL;

    for (type_parsing = 1; type_parsing < 4; type_parsing++)
    {
        switch (type_parsing)
        {
        case 1:
            strcpy(node_type, "input");
            type_size = 5; // Length of the string "input"
            break;

        case 2:
            strcpy(node_type, "output");
            type_size = 6; // Length of the string "output"
            break;

        case 3:
            strcpy(node_type, "wire");
            type_size = 4; // Length of the string "wire"
            break;

        default:
            printf("Error in switch - case type_parsing.\n");
            exit(EXIT_FAILURE);
        }

        while (1) // Find "input", "output" or "wire" string in the file
        {
            fread(str, 1, type_size, parser_file);
            if (!strcmp(str, node_type))
            {
                break;
            }
            fseek(parser_file, -(type_size - 1), SEEK_CUR);
        }

        clean_str(str);

        while (1) // Find nodes in the file and create the corresponding structs
        {
            fread(&single, 1, 1, parser_file);
            if (single == ' ' || single == '\n')
            {
                continue;
            }
            else if (single == ',' || single == ';')
            {
                if (type_parsing == 1 && (!strcmp(str, "GND") || !strcmp(str, "VDD") || !strcmp(str, "CK")))   
                { //Exclude input nodes with names: GND, VDD, CK
                    clean_str(str);
                    i = 0;
                }
                else
                {
                    node_creator(inouwi, str, type_parsing);   

                    clean_str(str);
                    i = 0;
                }
                if (single == ';')
                {
                    break;
                }
            }
            else
            {
                str[i] = single;
                i++;
            }
        }
    }

    printf("Parsed nodes successfully.\n");

    while (1)
    { // Find the gates in the file and create the corresponding structs. Connect the nodes with the gates appropriately.
        do
        {
            fread(&single, 1, 1, parser_file);
        } while (single == ' ' || single == '\n');

        str[0] = single;
        fread(&str[1], 1, 8, parser_file);

        if (!strcmp(str, "endmodule"))
        { // Reached "endmodule", so parsing is finished.
            break;
        }

        clean_str(str);

        fseek(parser_file, -9, SEEK_CUR); // Current file position is in the beginning of the line.

        while (1) // Read the type of the gate, including the fan-in and strore it in str.
        {
            fread(&single, 1, 1, parser_file);
            if (single != '_')
            {
                str[i] = single;
                i++;
            }
            else
            {
                i = 0;
                break;
            }
        } // Current file position in front of the first undescore '_'

        do
        { // Go right in front of the first parenthesis '('.
            fread(&single, 1, 1, parser_file);
        } while (single != '(');

        // Type and fan-in of the current gate have been detected
        if (!strcmp(str, "DFF"))
        { // Flip-Flop case
            clean_str(str);
            do
            { // Go in front of the next '('.
                fread(&single, 1, 1, parser_file);
            } while (single != '(');

            while (1)
            {
                fread(&single, 1, 1, parser_file);
                if (single == ' ')
                {
                    continue;
                }
                if (single == ')')
                { // If the input of the DFF is also a primary input of the circuit, then we do not change its type. Otherwise we make it an output of the circuit.
                    for (i = 0; i < inouwi->size; i++)
                    {
                        if (!strcmp(str, inouwi->instance[i].name) && inouwi->instance[i].type != 1)
                        {
                            inouwi->instance[i].type = 2;
                            i = 0;
                            break;
                        }
                    }

                    clean_str(str);
                    do
                    { // Go to the output of the DFF
                        fread(&single, 1, 1, parser_file);
                        if (single == '(')
                        {
                            i++;
                        }
                    } while (i < 2);

                    i = 0;

                    while (1)
                    {
                        fread(&single, 1, 1, parser_file);
                        if (single == ' ')
                        {
                            continue;
                        }
                        else if (single == ')')
                        {
                            break;
                        }
                        else
                        {
                            str[i] = single;
                            i++;
                        }
                    }


                    for (i = 0; i < inouwi->size; i++)
                    { // Make the output of the DFF a primary input
                        if (!strcmp(str, inouwi->instance[i].name))
                        {
                            inouwi->instance[i].type = 1;
                            i = 0;
                            break;
                        }
                    }

                    clean_str(str);

                    do
                    { 
                        fread(&single, 1, 1, parser_file);
                    } while (single == ' ');


                    if(single == ','){
                        //There is another output of the flip flop
                       do
                       { // Go in front of the next '('.
                           fread(&single, 1, 1, parser_file);
                       } while (single != '(');

                       while (1)
                       {
                          fread(&single, 1, 1, parser_file);
                          if (single == ' ')
                          {
                            continue;
                          }
                          else if (single == ')')
                          {
                              break;
                          }
                          else
                          {
                              str[i] = single;
                              i++;
                          }
                       }

                       for (i = 0; i < inouwi->size; i++)
                       { // Make the second output of the DFF a primary input.
                        if (!strcmp(str, inouwi->instance[i].name))
                        {
                            inouwi->instance[i].type = 1;
                            i = 0;
                            break;
                        }
                      }

                      clean_str(str);

                    }

                
                    do
                    { // Go in front of the ';', which indicates the end of the line.
                        fread(&single, 1, 1, parser_file);
                    } while (single != ';');
                    break;
                    
                }
                str[i] = single;
                i++;
            }
        }
        else
        { // Gate instatiation
            for (i = 0; str[i] != '\0'; i++); // Go to the end of str, which includes the gate type followed by its fan-in.

            if (!strcmp(str, "INV"))
            { // Inverter case
                fan_in = 1;
                strcpy(gate_type, "INV");
            }
            else
            {                                  // Other gates
                fan_in = atoi(&str[i - 1]);    // Convert the last character of str, which holds the fan-in of the gate, to an integer and store it to the fan_in variable.
                memcpy(gate_type, str, i - 1); // Copy only the gate type of str, without the fan-in, to string gate_type
            }

            i = 0;

            gate *gate_instance = gate_creator(fan_in, gate_type); // Create struct for the current gate

            clean_str(str);
            clean_str(gate_type);

            // Put input nodes
            for (j = 0; j < fan_in; j++)
            {
                do
                {
                    fread(&single, 1, 1, parser_file);
                } while (single != '('); // Go in front of the next parenthesis '('.

                while (1)
                {

                    fread(&single, 1, 1, parser_file);
                    if (single == ' ')
                    {
                        continue;
                    }
                    if (single == ')')
                    {
                        for (i = 0; i < inouwi->size; i++)
                        {
                            if (!strcmp(str, inouwi->instance[i].name))
                            {
                                node_to_gate(gate_instance, &(inouwi->instance[i])); // Connect the gate with its input node
                                i = 0;
                                clean_str(str);
                                break;
                            }
                        }
                        break;
                    }
                    str[i] = single; // Read the name of the input node
                    i++;
                }
            }

            do
            {
                fread(&single, 1, 1, parser_file);
            } while (single != '('); // Go to the next parenthesis '('.

            // Put output node
            while (1)
            {
                fread(&single, 1, 1, parser_file);
                if (single == ' ')
                {
                    continue;
                }
                else if (single == ')')
                {
                    for (i = 0; i < inouwi->size; i++)
                    {
                        if (!strcmp(str, inouwi->instance[i].name))
                        {

                            gate_instance->node_pointer = &(inouwi->instance[i]); // Connect gate with its output node
                            i = 0;
                            break;
                        }
                    }
                    
                    clean_str(str);
					
                    do
                    {
                        fread(&single, 1, 1, parser_file);
                    } while (single != ';'); // Go to the ';', which indicates the end of the line.
                    break;
                }
                str[i] = single; // Read the name of the output node
                i++;
            }
        }
    }
    printf("Parsing finished.\n\n");
    return (inouwi);
}
