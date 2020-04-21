#include <iostream>
#include <fstream>
#include<string>
#include <vector>
#include<cmath>
#include<cstdlib>
#include "a.h"

using namespace std;


vector <int> reg31;

int mem[6];

int main(int argc, char *argv[]) {
		int cycle = atoi(argv[2]);
		int count = 0;
		int rc = 0;
		int mod = 2;
		string file(argv[1]);
		ifstream read;
		ifstream read1;
		

		if(argc == 4){
				string str = argv[3];
				if(str =="[01]"||str=="01"||str=="Mode:[01]") mod =1;
				else mod =0;}

		read.open(file.c_str());
		if (read.is_open()) {
			while (!read.eof()) {
				pins[count].current = 4*count;
				insmem[pins[count].current] = count;
				char hexacode[9];
				read.getline(hexacode, 9);
				pins[count].setins(hexacode);
				int *bin = hexatobinary(hexacode);
				pins[count].getbinary(bin);
				pins[count].com = command[getcom(bin)];
				baseinfo(bin, pins[count].com);
				pins[count].rs = mem[0];
				pins[count].rt = mem[1];
				pins[count].rd = mem[2];
				pins[count].shamt= mem[3];
				pins[count].cons = mem[4];
				pins[count].address= mem[5];
				count++; }
		read.close();
	}
		
		 if (argc == 5){
                        string file1(argv[4]);
                        read1.open(file1.c_str());
                        if (read1.is_open()) {
                                while (!read1.eof()) {
                                        char regis[9];
                                        read1.getline(regis, 9);
                                        reg[rc] = hexadec(regis);
                                        if (rc == 32) break;
                                        else rc++;
                                }
                                read1.close();
                        }
                }



		int exe = 0;
		int calcount = 0;
		while (exe < cycle){
			int pre;
			cout << "Cycle " << ++exe << endl;
			cout << "PC: ";
			printhexa(pins[calcount].current,4,0);
			pins[calcount].printins();
			pre = calcount;
			calcount = calculator(pre);
			if(argc > 3) printmem(calcount);
			if (count == calcount) break;
			cout << endl;
		}
		return 0;
}

void printmem(int calcount) {
	char hexa[8];
	cout<<"Registers:"<<endl;
	for (int i = 0; i < 32; i++) {
		hexa[0] = '0'; hexa[1] = '0'; hexa[2] = '0'; hexa[3] = '0';
		hexa[4] = '0'; hexa[5] = '0'; hexa[6] = '0'; hexa[7] = '0';
		cout << "[" << i << "] ";
		printhexa(reg[i], 8,0);
	}

	cout << "Memory I/O: ";
	if (pins[calcount].com == "lw") {
		cout << "R " << "8 ";
		printhexa(reg[pins[calcount].rs] + pins[calcount].cons, 4,1);
		cout << " ";
		printhexa(pins[calcount].cons, 8,0);
	}

	else if (pins[calcount].com == "sw") {
		cout << "W " << "8 ";
		printhexa(reg[pins[calcount].rt] + pins[calcount].cons, 4,1);
		cout <<" ";
		printhexa(pins[calcount].cons, 8,0);
	}

	else if (pins[calcount].com == "lh") {
		cout << "R " << "4 ";
		printhexa(reg[pins[calcount].rs] + pins[calcount].cons, 4,1);
		cout << " ";
		printhexa(pins[calcount].cons, 4,0);
	}

	else if (pins[calcount].com == "lhu") {
		int a = pins[calcount].cons;
		if (pins[calcount].cons < 0) {
			a = (~pins[calcount].cons) + 1;
		}
		cout << "R " << "4 ";
		printhexa(reg[pins[calcount].rs] + a, 4,1);
		cout << " ";
		printhexa(pins[calcount].cons, 4,0);
	}

	else if (pins[calcount].com == "sh") {
		cout << "W " << "4 ";
		printhexa(pins[calcount].cons + reg[pins[calcount].rt], 4,2);
		cout << " ";
		printhexa(pins[calcount].cons, 4,0);
	}
	else if (pins[calcount].com == "lb") {
		cout << "R " << "2 ";
		printhexa(reg[pins[calcount].rs] + pins[calcount].cons, 4,1);
		cout << " ";
		printhexa(pins[calcount].cons, 2,0);
	}

	else if (pins[calcount].com == "lbu") {
		int a = pins[calcount].cons;
		if (pins[calcount].cons < 0) {
			a = (~pins[calcount].cons) + 1;
		}
		cout << "R " << "2 ";
		printhexa(reg[pins[calcount].rs] + a, 4,1);
		cout << " ";
		printhexa(pins[calcount].cons, 2,0);
	}

	else if (pins[calcount].com == "sb") {
		cout << "W " << "2 ";
		printhexa(reg[pins[calcount].rt] + pins[calcount].cons, 4,1);
		cout << " ";
		printhexa(pins[calcount].cons, 2,0);
	}
	else cout << " " << endl;
	cout << endl;
}





