/*
	Jake Weber
	HW1
	Professor Montagne
	2/2/24
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARRAY_SIZE 500

struct instructionRegister
{
	int OP;
	int M;
	int L;
};

int main (int argc, char *argv[])
{
	
	// Initial values
	int BP = ARRAY_SIZE - 1;
	int SP = ARRAY_SIZE;
	int PC = 0;
	int eop = 1;
	int arb;
	
	// Variables only used for printing
	int numOfStacks = 0;
	int stack[500];
	int l;
	
	// Printing initial values
	printf("\t\t\tPC\tBP\tSP\tstack\n");
	printf("Initial values:\t\t%d\t%d\t%d\n\n", PC, BP, SP);
	
	// Initializing IR
	struct instructionRegister IR;
	IR.OP = 0;
	IR.M = 0;
	IR.L = 0;
	
	// File handling
	FILE *in = fopen(argv[1], "r");
	if (in ==  NULL)
	{
		printf("Could not open file\n");
		exit(1);
	}

	int pas[ARRAY_SIZE];
	for (int i = 0; i < ARRAY_SIZE; i++)
		pas[i] = 0;
	
	// Add input to pas
	int i = 0;
	while(fscanf(in, "%d %d %d", &pas[i], &pas[i + 1], &pas[i + 2]) == 3)
		i += 3;
	
	// P-Machine Cycles
	while (eop)
	{
		// Fetch Cycle
		IR.OP = pas[PC];
		IR.L = pas[PC + 1];
		IR.M = pas[PC + 2];
		PC += 3;

		// Execute Cycle
		switch (IR.OP)
		{
			// Literal
			case 1:
				SP--;
				pas[SP] = IR.M;
				
				// Printing
				stack[numOfStacks - 1]++;
				if (IR.M > 9)
					printf("\tLIT %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
				else
					printf("\tLIT %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
				break;
			// Operation
			case 2:
				switch (IR.M)
				{
					case 0:
						// Printing
						numOfStacks--;
						if (IR.M > 9)
							printf("\tRTNd\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tRTN %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						SP = BP + 1;
						BP = pas[SP - 2];
						PC = pas[SP - 3];
						break;
					case 1:
						pas[SP + 1] = pas[SP + 1] + pas[SP];
						SP++;
						
						// Printing
						stack[numOfStacks - 1]--;
						if (IR.M > 9)
							printf("\tADD %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tADD %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
					case 2:
						pas[SP + 1] = pas[SP + 1] - pas[SP];
						SP = SP + 1;
						
						// Printing
						stack[numOfStacks - 1]--;
						if (IR.M > 9)
							printf("\tSUB %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tSUB %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
					case 3:
						pas[SP + 1] = pas[SP + 1] * pas[SP];
						SP = SP + 1;
						
						// Printing
						stack[numOfStacks - 1]--;
						if (IR.M > 9)
							printf("\tMUL %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tMUL %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
					case 4:
						pas[SP + 1] = pas[SP + 1] / pas[SP];
						SP = SP + 1;
						
						// Printing
						stack[numOfStacks - 1]--;
						if (IR.M > 9)
							printf("\tDIV %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tDIV %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
					case 5:
						pas[SP + 1] = (pas[SP + 1] == pas[SP] ? 1 : 0);
						SP = SP + 1;
						
						// Printing
						stack[numOfStacks - 1]--;
						if (IR.M > 9)
							printf("\tEQL %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tEQL %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
					case 6:
						pas[SP + 1] = (pas[SP + 1] != pas[SP] ? 1 : 0);
						SP = SP + 1;
						
						// Printing
						stack[numOfStacks - 1]--;
						if (IR.M > 9)
							printf("\tNEQ %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tNEQ %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
					case 7:
						pas[SP + 1] = (pas[SP + 1] < pas[SP] ? 1 : 0);
						SP = SP + 1;
						
						// Printing
						stack[numOfStacks - 1]--;
						if (IR.M > 9)
							printf("\tLSS %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tLSS %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
					case 8:
						pas[SP + 1] = (pas[SP + 1] <= pas[SP] ? 1 : 0);
						SP = SP + 1;
						
						// Printing
						stack[numOfStacks - 1]--;
						if (IR.M > 9)
							printf("\tLEQ %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tLEQ %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
					case 9:
						pas[SP + 1] = (pas[SP + 1] > pas[SP] ? 1 : 0);
						SP = SP + 1;
						
						// Printing
						stack[numOfStacks - 1]--;
						if (IR.M > 9)
							printf("\tGTR %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tGTR %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
					case 10:
						pas[SP + 1] = (pas[SP + 1] >= pas[SP] ? 1 : 0);
						SP = SP + 1;
						
						// Printing
						stack[numOfStacks - 1]--;
						if (IR.M > 9)
							printf("\tGEQ %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tGEQ %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
					case 11:
						pas[SP] = pas[SP] % 2;
						
						// Printing
						stack[numOfStacks - 1]--;
						if (IR.M > 9)
							printf("\tODD %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tODD %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
				}
				break;
			// Load
			case 3:
				// To print
				l = IR.L;
				
				SP--;
				arb = BP; // arb = activation record base
				while ( IR.L > 0) //find base L levels down
				{
					arb = pas[arb];
					IR.L--;
				}
				pas[SP] = pas[arb - IR.M];
				
				// Printing
				stack[numOfStacks - 1]++;
				if (IR.M > 9)
					printf("\tLOD %d %d\t%d\t%d\t%d\t", l, IR.M, PC, BP, SP);
				else
					printf("\tLOD %d %d\t\t%d\t%d\t%d\t", l, IR.M, PC, BP, SP);
				break;
			// Store
			case 4:
				// To print
				l = IR.L;
				
				arb = BP; // arb = activation record base
				while (IR.L > 0) //find base L levels down
				{
					arb = pas[arb];
					IR.L--;
				}
				pas[arb - IR.M] = pas[SP];
				SP = SP + 1;
				
				// Printing
				stack[numOfStacks - 1]--;
				if (IR.M > 9)
					printf("\tSTO %d %d\t%d\t%d\t%d\t", l, IR.M, PC, BP, SP);
				else
					printf("\tSTO %d %d\t\t%d\t%d\t%d\t", l, IR.M, PC, BP, SP);
				break;
			// Call 
			case 5:
				// For printing
				stack[numOfStacks - 1] = BP - SP + 1;
				l = IR.L;
				
				arb = BP; // arb = activation record bases
				while (IR.L > 0) //find base L levels down
				{
					arb = pas[arb];
					IR.L--;
				}
				pas[SP - 1] = arb; //static link (SL)
				pas[SP - 2] = BP;	// dynamic link (DL)
				pas[SP - 3] = PC; //eturn address (RA)
				BP = SP - 1;
				PC = IR.M;
				
				// Printing
				if (IR.M > 9)
					printf("\tCAL %d %d\t%d\t%d\t%d\t", l, IR.M, PC, BP, SP);
				else
					printf("\tCAL %d %d\t\t%d\t%d\t%d\t", l, IR.M, PC, BP, SP);
				break;
			// Increment
			case 6:
				SP = SP - IR.M;
				
				// Printing
				numOfStacks++;
				stack[numOfStacks - 1] = IR.M;
				if (IR.M > 9)
					printf("\tINC %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
				else
					printf("\tINC %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
				break;
			// Jump
			case 7:
				PC = IR.M;
				
				// Printing
				if (IR.M > 9)
					printf("\tJMP %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
				else
					printf("\tJMP %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
				break;
			// Jump Conditional
			case 8:
				if (pas[SP] == 0)
				{
					PC = IR.M;
					SP++;
				}
				
				// Printing
				stack[numOfStacks - 1]--;
				if (IR.M > 9)
					printf("\tJPC %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
				else
					printf("\tJPC %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
				break;
			// System
			case 9:
				switch (IR.M)
				{
					case 1:
						printf("Output result is: %d\n", pas[SP]);
						SP++;
						
						// Printing
						stack[numOfStacks - 1]--;
						if (IR.M > 9)
							printf("\tSOU %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tSOU %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
					case 2:
						SP--;
						pas[SP] = getc(stdin);
						
						//Printing
						stack[numOfStacks - 1]++;
						if (IR.M > 9)
							printf("\tSOU %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tSOU %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
					case 3:
						eop = 0;
						
						//Printing
						if (IR.M > 9)
							printf("\tEOP %d %d\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						else
							printf("\tEOP %d %d\t\t%d\t%d\t%d\t", IR.L, IR.M, PC, BP, SP);
						break;
				}
				break;
		}
		
		// Printing stack
		int cur = 499;
		for (int i = 0; i < numOfStacks; i++)
		{
			
			for (int j = cur; j > cur - stack[i]; j--)
			{
				printf("%d ", pas[j]);
			}
			cur -= stack[i];
			if (i + 1 < numOfStacks)
				printf("| ");
		}
		printf("\n");
	}
	
	fclose(in);
	return 0;

}