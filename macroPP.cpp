/*  Gabriel Arias, Tuan Nguyen, Patrick Nolan, Aaron Cruz, Jesus Cortez, Anthony Fernandez
    cssc0627
    team Nevada
    prog2
    file_parser.cpp
    CS530, Fall 2016
*/


#include "file_parser.h"
#include "file_parse_exception.h"
#include "macroPP_error_exception.h"
#include "macroPP.h"


using namespace std;


macroPP::macroPP(string s){
	filename = s;
	file_parser parser(s);
	parser.read_file();
	contents = replace_mincludes(parser);
	contents = get_macros(contents);
	
	contents = remove_mincludes(parser);
	expand_all_macros();
	
	create_file();
}


macroPP::~macroPP(){
	
}

//replaces part of string line
string macroPP::replace2(string line, string replace, string with){
	size_t index = 0;
	while((index = line.find(replace, index)) != string::npos){
		line.replace(index, replace.length(), with);
		index = index + with.length();
	}
	return line;
}

//replaces all of the rows (except comments)
vector<macroPP::Row> macroPP::replace_all(vector<Row> v, string replace, string with){
	for(int i = 0; i < v.size(); i++){
		v[i].label = replace2( v[i].label, replace, with );
		v[i].opcode = replace2( v[i].opcode, replace, with );
		v[i].operand = replace2( v[i].operand, replace, with );
	}
	return v;
}

//calls expand_macros until the result is false and all macros are expanded
void macroPP::expand_all_macros(){
	while(expand_macros());
}

//returns index of the *end of macro expansion
int macroPP::expand_macro(int index){
	string name, params, size;
	int inv;
	vector<string> parameters;
	  
	name = contents[index].opcode;

	//get the size (word, byte, etc)
	size_t pos = name.find('.');
	if(pos != string::npos){
		size = name.substr(pos);
		name = name.substr(0, pos);
	}
	
	unsigned int found = 0;
	for(; found < macros.size(); found++){
		if(macros[found].name == name) break;
	}
	inv = macros[found].invocation_counter;
	macros[found].invocation_counter ++;
	
	params = contents[index].operand;
	
	stringstream sstream(params);
	string p;
	parameters.push_back(size);
	while(getline(sstream, p, ',')){
		parameters.push_back(p);
	}
	
	contents[index].label = "";
	contents[index].opcode = "";
	contents[index].operand = "";
	stringstream comment;
	comment << "*" << name << " " << params;
	contents[index].comment = comment.str();
	
	vector<Row> macro;
	
	vector<macro_line> def = macros[found].definitions;
	
	for(int i = 0; i < def.size(); i++){
		Row r;
		r.label = def[i].label;
		r.opcode = def[i].opcode;
		r.operand = def[i].operand;
		macro.push_back(r);
	}
	
	string s;
	stringstream ss;
	for(int i = 0; i < parameters.size(); i++){
		ss << "\\" << i;
		s = ss.str();
		macro = replace_all(macro, s, parameters[i]);
		ss.str("");
	}
	
	ss << "\\@";
	s = ss.str();
	ss.str("");
	ss << inv;
	macro = replace_all(macro, s, ss.str());
	
	Row r;
	r.comment = "*end of macro expansion";
	macro.push_back(r);
	
	contents.insert(contents.begin()+index+1, macro.begin(), macro.end());
	return index + macro.size();
}

//returns true if macros were expended and false if there were no macros to expand
bool macroPP::expand_macros(){
	bool macro_expanded = false;
	for(int i = 0; i < contents.size(); i++){
		bool isMacro = false;
		for(int j = 0; j < macros.size(); j++){
			if(contents[i].opcode == macros[j].name){
				isMacro = true;
				break;
			}
		}
		if(isMacro){
			i = expand_macro(i);
			macro_expanded = true;
		}
	}
	return macro_expanded;
}


//prints all the macro definitions in the contents vector include macro name and endm
void macroPP::print_macros(){
	for(int i = 0; i < macros.size(); i++){
		cout << macros[i].name << "\t" << "macro" << endl;
		string label, opcode, operand;
		for(int i = 0; i < macros[i].definitions.size(); i++){
			label = macros[i].definitions[i].label;
			opcode = macros[i].definitions[i].opcode;
			operand = macros[i].definitions[i].operand;
			cout << label << "\t" << opcode << "\t" << operand << endl;
		}
		cout << "\t" << "endm" << endl;
	}
}