int hexadec(char hexareg[]) {
	int value = 0;
	int *bi = hexatobinary(hexareg);
	for (int i = 31; i > 0; i--) {
		value += bi[i] * (int)pow(2, 31 - i);
	}
	if (bi[0] == 1) { value -= (int)pow(2, 31); }

	return value;
}

	
void printhexa(unsigned int dec, int length,int a) {
	char *hexa = new char[length];
	for (int i = 0; i < length; i++) { hexa[i] = '0'; }
	int pos = 0;

	while (1) {
		unsigned int mod = dec % 16;
		if (mod < 10) { hexa[pos] = '0' + mod; }
		else hexa[pos] = 'A' + (mod - 10);

		pos++;
		dec = dec / 16;
		if (dec == 0) break;
	}
	for (int i = length - 1; i >= 0; i--) { cout << hexa[i]; }
	delete[] hexa;
	if(a!=1) {cout << endl;}
}

int calculator(int calcount) {
	string com = pins[calcount].com;
	if (com == "add") { reg[pins[calcount].rd] = reg[pins[calcount].rs] + reg[pins[calcount].rt]; }
	else if (com == "sub") { reg[pins[calcount].rd] = reg[pins[calcount].rs] - reg[pins[calcount].rt]; }
	else if (com == "addi") { reg[pins[calcount].rt] = reg[pins[calcount].rs] + pins[calcount].cons; }
	else if (com == "lw") { reg[pins[calcount].rt] = memory[reg[pins[calcount].rs] + pins[calcount].cons]; }
	else if (com == "sw") { memory[reg[pins[calcount].rt] + pins[calcount].cons] = reg[pins[calcount].rt]; }
	else if (com == "lh") {
		short int b = memory[reg[pins[calcount].rs] + pins[calcount].cons];
		reg[pins[calcount].rt] = b;
	}

	else if (com == "lhu") {
		unsigned int a = pins[calcount].cons;
		short int b = memory[reg[pins[calcount].rs] + a];
		reg[pins[calcount].rt] = b;
	}

	else if (com == "sh") {
		short int b = reg[pins[calcount].rs];
		memory[pins[calcount].cons + reg[pins[calcount].rt]] = b;
	}

	else if (com == "lb") {
		reg[pins[calcount].rt] = memory[reg[pins[calcount].rs] + pins[calcount].cons] % 256;
	}

	else if (com == "lbu") {
		unsigned int a = pins[calcount].cons;
		reg[pins[calcount].rt] = memory[reg[pins[calcount].rs] + a] % 256;
	}

	else if (com == "sb") {
		int b = reg[pins[calcount].rs];
		memory[reg[pins[calcount].rt] + pins[calcount].cons] = b % 256;
	}

	else if (com == "lui") {
		int a = (reg[pins[calcount].rs] + pins[calcount].cons) / (int)pow(2, 16);
		reg[pins[calcount].rt] = memory[a];
	}

	else if (com == "and") { reg[pins[calcount].rd] = (reg[pins[calcount].rs] & reg[pins[calcount].rt]); }
	else if (com == "or") { reg[pins[calcount].rd] = (reg[pins[calcount].rs] | reg[pins[calcount].rt]); }
	else if (com == "nor") { reg[pins[calcount].rd] = (~(reg[pins[calcount].rs] | reg[pins[calcount].rt])); }
	else if (com == "andi") { reg[pins[calcount].rt] = (reg[pins[calcount].rs] & pins[calcount].cons); }
	else if (com == "ori") { reg[pins[calcount].rt] = (reg[pins[calcount].rs] | pins[calcount].cons); }
	else if (com == "sll") { reg[pins[calcount].rd] = (reg[pins[calcount].rt] << pins[calcount].shamt); }
	else if (com == "srl") { reg[pins[calcount].rd] = (reg[pins[calcount].rt] >> pins[calcount].shamt); }
	else if (com == "beq") {
		if (reg[pins[calcount].rs] == reg[pins[calcount].rt]) {
			return calcount = insmem[pins[calcount].current + 4 * pins[calcount].cons + 4];
		}
	}

	else if (com == "bne") {
		if (reg[pins[calcount].rt] != reg[pins[calcount].rs]) {
			return calcount = insmem[pins[calcount].current + 4 * pins[calcount].cons + 4];
		}
	}

	else if (com == "slt") {
		if (reg[pins[calcount].rs] < reg[pins[calcount].rt]) { reg[pins[calcount].rd] = 1; }
		else reg[pins[calcount].rd] = 0;
	}
	else if (com == "sltu") {
		unsigned int a = reg[pins[calcount].rs];
		unsigned int b = reg[pins[calcount].rt];

		if (a < b) { reg[pins[calcount].rd] = 1; }
		else reg[pins[calcount].rd] = 0;
	}

	else if (com == "slti") {
		if (reg[pins[calcount].rt] < pins[calcount].cons) { reg[pins[calcount].rd] = 1; }
		else reg[pins[calcount].rd] = 0;
	}

	else if (com == "sltiu") {
		unsigned int a = reg[pins[calcount].rt];
		unsigned int b = reg[pins[calcount].cons];
		if (a < b) { reg[pins[calcount].rd] = 1; }
		else reg[pins[calcount].rd] = 0;
	}

	else if (com == "j") { return insmem[(4*pins[calcount].address)]; }
	else if (com == "jr") {
		calcount = insmem[reg[31]];
		reg31.pop_back();
	}

	else if (com == "jal") {
		reg31.push_back(pins[calcount].current);
		reg[31] = reg31.back();
		return insmem[(4*pins[calcount].address)];
	}

	else return calcount + 1;

	return calcount+1;
}




