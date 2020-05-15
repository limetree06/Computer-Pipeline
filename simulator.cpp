#include<iostream>
#include<string>
#include <fstream>
#include <vector>
#include<cstdlib>
#include<cmath>
#include "simulator.h"

using namespace std;
vector <int> reg31;
vector <string> input;
int mod = 0;
bool load = false;

int main(int argc, char *argv[]) {
	int cycle = atoi(argv[2]);
	int rc = 0;
	int count = 0;
	string get;
	string file(argv[1]);
	ifstream read;
	ifstream read1;
	
	if(argc == 4){
		string str = argv[3];
		if(str == "Mode: [01]" || str == "[01]" || str == "01" ) mod = 1;
		else mod = 0;}

	read.open(file.c_str());
	if (read.is_open()) {
		while (!read.eof()) {
			getline(read, get);
			input.push_back(get);
			count++;
		}
		read.close();
	}

	if (argc == 5) {
		string file1(argv[4]);
		read1.open(file1.c_str());
		if (read1.is_open()) {
			while (!read1.eof()) {
				getline(read1, get);
				reg[rc] = hex2dec(get);
				if (rc == 32) break;
				else rc++;
			}
			read1.close();
		}
	}

	int wbcount = -1;
	int memcount = -1;
	int excount = -1;
	int idcount = -1;
	int ifcount = 0;
	int exe =0 ;
	int box=0;
	bool stall = false;
	int size = input.size() - 1;

	for (int i = 0; i < cycle; i++) {
		WB(wbcount);
		MEM(memcount, stall);
		EX(excount, stall);
		ID(idcount, stall);

		if (stall && ifcount != -1) { box = ifcount; ifcount = -1; }
		else if (load) { box = ifcount; ifcount = -1; }
		else if (stall && ifcount == -1) ifcount = -1;
		else if (ifcount == -1 && stall == false) { ifcount = box + 1; exe_address++;}
		else if (ifcount == 0) { exe_address = 0; ifcount++;}
		else {exe_address++; ifcount++;}
		exe = exe_address;

		if (exe_address >= size) break;
		cout << "Cycle  " << i + 1 << endl;
		IF(box, ifcount, exe);

		printreg();
		printmemIO(memcount % 50);

		wbcount = memcount;
		memcount = excount;
		excount = idcount;
		idcount = ifcount;

		cout << endl;
	}
	return 0;
}

void WB(int cycle) {
	int index = cycle % 50;
	if (cycle < 0 || program[index].memwb.RegWrite == false) return;

	int finalreg = program[index].memwb.result;
	if (program[index].memwb.MemtoReg) { 
		if (program[index].memwb.width == 2) { 
			reg[finalreg] = (reg[finalreg] >> 16) << 16;
			reg[finalreg] += program[index].memwb.MEMresult; }

		else if (program[index].memwb.width == 1) {
			reg[finalreg] = (reg[finalreg] >> 8) << 8;
			reg[finalreg] += program[index].memwb.MEMresult; }

		else reg[finalreg] = program[index].memwb.MEMresult;
	
	}
	else { 
			reg[finalreg] = program[index].memwb.ALUresult; }
}

void MEM(int cycle, bool& stall) {
	if (cycle < 0) return;
	int index = cycle % 50;

	program[index].memwb.RegWrite = program[index].exmem.RegWrite;
	program[index].memwb.MemtoReg = program[index].exmem.MemtoReg;
	program[index].memwb.result = program[index].exmem.result;
	program[index].memwb.ALUresult = program[index].exmem.ALUresult;
	program[index].memwb.width = program[index].exmem.width;

	if (program[index].exmem.Branch == true) { 
		stall = false;
		exe_address = program[index].exmem.branchadd; }

	GetMem(index);

}

void EX(int cycle, bool& stall) {
	if (cycle < 0) return;
	int index = cycle % 50;

	program[index].exmem.Branch = program[index].idex.Branch;
	program[index].exmem.MemRead = program[index].idex.MemRead;
	program[index].exmem.MemWrite = program[index].idex.MemWrite;
	program[index].exmem.RegWrite = program[index].idex.RegWrite;
	program[index].exmem.MemtoReg = program[index].idex.MemtoReg;
	program[index].exmem.width = program[index].idex.width;

	if(mod==1) DataHazard(cycle);
	ALU(index, stall);

	if (program[index].idex.loaduse) load = true;
	else load = false;
}

