#ifndef A_H
#define A_H
#include<iostream>
#include<string>
using namespace std;

int *hexatobinary(char[]);
int getcom(int[]);
void baseinfo(int[], string);
int calculator(int);
void printhexa(unsigned int, int,int);
void printmem(int);
int hexadec(char[]);

int PC = 0;
int reg[32]={0,};
int memory[65536] = { 0, };
int insmem[4096] = { 0, };

static string command[] =
{ "empty", "add", "sub", "addi", "lw", "sw", "lh", "lhu", "sh", "lb", "lbu", "sb" , "lui",
	"and", "or", "nor", "andi", "ori", "sll", "srl", "beq", "bne", "slt", "sltu", "slti", "sltiu",
	 "j", "jr", "jal"
};

class instruction {
private:
	char code[9];
	int bi[32];

public:
	string com;
	int rs;
	int rt;
	int rd;
	int shamt;
	int cons;
	int address;
	char RW;
	int current;

	void setins(char hexacode[]) { for (int i = 0; i < 9; i++) { code[i] = hexacode[i]; } }

	void getbinary(int bin[]) { for (int i = 0; i < 32; i++) { bi[i] = bin[i]; } }

	void printins() {
		cout << "Instruction: ";
		for (int i = 0; i < 8; i++) { cout << code[i]; }
		cout << endl;
	}
}; instruction pins[4096];




#endif