// For each minclude in the original file
// Replace the minclude with the macro code in the file
vector<macroPP::Row> macroPP::replace_mincludes(file_parser parser){
	vector<Row> code = get_contents(parser);
	Row r;
	for(int i = 0; i < code.size(); i++){
		if(code[i].label == "#minclude"){
			try{
				vector<Row> macro_defs;
				file_parser p(code[i].opcode);
				p.read_file();
				macro_defs = get_contents(p);
				code.erase(code.begin()+i);
				code.insert(code.begin()+i, macro_defs.begin(), macro_defs.end());
				//code = transfer_macros(code, macro_defs, i);
			}catch(file_parse_exception e){
				stringstream s;
				s << "Line " << (i+1) << ": failed to load file. " << e.getMessage();
				throw macroPP_error_exception(s.str());
			}
		}
	}
	//print_vector(code);
	return code;
}

vector<macroPP::Row> macroPP::remove_mincludes(file_parser parser){
  	vector<Row> code = get_contents(parser);
	Row r;
	int i = 0;
	while(i < code.size()){
		if(code[i].label == "#minclude"){	
			code.erase(code.begin()+i);
		}else if(code[i].opcode == "macro"){
			while(code[i].opcode != "endm"){
				code.erase(code.begin()+i);
			}
			code.erase(code.begin()+i);
		}else{
		i++;
		}
	}
	return code;
}

vector<macroPP::Row> macroPP::transfer_macros(vector<Row> main, vector<Row> m, int index){
	int j = index+1;
	for(int i = 0; i < m.size(); i++){
		if(m[i].opcode == "macro"){
			int start = i;
			while(m[i].opcode != "endm"){
				i++;
			}
			main.insert(main.begin()+j, m.begin()+start, m.begin()+i);
			j++;
		}
	}
	main.erase(main.begin()+index);
}


vector<macroPP::Row> macroPP::get_contents(file_parser parser){
	vector<Row> vector;
	for(int i = 0; i < parser.size(); i++){
		Row r;
		r.label = parser.get_token(i, 0);
		r.opcode = parser.get_token(i, 1);
		r.operand = parser.get_token(i, 2);
		r.comment = parser.get_token(i, 3);
		vector.push_back(r);
	}
	return vector;
}


void macroPP::print_vector(vector<Row> v) {
	string label, opcode, operand, comment;
	for(int i = 0; i < v.size(); i++) {
		label = v[i].label;
		opcode = v[i].opcode;
		operand = v[i].operand;
		comment = v[i].comment;
		if(label == "" && opcode == "" && operand == "") cout << comment << endl;
		else {
			cout << label << "\t"  << opcode << "\t" << operand << "\t\t" << comment << endl;
		}
	}
}

// remove all code between macro and endm and store it in macro struct
// remove macro and endm
vector<macroPP::Row> macroPP::get_macros(vector<Row> v){
	int i = 0;
	vector<Row> c;
	while(i < v.size()){
		if(v[i].opcode == "macro" ){
			if(v[i].label == ""){
				stringstream s;
				s << "Line: " << i << "macro has no label" << endl;
				throw macroPP_error_exception(s.str());
			}
			//cout << "macro" << endl;
			macro m;
			m.name = v[i].label;
			i++;
			while(v[i].opcode != "endm"){
				if(i >= v.size()){
					stringstream s;
					s << "Line: " << i << "macro started but not ended" << endl;
					throw macroPP_error_exception(s.str());
				}
				macro_line line;
				line.label = v[i].label;
				line.opcode = v[i].opcode;
				line.operand = v[i].operand;
				m.definitions.push_back(line);
				i++;
			}
			macros.push_back(m);
		}else{
			c.push_back(v[i]);
		}
		i++;
	}
	//print_vector(c);
	return c;
}


//creates a file with the macro definitions removed and with .se at the end
void macroPP::create_file(){
	stringstream name;
	name << filename << "e";
	ofstream file(name.str().c_str());
	string label, opcode, operand, comment;
	for(int i = 0; i < contents.size(); i++){
		label = contents[i].label;
		opcode = contents[i].opcode;
		operand = contents[i].operand;
		comment = contents[i].comment;
		if(label == "" && opcode == "" && operand == "") file << comment << endl;
		else {
			file << label << "\t"  << opcode << "\t" << operand << "\t\t" << comment << endl;
		}
	}
	
	
} 

int main(int argc, char** argv){
	if(argc == 2){
		try{
			try{
				macroPP m(argv[1]);
				//m.print_macros();
				cout << "Macro Pre-processing complete."  << endl;
			}catch(macroPP_error_exception e){
				cout << "Error: " << e.getMessage() << endl;
				exit(1);
			}
		}catch(file_parse_exception e){
			cout << "Error: " << e.getMessage() << endl;
			exit(1);
		}
	}else{
		cout << "Please enter one and only one argument." << endl;
	}
	return 0;
}