int *hexatobinary(char hexacode[]) {
	static int bin[32];
	for (int i = 0; i < 8; i++) {
		switch (hexacode[i]) {
		case '0': bin[4 * i + 0] = 0; bin[4 * i + 1] = 0; bin[4 * i + 2] = 0; bin[4 * i + 3] = 0; break;
		case '1': bin[4 * i + 0] = 0; bin[4 * i + 1] = 0; bin[4 * i + 2] = 0; bin[4 * i + 3] = 1; break;
		case '2': bin[4 * i + 0] = 0; bin[4 * i + 1] = 0; bin[4 * i + 2] = 1; bin[4 * i + 3] = 0; break;
		case '3': bin[4 * i + 0] = 0; bin[4 * i + 1] = 0; bin[4 * i + 2] = 1; bin[4 * i + 3] = 1; break;
		case '4': bin[4 * i + 0] = 0; bin[4 * i + 1] = 1; bin[4 * i + 2] = 0; bin[4 * i + 3] = 0; break;
		case '5': bin[4 * i + 0] = 0; bin[4 * i + 1] = 1; bin[4 * i + 2] = 0; bin[4 * i + 3] = 1; break;
		case '6': bin[4 * i + 0] = 0; bin[4 * i + 1] = 1; bin[4 * i + 2] = 1; bin[4 * i + 3] = 0; break;
		case '7': bin[4 * i + 0] = 0; bin[4 * i + 1] = 1; bin[4 * i + 2] = 1; bin[4 * i + 3] = 1; break;
		case '8': bin[4 * i + 0] = 1; bin[4 * i + 1] = 0; bin[4 * i + 2] = 0; bin[4 * i + 3] = 0; break;
		case '9': bin[4 * i + 0] = 1; bin[4 * i + 1] = 0; bin[4 * i + 2] = 0; bin[4 * i + 3] = 1; break;
		case 'a': bin[4 * i + 0] = 1; bin[4 * i + 1] = 0; bin[4 * i + 2] = 1; bin[4 * i + 3] = 0; break;
		case 'b':  bin[4 * i + 0] = 1; bin[4 * i + 1] = 0; bin[4 * i + 2] = 1; bin[4 * i + 3] = 1; break;
		case 'c': bin[4 * i + 0] = 1; bin[4 * i + 1] = 1; bin[4 * i + 2] = 0; bin[4 * i + 3] = 0; break;
		case 'd': bin[4 * i + 0] = 1; bin[4 * i + 1] = 1; bin[4 * i + 2] = 0; bin[4 * i + 3] = 1; break;
		case 'e': bin[4 * i + 0] = 1; bin[4 * i + 1] = 1; bin[4 * i + 2] = 1; bin[4 * i + 3] = 0; break;
		case 'f': bin[4 * i + 0] = 1; bin[4 * i + 1] = 1; bin[4 * i + 2] = 1; bin[4 * i + 3] = 1; break;
		case 'A': bin[4 * i + 0] = 1; bin[4 * i + 1] = 0; bin[4 * i + 2] = 1; bin[4 * i + 3] = 0; break;
		case 'B':  bin[4 * i + 0] = 1; bin[4 * i + 1] = 0; bin[4 * i + 2] = 1; bin[4 * i + 3] = 1; break;
		case 'C': bin[4 * i + 0] = 1; bin[4 * i + 1] = 1; bin[4 * i + 2] = 0; bin[4 * i + 3] = 0; break;
		case 'D': bin[4 * i + 0] = 1; bin[4 * i + 1] = 1; bin[4 * i + 2] = 0; bin[4 * i + 3] = 1; break;
		case 'E': bin[4 * i + 0] = 1; bin[4 * i + 1] = 1; bin[4 * i + 2] = 1; bin[4 * i + 3] = 0; break;
		case 'F': bin[4 * i + 0] = 1; bin[4 * i + 1] = 1; bin[4 * i + 2] = 1; bin[4 * i + 3] = 1; break;
		default: break;
		};
	}
	return bin;
};

