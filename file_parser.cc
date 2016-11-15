/*  Gabriel Arias, Tuan Nguyen, Patrick Nolan 
    cssc0598
    team Nevada
    prog1
    file_parser.cc
    CS530, Fall 2016
*/


#include "file_parser.h"
using namespace std;

  // ctor, filename is the parameter.  A driver program will read
  // the filename from the command line, and pass the filename to
  // the file_parser constructor.  Filenames must not be hard-coded.
file_parser::file_parser(string s) {
  filename = s;
  number_of_lines = 0;
  delim = ' ';
}
        
  // dtor
file_parser::~file_parser() {
}
        
  // reads the source file, storing the information is some
  // auxiliary data structure you define in the private section. 
  // Throws a file_parse_exception if an error occurs.
  // if the source code file fails to conform to the above
  // specification, this is an error condition.     
void file_parser::read_file() {
  ifstream input;
  string line;
  Row row;
  try{
    input.open(filename.c_str(),ios::in);
    if(!input)
      throw file_parse_exception("Could not open the file.");
    while(getline(input, line)) {
      number_of_lines ++;
      replace(line.begin(), line.end(), '\t', ' ');
      replace(line.begin(), line.end(), '\r', ' ');
      lines.push_back(line);
      row = tokenize(line);
      contents.push_back(row);
    }
    input.close();
  } catch(file_parse_exception e) {
    stringstream s;
    s << "file_parse_exception: " << e.getMessage() << endl;
    throw file_parse_exception(s.str());
  }
}
        
  // returns the token found at (row, column).  Rows and columns
  // are zero based.  Returns the empty string "" if there is no 
  // token at that location. column refers to the four fields
  // identified above.
string file_parser::get_token(unsigned int row, unsigned int col) {
  if(col >= 4)
    throw file_parse_exception("Column must be between 0 and 3.");
  if(row > number_of_lines)
    throw file_parse_exception("Row number out of bounds.");

  switch(col) {
  case 0:
    return contents[row].label;
  case 1:
    return contents[row].opcode;
  case 2:
    return contents[row].operand;
  case 3:
    return contents[row].comment;
  }
}
        
  // prints the source code file to stdout.  Should not repeat 
  // the exact formatting of the original, but uses tabs to align
  // similar tokens in a column. The fields should match the 
  // order of token fields given above (label/opcode/operands/comments)
void file_parser::print_file() {
  string label, opcode, operand, comment;
  for(int i = 0; i < number_of_lines; i++) {
    label = get_token(i, 0);
    opcode = get_token(i, 1);
    operand = get_token(i, 2);
    comment = get_token(i, 3);
    if(label == "" && opcode == "" && operand == "") cout << comment << endl;
    else {
      cout << label << "\t"  << opcode << "\t" << operand << "\t\t" << comment << endl;
    }
  }
}  
        
  // returns the number of lines in the source code file
int file_parser::size() {
  return number_of_lines;
}  

file_parser::Row file_parser::tokenize(string line) {
  Row row;
  
  if(line.empty()) return row;

  row.comment = parse_comment(line);
  if(row.comment != "") return row; 
  row.label = parse_label(line);
  line.erase(0, line.find_first_not_of(delim));

  row.comment = parse_comment(line);
  if(row.comment != "" || line[0] == '\0') return row;
  
  row.opcode = parse_opcode(line);
  line.erase(0, line.find_first_not_of(delim));

  row.comment = parse_comment(line);
  if(row.comment != "" || line[0] == '\0') return row;
      
  row.operand = parse_operand(line);
  line.erase(0, line.find_first_not_of(delim));
  if(line[0] == '\0') return row;

  if(line[0] != '*' && line[0] != ';' && line[0] != delim) {
    stringstream s;
    s << "Line " << number_of_lines << ": too many tokens.";
    throw file_parse_exception(s.str());
  }
  row.comment = parse_comment(line);
  return row;
}

string file_parser::parse_label(string& line) {
  string label = "";
  int count;
  if(line.substr(0, line.find_first_of(delim)) == "#minclude") {
    label = line.substr(0, line.find_first_of(delim));
    line.erase(0, line.find_first_of(delim));
    return label;
  } else if(isalpha(line[0]) || line[0] == '.') {
    count = 0;
    while(count < 12 && (isalpha(line[0]) || isdigit(line[0]) || line[0] == ':'
			 || line[0] == '.' || line[0] == '\\' || line[0] == '_' || line[0] == '@') && line[0] != delim) {
      label += line[0];
      line.erase(0, 1);
      count++;
    }
    if(count >= 12) line.erase(0, 1);
    if(!isalpha(line[0]) && !isdigit(line[0] && line[0] != '.') && line[0] != ':'
       && line[0] != '\\' && line[0] != '_' && line[0] != '@' && line[0] != delim && line[0] != '\0') {
      stringstream s;
      s << "Line " << number_of_lines << ": invalid label.";
      throw file_parse_exception(s.str());
    }
    return label;
  } else if(line[0] != delim) {
    stringstream s;
    s << "Line " << number_of_lines << ": invalid label.";
    throw file_parse_exception(s.str());
  }
  return label;
}

string file_parser::parse_opcode(string& line) {
  string opcode = "";
  size_t found_delim = line.find_first_of(delim);
  size_t found_quote = line.find_first_of('\'');
  while(found_quote < found_delim && found_quote != string::npos) {
    opcode += parse_quoted_string(line);
    found_delim = line.find_first_of(delim);
    found_quote = line.find_first_of('\'');
  }
  if(line[0] != delim && line[0] != '\0') {
    opcode += line.substr(0, line.find_first_of(delim));
    line.erase(0, line.find_first_of(delim));
    return opcode;
  }
  return opcode;
}

string file_parser::parse_operand(string& line) {
  string operand = "";
  size_t found_delim = line.find_first_of(delim);
  size_t found_quote = line.find_first_of('\'');
  while(found_quote != string::npos && found_quote < found_delim) {
    operand += parse_quoted_string(line);
    found_delim = line.find_first_of(delim);
    found_quote = line.find_first_of('\'');
  }
  if(line[0] != delim && line[0] != '\0') {   
    operand += line.substr(0, line.find_first_of(delim));
    line.erase(0, line.find_first_of(delim));
  }
  return operand;
}
    
string file_parser::parse_comment(string line) {
  string comment = "";
  if(line[0] == '*' || line[0] == ';') {
    comment = line;
    return comment;
  }
  return comment;
}

string file_parser::parse_quoted_string(string& line) {
  size_t found;
  string token = "";
  if(line[0] == '\'') {
    token = line.substr(0, 1);
    line.erase(0, 1);
    found = line.find_first_of('\'', 0);
    if(found == string::npos) return token;
    token += line.substr(0, found);
    line.erase(0, found);
    return token;
  } else if(line[0] != delim && line[0] != '\0') {
    token = line.substr(0, line.find_first_of('\''));
    line.erase(0, line.find_first_of('\''));
  }
  return token;
}




