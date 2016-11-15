/*  Gabriel Arias, Tuan Nguyen, Patrick Nolan, Aaron Cruz, Jesus Cortez, Anthony Fernandez
    cssc0627
    team Nevada
    prog2
    file_parser.h
    CS530, Fall 2016
*/

#ifndef MACROPP_ERROR_EXCEPTION_H
#define MACROPP_ERROR_EXCEPTION_H
#include <string>

using namespace std;

class macroPP_error_exception {
	public:
		macroPP_error_exception(string s){
			message = s;
		}
		
		macroPP_error_exception(){
			message = "An error has occured.";
		}
		
		string getMessage(){
			return message;
		}
		
		private:
			string message;
};

#endif