int getcom(int bin[]) {
	int opcode = 0;
	for (int i = 5; i >= 0; i--) {
		opcode += bin[i] * (int)pow(2, 5 - i);
	}

	if (opcode == 0) {
		for (int i = 31; i >= 26; i--) {
			opcode += bin[i] * (int)pow(2, 31 - i);
		}

		switch (opcode) {
		case 32: return 1;
		case 34: return 2;
		case 36: return 13;
		case 37: return 14;
		case 39: return 15;
		case 0: return 18;
		case 2: return 19;
		case 42: return 22;
		case 43: return 23;
		case 8: return 27;
		default: return 0;
		}
	}

	else {
		switch (opcode) {
		case 8: return 3;
		case 35: return 4;
		case 43:return 5;
		case 33: return 6;
		case 37: return 7;
		case 41: return 8;
		case 32: return 9;
		case 36: return 10;
		case 40: return 11;
		case 15: return 12;
		case 12: return 16;
		case 13: return 17;
		case 4: return 20;
		case 5: return 21;
		case 10: return 24;
		case 11: return 25;
		case 2: return 26;
		case 3: return 28;
		default: return 0;
		}
	}
	return 0;
}

void baseinfo(int bin[], string com) {
	mem[0] =0; mem[1]=0; mem[2]=0; mem[3]=0; mem[4]=0; mem[5]=0;
	int address = 0, rs = 0, rt = 0, rd = 0, shamt = 0, cons = 0;

	if (com == "j" || com == "jal") {

		for (int i = 31; i >= 6; i--) {
			address += bin[i] * (int)pow(2, 31 - i);
		}
		
		mem[5] = address;
		}

	else {
		for (int i = 10; i >= 6; i--) {
			rs += bin[i] * (int)pow(2, 10 - i);
		}
		mem[0] = rs;

		for (int i = 15; i >= 11; i--) {
			rt += bin[i] * (int)pow(2, 15 - i);
		}
		mem[1] = rt;

		if (com == "add" || com == "sub" || com == "and" || com == "or"
			|| com == "nor" || com == "sll" || com == "srl" || com == "slt"
			|| com == "sltu" || com == "jr") {
			for (int i = 20; i >= 16; i--) {
				rd += bin[i] * (int)pow(2, 20 - i);
			}
			mem[2] = rd;

			if (com == "sll" || com == "srl") {
				for (int i = 25; i <= 21; i--) {
					shamt += bin[i] * (int)pow(2, 25 - i);
				}
				mem[3] = shamt;
			}
		}

		else {
			for (int i = 31; i >= 16; i--) {
				cons += bin[i] * (int)pow(2, 31 - i);
			}
			if (bin[16] == 1) { cons -= (int)pow(2, 16); }
			mem[4] = cons;
		}
	}
};
