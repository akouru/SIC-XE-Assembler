/*  Gabriel Arias, Tuan Nguyen, Patrick Nolan, Aaron Cruz, Jesus Cortez, Anthony Fernandez
    cssc0627
    team Nevada
    prog2
    macroPP.h
    CS530, Fall 2016
*/

#ifndef MACROPP_H
#define MACROPP_H

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <iomanip>
#include <sstream>
#include <stddef.h>
#include <algorithm>

using namespace std;


class macroPP{
	public:
		//filename as parameter
		macroPP(string);
		~macroPP();
		void print_macros();
	
	private:
		struct Row{
			string label;
			string opcode;
			string operand;
			string comment;
			
			Row(){
					label = "";
					opcode = "";
					operand = "";
					comment = "";
			}
		};
		
		struct macro_line{
			string label;
			string opcode;
			string operand;
			
			macro_line(){
				label = "";
				opcode = "";
				operand = "";
			}
		};
		
		struct macro{
			string name;
			int invocation_counter;
			vector<macro_line> definitions;
			vector<string*> params;
			
			macro(){
				name = "";
				invocation_counter = 0;
			}
		};
		
		vector<macro> macros;
		vector<Row> contents;
		string filename;
		void print_vector(vector<Row>); //for testing
		vector<Row> replace_mincludes(file_parser);
		vector<Row> get_contents(file_parser);
		vector<Row> transfer_macros(vector<Row>, vector<Row>, int);
		vector<Row> get_macros(vector<Row>);
		void print_macro();
		void create_file();
		
		vector<Row> replace_all(vector<Row>, string, string);
		void expand_all_macros();
		bool expand_macros();
		int expand_macro(int);
		string replace2(string, string, string);
		vector<Row> remove_mincludes(file_parser);
		
		
		
		
};

#endif