void ID(int cycle, bool& stall) {
	if (cycle < 0) return;
	cycle = cycle % 50;

	program[cycle].registermem.command = com[getcom(program[cycle].ifid.binarycode)];
	baseinfo(program[cycle].ifid.binarycode, program[cycle].registermem.command, cycle);

	program[cycle].idex.regvalue1 = reg[program[cycle].registermem.rs];
	program[cycle].idex.regvalue2 = reg[program[cycle].registermem.rt];

	controller(cycle, stall);

}

void IF(int box, int cycle, int exe) {
	if (cycle == -1) cycle = box;

	else {initializeclass(cycle%50); 
		program[cycle % 50].execute = exe;
	}
	
	string get = input[exe];
	cout << "PC: ";
	printhexa(exe * 4, 4);

	cout << endl << "Instruction: " << get << endl;

	cycle = cycle % 50;

	const char *instruct = get.c_str();
	char instruction[8];
	for (int i = 0; i < 8; i++){ instruction[i] = instruct[i]; }

	int *binarycode = hexatobinary(instruction);
	program[cycle].ifid.getbinary(binarycode);
}

void initializeclass(int index) { 
	program[index].execute = 0;
	for(int i=0; i<32; i++){program[index].ifid.binarycode[i] = 0;}
	program[index].registermem.command = ' ';
	program[index].registermem.rs = 0;
	program[index].registermem.rt = 0;
	program[index].registermem.rd = 0;
	program[index].registermem.shamt = 0;
	program[index].registermem.constant = 0;
	program[index].registermem.address = 0;

	program[index].idex.width = 0;
	program[index].idex.unsign = false;
	program[index].idex.ALUOp = 11;
	program[index].idex.ALUSrc = false;
	program[index].idex.Jump = false;

	program[index].idex.Branch = false;
	program[index].idex.MemRead = false;
	program[index].idex.MemWrite = false;
	program[index].idex.RegWrite = false;
	program[index].idex.MemtoReg = false;

	program[index].idex.regvalue1 = 0;
	program[index].idex.regvalue2 = 0;

	program[index].exmem.MemValue = 0; // 메모리에 써야할값 (메모리에 써야할때)
	program[index].exmem.result = 0; // 값이 들어가야할 최종 레지스터 번호
	program[index].exmem.ALUresult = 0;
	program[index].exmem.branchadd = 0; //branch 일때의 주소
	program[index].exmem.Memadd = 0;


	program[index].exmem.Branch = false;
	program[index].exmem.MemRead = false; //load ture
	program[index].exmem.MemWrite = false; // store 
	program[index].exmem.width = 0;
	program[index].exmem.RegWrite = false; // Register 값을 변경하는 오퍼레이션일시 true 
	program[index].exmem.MemtoReg = false; // Register 값을 MEM_OUT으로 업데이트 true

	program[index].memwb.result = 0; // 값이 들어가야할 최종 레지스터 번호
	program[index].memwb.MEMresult = 0; // 메모리에서 load 해온 값
	program[index].memwb.ALUresult = 0;
	program[index].memwb.width = 0;
        program[index].memwb.RegWrite = false;
        program[index].memwb.MemtoReg = false;
}


