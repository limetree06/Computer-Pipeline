#ifndef SIMULATOR_H
#define SIMULATOR_H

#include<iostream>
#include<string>
using namespace std;

int *hexatobinary(char[]);
int getcom(int[]);
void printhexa(unsigned int, int);
int hex2dec(string);
void WB(int);
void MEM(int, bool&);
void EX(int, bool&);
void ID(int, bool&);
void IF(int,int, int);
void initializeclass(int);
void GetMem(int);
void ALU(int, bool&);
void controller(int, bool&);
int getcom(int[]);
void baseinfo(int[], string, int);
void printreg();
void printmemIO(int);
void DataHazard(int);

int reg[32] = { 0, };
int datamemory[65536] = { 0, };

int exe_address = 0;

static string com[] =
{ "end", "add", "sub", "addi", "lw", "sw", "lh", "lhu", "sh", "lb", "lbu", "sb" , "lui",
	"and", "or", "nor", "andi", "ori", "sll", "srl", "beq", "bne", "slt", "sltu", "slti", "sltiu",
	 "j", "jr", "jal"
};

class prog {
public: 
	int execute;

public:
	class IFID {
	public:
		int binarycode[32];
		void getbinary(int bin[]) { for (int i = 0; i < 32; i++) { binarycode[i] = bin[i]; } }
	}; IFID ifid;

	class IDEX {
	public:
		int width;
		bool unsign; // unsign = true;
		int ALUOp;
		bool ALUSrc; // i type = true
		bool Jump;
		int target;
		
		bool loaduse;
		bool Branch;
		bool MemRead; //load ture
		bool MemWrite; // store 

		bool RegWrite; // Register 값을 변경하는 오퍼레이션일시 true 
		bool MemtoReg; // Register 값을 MEM_OUT으로 업데이트 true

		int regvalue1;
		int regvalue2;

	}; IDEX idex;

	class EXMEM {
	public:
		int MemValue; // 메모리에 써야할값 (메모리에 써야할때)
		int  result; // 값이 들어가야할 최종 레지스터 번호
		int ALUresult;
		int  branchadd; //branch 일때의 주소
		int Memadd;

		bool Branch;
		bool MemRead; //load ture
		bool MemWrite; // store 

		int width;
		bool RegWrite; // Register 값을 변경하는 오퍼레이션일시 true 
		bool MemtoReg; // Register 값을 MEM_OUT으로 업데이트 true

	}; EXMEM exmem;

	class MEMWB {
	public:
		int  result; // 값이 들어가야할 최종 레지스터 번호
		int  MEMresult; // 메모리에서 load 해온 값
		int  ALUresult;

		int width;
		bool RegWrite; // Register 값을 변경하는 오퍼레이션일시 true 
		bool MemtoReg; // Register 값을 MEM_OUT으로 업데이트 true
	}; MEMWB memwb;

	class registermemory {
	public:
		string command;
		int rs; // read register 1
		int rt; // read register 2
		int rd; // write register 
		int shamt;
		int constant;
		int address;
	}; registermemory registermem;

}; prog program[51];

#endif
