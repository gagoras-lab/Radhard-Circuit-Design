//Theodoros Lianidis AEM:02919
//Gerasimos Agoras AEM:02947

#include "structs.h"
#include "functions.h"

int main(int argc, char *argv[])
{

    FILE *parser_file; // File descriptor of Verilog post-synthesis file
    FILE *sim_file;    // File descriptor of the golden truth table of the circuit
    int i, j;
    node *ptr;
    interface *main_pointer;

    parser_file = fopen("s27_mapped.txt", "r");  //Open Verilog post-synthesis file
    if (parser_file == NULL)
    {
        printf("Could not open post-synthesis Verilog file for reading.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Post-synthesis Verilog file is opened for reading.\n");
    }

    main_pointer = circuit_parser(parser_file);


    for (i = 0; i < main_pointer->size; i++)   //Check if parsing was successful
    {
        printf("Node name: %s\n", main_pointer->instance[i].name);  //Print node names

        switch (main_pointer->instance[i].type)  //Print node type
        {
        case 1:
            printf("Node type: input\n");
            break;

        case 2:
            printf("Node type: output\n");
            break;

        case 3:
            printf("Node type: wire\n");
            break;

        default:
            printf("Error, invalid node type %d .\n", main_pointer->instance[i].type);
            exit(EXIT_FAILURE);
        }

        printf("Input to:\n");
        for (j = 0; j < main_pointer->instance[i].gate_pointer_size; j++)  //Print the gates to which the current node is connected
        {
            switch (main_pointer->instance[i].gate_pointer[j]->type)
            {
            case 1:
                printf("AND");
                break;

            case 2:
                printf("NAND");
                break;

            case 3:
                printf("OR");
                break;

            case 4:
                printf("NOR");
                break;

            case 5:
                printf("INV");
                break;

            default:
                printf("Error invalid gate type: %d .\n", main_pointer->instance[i].gate_pointer[j]->type);
                exit(EXIT_FAILURE);
            }
            printf("%d", main_pointer->instance[i].gate_pointer[j]->fan_in_size);  //Print the fan-in of the gate
            ptr = main_pointer->instance[i].gate_pointer[j]->node_pointer;

            printf("   Output-> %s \n", ptr->name);  //Print the name of the output node of the gate
        }
        printf("\n\n");
    }

    sim_file = fopen("truth_table_s27.txt", "w+");   // Creation of golden truth table text file
    simulator(main_pointer, sim_file);
    logical_masking(main_pointer, sim_file);

    fclose(sim_file);
    fclose(parser_file);
    return 0;
}