void GetMem(int index) {
	int width = program[index].exmem.width;

	if (program[index].exmem.MemRead == true) {//L
		int MEMresult = datamemory[program[index].memwb.ALUresult];
		if (width == 2) {
			MEMresult = ((MEMresult << 16) >> 16);
			if (MEMresult < 0) MEMresult = (~(~MEMresult) % 0x00010000);
			else MEMresult = MEMresult % 0x00010000;
		}

		if (width == 1) {
			MEMresult = ((MEMresult << 24) >> 24);
			if (MEMresult < 0) MEMresult = (~(~MEMresult) % 0x00000100);
			else MEMresult = MEMresult % 0x00000100;
		}
		program[index].memwb.MEMresult = MEMresult;
	}

	if (program[index].exmem.MemWrite == true) {//S
		int MemValue = program[index].exmem.MemValue;
		int Memadd = program[index].exmem.Memadd;
		if (width == 4) { datamemory[Memadd] = MemValue; }

		else if (width == 2) {
			datamemory[Memadd] = ((datamemory[Memadd] >> 16) << 16);
			MemValue = ((MemValue << 16) >> 16);
			if (MemValue < 0) MemValue = (~(~MemValue) % 0x00010000);
			else MemValue = MemValue % 0x00010000;

			datamemory[Memadd] += MemValue;
		}

		else {
			datamemory[Memadd] = ((datamemory[Memadd] << 8) >> 8);
			MemValue = ((MemValue << 24) >> 24);
			if (MemValue < 0) MemValue = (~(~MemValue) % 0x00000100);
			else MemValue = MemValue % 0x00000100;

			datamemory[Memadd] += MemValue;
		}
		program[index].exmem.MemValue = MemValue;
	}
}

void DataHazard(int index) {
	if ( !(program[index + 1].exmem.RegWrite || program[index + 2].memwb.RegWrite) )	return;

	if (program[index+1].idex.MemRead && ((program[index+1].registermem.rt == program[index].registermem.rs ) || (program[index+1].registermem.rt == program[index].registermem.rt) )) program[index].idex.loaduse= true;

	if (program[index + 2].memwb.result != 0 || program[index + 1].exmem.result != 0 ) {

		if (program[index + 2].memwb.RegWrite && (program[index + 2].memwb.result != 0) && (program[index + 2].memwb.result == program[index].registermem.rs) ) {
			if (program[index].idex.ALUSrc) { // I
				if (program[index + 2].memwb.result == program[index].registermem.rs) program[index].idex.regvalue1 = program[index + 2].exmem.ALUresult;

				else {	if (program[index + 2].memwb.result == program[index].registermem.rs) program[index].idex.regvalue1 = program[index + 2].exmem.ALUresult;
						if (program[index + 2].memwb.result == program[index].registermem.rt) program[index].idex.regvalue2 = program[index + 2].exmem.ALUresult;	}
			}
		}


		else if (program[index + 1].exmem.RegWrite && (program[index + 1].exmem.result != 0) && (program[index + 1].exmem.result == program[index].registermem.rs)) {
			if (program[index].idex.ALUSrc) { // I
				if (program[index + 1].exmem.result == program[index].registermem.rs) program[index].idex.regvalue1 = program[index + 1].exmem.ALUresult; }

			else {
				if (program[index + 1].exmem.result == program[index].registermem.rs) program[index].idex.regvalue1 = program[index + 1].exmem.ALUresult;
				if (program[index + 1].exmem.result == program[index].registermem.rt) program[index].idex.regvalue2 = program[index + 1].exmem.ALUresult;
			}
		}
	}
}


