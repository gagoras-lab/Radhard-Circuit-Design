//Theodoros Lianidis AEM:02919
//Gerasimos Agoras AEM:02947

#include "structs.h"
#include "functions.h"

int compareFiles(FILE *file1, FILE *file2)
{ //Compare the golden truth table with the fault injected one
   char ch1 = getc(file1);
   char ch2 = getc(file2);
   int error = 0;
   int pos = 0, line = 1, false_line = 0;
   while (ch1 != EOF && ch2 != EOF){
      pos++;
      if (ch1 == '\n' && ch2 == '\n'){
         line++;
         pos = 0;
      }
      if (ch1 != ch2 && line != false_line){   //If two corresponding lines of the truth tables have one or more differencies, then error advances by 1
         error++;
         false_line = line;
      }
      ch1 = getc(file1);
      ch2 = getc(file2);
   }
   return(error);   //Return the number of total computed errors
}

void logical_masking(interface* main_pointer, FILE* sim_file)
{ //Computes soft error rate of the circuit
	
    double combinations;
    int false_outs = 0,curr_false, num_of_iters = 0;
    FILE* fault_file = fopen("fault_injected_table_s27.txt", "w+"); // Creation of fault injected truth table 

    for(int i=0;i<main_pointer->size;i++){
        if(main_pointer->instance[i].type == 3){      //All gates which have as output wires will be hited
            num_of_iters++;                           //Calculate total number of fault injections
            main_pointer->instance[i].hit=1;           //Mark this wire as particle hited
            rewind(sim_file);                            //Put read/write position in the beginning of the golden truth table
            combinations = simulator(main_pointer, fault_file);  //Do the simulation for the current fault injection
            rewind(fault_file);                          //Put read/write position in the beginning of the fault injected truth table
            curr_false = compareFiles(sim_file, fault_file);  //Compare the two truth tables
            false_outs += curr_false;                       //Total number of Soft errors for all fault injections
            main_pointer->instance[i].hit=0;                //Mark this wire as non particle hited
            fclose(fault_file);                             //Close fault injected truth table file and open it again, in order to remove the contents
            fault_file = fopen("fault_injected_table_s27.txt", "w+");  
            
            // Print the name of the node, the number of soft errors and the soft error rate for this particular fault injection
            printf("Node name: %s, False outputs = %d, False Rate = %lf\n",main_pointer->instance[i].name, curr_false, curr_false/combinations);
        }

    }
    printf("TOTAL SOFT ERROR RATE = %lf\n", false_outs/(combinations*num_of_iters)); //Print the total soft error rate for all fault injections
    fclose(fault_file);
    remove("fault_injected_table_s27.txt");   // Erase the fault injected truth table from the directory
}