void ALU(int index, bool& stall) {
	int ALUOp = program[index].idex.ALUOp;
	int value1 = program[index].idex.regvalue1;
	int value2 = program[index].idex.regvalue2;
	int shamt = program[index].registermem.shamt;
	int imm = program[index].registermem.constant;

	if (program[index].idex.ALUSrc == false) {
		program[index].exmem.result = program[index].registermem.rd;

		if (ALUOp == 0) { program[index].exmem.ALUresult = value1 & value2; }
		else if (ALUOp == 1) { program[index].exmem.ALUresult = value1 | value2; }
		else if (ALUOp == 2) { program[index].exmem.ALUresult = value1 + value2; }
		else if (ALUOp == 3) { program[index].exmem.ALUresult = value2 << shamt; }
		else if (ALUOp == 4) { program[index].exmem.ALUresult = value2 >> shamt; }
		else if (ALUOp == 6) { program[index].exmem.ALUresult = value1 - value2; }
		else if (ALUOp == 8) { program[index].exmem.ALUresult = ~(value1 | value2); }

		else if (ALUOp == 7) {
			if (program[index].idex.unsign == false) {
				if (value1 < value2) program[index].exmem.ALUresult = 1;
				else program[index].exmem.ALUresult = 0;
			}

			else {
				unsigned int val1 = value1;
				unsigned int val2 = value2;
				if (val1 < val2) program[index].exmem.ALUresult = 1;
				else program[index].exmem.ALUresult = 0;
			}
		}

		else return;
	}

	else {
		program[index].exmem.result = program[index].registermem.rt;

		if (ALUOp == 0) { program[index].exmem.ALUresult = imm & value1; }
		else if (ALUOp == 1) { program[index].exmem.ALUresult = imm | value1; }

		else if (ALUOp == 2) {
			if (program[index].idex.MemWrite == true) {
				if (program[index].idex.unsign == false) {
					program[index].exmem.MemValue = value2;
					program[index].exmem.Memadd = value1 + imm;
				}

				else {
					unsigned int val1 = value1;
					unsigned int val2 = value2;
					unsigned int immm = imm;
					program[index].exmem.MemValue = val2;
					program[index].exmem.Memadd = val1 + immm;
				}
			} //S

			else {
				if (program[index].idex.unsign == false) {
					program[index].exmem.ALUresult = imm + value1; } // L + ADD 

				else {
					unsigned int val1 = value1;
					unsigned int immm = imm;
					program[index].exmem.ALUresult = immm + val1;
				}
			}
		}

		else if (ALUOp == 5) {
			program[index].exmem.ALUresult = imm << 16;
		}

		else if (ALUOp == 6) {
			if (program[index].registermem.command == "beq") {
				if (value1 == value2) {program[index].exmem.branchadd = program[index].execute + imm +1 ;
				}

				else { program[index].exmem.Branch = false; stall = false; }
			}

			else {
				if (value1 != value2) {
					program[index].exmem.branchadd = program[index].execute + imm +1;
				}
				else { program[index].exmem.Branch = false; stall = false; }
			}
		}

		else if (ALUOp == 7) {
			if (program[index].idex.unsign == false) {
				if (value1 < imm) program[index].exmem.ALUresult = 1;
				else program[index].exmem.ALUresult = 0;
			}

			else {
				unsigned int immm = imm;
				unsigned int val1 = value1;
				if (val1 < immm) program[index].exmem.ALUresult = 1;
				else program[index].exmem.ALUresult = 0;
			}
		}
		else return;
	}
}

void controller(int index, bool& stall) {
	string com = program[index].registermem.command;
	if (com == "add") {
		program[index].idex.ALUOp = 2;
		program[index].idex.RegWrite = true;
		if (program[index].registermem.rs == 0 && program[index].registermem.rt == 0) reg [program[index].registermem.rd] = 0;
	}

	else if (com == "sub") {
		program[index].idex.ALUOp = 6;
		program[index].idex.RegWrite = true;
		if (program[index].registermem.rs == 0 && program[index].registermem.rt == 0) reg[program[index].registermem.rd] = 0;
	}

	else if (com == "addi") {
		program[index].idex.ALUOp = 2;
		program[index].idex.ALUSrc = true;
		program[index].idex.RegWrite = true;
		if (program[index].registermem.rs == 0) reg[program[index].registermem.rt] = program[index].registermem.constant;
	}

	else if (com == "lw") {
		program[index].idex.ALUOp = 2;
		program[index].idex.ALUSrc = true;
		program[index].idex.MemRead = true;
		program[index].idex.MemtoReg = true;
		program[index].idex.RegWrite = true;
		program[index].idex.width = 4;
		if (program[index].registermem.rs == 0) reg[program[index].registermem.rt] = datamemory[program[index].registermem.constant];

	}

	else if (com == "sw") {
		program[index].idex.ALUOp = 2;
		program[index].idex.ALUSrc = true;
		program[index].idex.MemWrite = true;
		program[index].idex.width = 4;
	}

	else if (com == "lh") {
		program[index].idex.ALUOp = 2;
		program[index].idex.ALUSrc = true;
		program[index].idex.MemRead = true;
		program[index].idex.MemtoReg = true;
		program[index].idex.RegWrite = true;
		program[index].idex.width = 2;

		if (program[index].registermem.rs == 0) {
			int MEMresult = datamemory[program[index].registermem.constant];
			MEMresult = ((MEMresult << 16) >> 16);
			if (MEMresult < 0) MEMresult = (~(~MEMresult) % 0x00010000);
			else MEMresult = MEMresult % 0x00010000;

			reg[program[index].registermem.rt] = (reg[program[index].registermem.rt] >> 16) << 16;
			reg[program[index].registermem.rt] += MEMresult;
		}
	}

	else if (com == "lhu") {
		program[index].idex.ALUOp = 2;
		program[index].idex.ALUSrc = true;
		program[index].idex.MemRead = true;
		program[index].idex.MemtoReg = true;
		program[index].idex.RegWrite = true;
		program[index].idex.width = 2;
		program[index].idex.unsign = true;
		if (program[index].registermem.rs == 0) {
			int MEMresult = datamemory[program[index].registermem.constant];
			MEMresult = ((MEMresult << 16) >> 16);
			if (MEMresult < 0) MEMresult = (~(~MEMresult) % 0x00010000);
			else MEMresult = MEMresult % 0x00010000;

			reg[program[index].registermem.rt] = (reg[program[index].registermem.rt] >> 16) << 16;
			reg[program[index].registermem.rt] += MEMresult;
		}
	}

	else if (com == "sh") {
		program[index].idex.ALUOp = 2;
		program[index].idex.ALUSrc = true;
		program[index].idex.MemWrite = true;
		program[index].idex.width = 2;
	}

	else if (com == "lb") {
		program[index].idex.ALUOp = 2;
		program[index].idex.ALUSrc = true;
		program[index].idex.MemRead = true;
		program[index].idex.MemtoReg = true;
		program[index].idex.RegWrite = true;
		program[index].idex.width = 1;

		if (program[index].registermem.rs == 0) {
			int MEMresult = datamemory[program[index].registermem.constant];
			MEMresult = ((MEMresult << 24) >> 24);
			if (MEMresult < 0) MEMresult = (~(~MEMresult) % 0x00000100);
			else MEMresult = MEMresult % 0x00000100;
			reg[program[index].registermem.rt] += MEMresult;

			reg[program[index].registermem.rt] = (reg[program[index].registermem.rt] >> 8) << 8;
			reg[program[index].registermem.rt] += MEMresult;
		}
	}

	else if (com == "lbu") {
		program[index].idex.ALUOp = 2;
		program[index].idex.ALUSrc = true;
		program[index].idex.MemRead = true;
		program[index].idex.MemtoReg = true;
		program[index].idex.RegWrite = true;
		program[index].idex.width = 1;
		program[index].idex.unsign = true;

		if (program[index].registermem.rs == 0) {
			int MEMresult = datamemory[program[index].registermem.constant];
			MEMresult = ((MEMresult << 24) >> 24);
			if (MEMresult < 0) MEMresult = (~(~MEMresult) % 0x00000100);
			else MEMresult = MEMresult % 0x00000100;
			reg[program[index].registermem.rt] += MEMresult;

			reg[program[index].registermem.rt] = (reg[program[index].registermem.rt] >> 8) << 8;
			reg[program[index].registermem.rt] += MEMresult;
		}
	}

	else if (com == "sb") {
		program[index].idex.ALUOp = 2;
		program[index].idex.ALUSrc = true;
		program[index].idex.MemWrite = true;
		program[index].idex.width = 1;
	}

	else if (com == "lui") {
		program[index].idex.ALUOp = 5;
		program[index].idex.ALUSrc = true;
		program[index].idex.RegWrite = true;
	}

	else if (com == "and") {
		program[index].idex.ALUOp = 0;
		program[index].idex.RegWrite = true;
	}

	else if (com == "or") {
		program[index].idex.ALUOp = 1;
		program[index].idex.RegWrite = true;
	}

	else if (com == "nor") {
		program[index].idex.ALUOp = 8;
		program[index].idex.RegWrite = true;
	}

	else if (com == "andi") {
		program[index].idex.ALUOp = 0;
		program[index].idex.RegWrite = true;
		if (program[index].registermem.rs == 0) reg[program[index].registermem.rt] = 0 & program[index].registermem.constant;
	}

	else if (com == "ori") {
		program[index].idex.ALUOp = 1;
		program[index].idex.ALUSrc = true;
		program[index].idex.RegWrite = true;
		if (program[index].registermem.rs == 0) reg[program[index].registermem.rt] = 0 | program[index].registermem.constant;
	}

	else if (com == "sll") {
		program[index].idex.ALUOp = 3;
		program[index].idex.RegWrite = true;
		if (program[index].registermem.rs == 0) reg[program[index].registermem.rt] = 0;
	}

	else if (com == "srl") {
		program[index].idex.ALUOp = 4;
		program[index].idex.RegWrite = true;
		if (program[index].registermem.rs == 0) reg[program[index].registermem.rt] = 0;
	}

	else if (com == "beq") {
		program[index].idex.ALUOp = 6;
		program[index].idex.Branch = true;
		program[index].idex.ALUSrc = true;
		stall = true;
	}

	else if (com == "bne") {
		program[index].idex.ALUOp = 6;
		program[index].idex.Branch = true;
		program[index].idex.ALUSrc = true;
		stall = true;
	}

	else if (com == "slt") {
		program[index].idex.ALUOp = 7;
		program[index].idex.RegWrite = true;
	}

	else if (com == "sltu") {
		program[index].idex.ALUOp = 7;
		program[index].idex.RegWrite = true;
		program[index].idex.unsign = true;
	}

	else if (com == "slti") {
		program[index].idex.ALUOp = 7;
		program[index].idex.ALUSrc = true;
		program[index].idex.RegWrite = true;
	}

	else if (com == "sltiu") {
		program[index].idex.ALUOp = 7;
		program[index].idex.ALUSrc = true;
		program[index].idex.RegWrite = true;
		program[index].idex.unsign = true;
	}

	else if (com == "j") { 
		program[index].idex.Jump = true; 
		if (stall ==false)exe_address = program[index].registermem.address - 1;}

	else if (com == "jr") {
		program[index].idex.Jump = true;
		program[index].idex.ALUOp = 10; 
		if(stall == false) {exe_address = reg31.back();
					reg31.pop_back();}
}

	else if (com == "jal") { 
		program[index].idex.ALUOp = 9;
		if(stall == false){exe_address = program[index].registermem.address - 1;
		reg31.push_back(program[index].execute);}
	}
	else if (com == "end") { exe_address += 100000000; }

	else return;
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

void baseinfo(int bin[], string com, int cycle) {
	int mem[6];
	mem[0] = 0; mem[1] = 0; mem[2] = 0; mem[3] = 0; mem[4] = 0; mem[5] = 0;
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
	program[cycle].registermem.rs = mem[0];
	program[cycle].registermem.rt = mem[1];
	program[cycle].registermem.rd = mem[2];
	program[cycle].registermem.shamt = mem[3];
	program[cycle].registermem.constant = mem[4];
	program[cycle].registermem.address = mem[5];
};

void printhexa(unsigned int dec, int length) {
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
}

void printreg() {
	cout << "Registers:" << endl;
	for (int i = 0; i < 32; i++) {
		cout << "[" << i << "] ";
		printhexa(reg[i], 8);
		cout << endl;
	}
}


void printmemIO(int index) {
	cout << "Memory I/O: ";
	int width = program[index].exmem.width;

	if (program[index].exmem.MemRead) { 
		cout << "R " <<  program[index].exmem.width << " ";
		printhexa(program[index].exmem.ALUresult , 4);
		cout << " ";
		printhexa(program[index].memwb.MEMresult, width * 2);
		cout << endl;
	}

	else if (program[index].exmem.MemWrite) { 
		cout << "W " << program[index].exmem.width << " ";
		printhexa(program[index].exmem.Memadd, 4);
		cout << " ";
		printhexa(program[index].exmem.MemValue, width * 2);
		cout << endl;
	}
	else cout << endl;
}

int hex2dec(string hex)
{
	int len = hex.length();
	int base = 1;
	int ret = 0;
	for (int i = len - 1; i >= 0; i--) {
		if (hex[i] >= '0' && hex[i] <= '9') ret += (hex[i] - 48) * base;
		else if (hex[i] >= 'A' && hex[i] <= 'F') ret += (hex[i] - 55) * base;
		else if (hex[i] >= 'a' && hex[i] <= 'f') ret += (hex[i] - 87) * base;
		base *= 16;
	}
	return ret;
}
