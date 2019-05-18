/*
Library for interpreting and sending JSON data in C
Dependent on the JSMN library by Serge Zaitsev distributed under the MIT license
Authors: Phillip Dix
Date: April 25, 2017
*/

#include "ECE631JSON.h"
#include "jsmn.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>


void RemoveEscapes(char* CheckString){
	int head;
	int tail=0;
	int InQuote = 0;
	char LastChar = 0x0;
	int Length = strlen(CheckString);
	for(head=0; head<Length;head++){
		char currChar = CheckString[head];
		if (currChar=='"'&& LastChar!='\\'){
			InQuote = (InQuote+1)%2;
		}
		LastChar = CheckString[head];
		if(!InQuote && CheckString[head]=='\\'){
			head++;
		}
		CheckString[tail]=CheckString[head];
		tail++;
	}
	CheckString[tail]=0x0;
}

void extract_value(char* JSONSTR, char* key, char *target, int numtok1, jsmntok_t *tokens1) {//extract the JSON value associated with the specified key string
//NOTE: this function does not parse MULTIPLE NESTED JSON LAYERS
//A nested JSON string will be returned as a string, not as its contained JSON values
//to parse nested strings, this function will need to be called multiple times
	char output1[256];
	int i;
	for (i = 0; i < numtok1; i++) {//Loop through all the tokens
		memcpy(output1, (const char*)&JSONSTR[tokens1[i].start], abs(tokens1[i].start - tokens1[i].end));//copy the token of interest into a separate string
		output1[abs(tokens1[i].start - tokens1[i].end)] = '\0';
		if (strcmp(output1, key) == 0) {//Check to see if the token matches the specified key
			memcpy(target, &JSONSTR[tokens1[i + 1].start], abs(tokens1[i + 1].start - tokens1[i + 1].end));//copy the response type information into the target string location
			target[abs(tokens1[i + 1].start - tokens1[i + 1].end)] = '\0';//terminate the target string
			if (tokens1[i].type==JSMN_STRING){
				RemoveEscapes(target);
			}
		}
	}
}

int pack_json(char* format, char* target, ...){//works similar to printf: example format string "{s:b,s:{s:[n,n,n,n]}}"  b is bool, n is float, s is string
	char* s;
	long numb;
	char m[100];
	
	va_list arg;//initialize list of extra arguments
	va_start(arg, target);

	int z = 0;
	int j;
	for (j = 0; j < strlen(format); j++) {
		if ((format[j] == '{') || (format[j] == '}') || (format[j] == ':') || (format[j] == ',') || (format[j] == '"') || (format[j] == ']') || (format[j] == '[')) {
			target[z] = format[j];//direct copy valid json characters from format to target
			z++;
		}
		else {//retrieve arguments according to the format specifier
			if (format[j] == 's') {
				s = va_arg(arg, char*);
				target[z] = '"';
				z++;
				int n;
				for (n = 0; n < strlen(s); n++) {
					if (s[n]=='"' || s[n]=='\\'){
					  target[z] = '\\';
					  z++;
					}
					target[z] = s[n];
					z++;
				}
				target[z] = '"';
				z++;
			}
			if (format[j] == 'n') {
				numb = va_arg(arg, long);
				//sprintf(m, "%d", numb);
				itoa(numb,m,10);
				int n;
				for (n = 0; n < strlen(m); n++) {
					target[z] = m[n];
					z++;
				}
			}
			if (format[j] == 'b') {
				s = va_arg(arg, char*);
				if ((s[0] == 't') || (s[0] == 'f')) {
					int n;
					for (n = 0; n < strlen(s); n++) {
						target[z] = s[n];
						z++;
					}
				}
				else {
					return 1;
				}
			}
		}
	}
	va_end(arg);//close the argument list
	target[z] = '\n';//************************************************************************************************THis is what I changed
	return 0;
}

int check_JSMN_error(int numtok, char* location) {//Prints error and error location to the location specified by output_string definition, returns 1 if error is detected
	if (numtok == JSMN_ERROR_INVAL) {//Error Checking
		char errorString[50] = "ERROR:INVALID JSON IN ";
		strcat(errorString, location);
		output_string(errorString);
		output_string("\n");
		return 1;
	}
	if (numtok == JSMN_ERROR_NOMEM) {
		char errorString[50] = "ERROR:MEMORY OVERFLOW IN ";
		strcat(errorString, location);
		output_string(errorString);
		output_string("\n");
		return 1;
	}
	if (numtok == JSMN_ERROR_PART) {
		char errorString[50] = "ERROR:MORE DATA EXPECTED IN ";
		strcat(errorString, location);
		output_string(errorString);
		output_string("\n");
		return 1;
	}
	return 0;
}

void GetNameValue(char* JSONStr, char* Name, char* Result){
	//Example:
	//char Input = "{\"Response\":\"StartUp\":{\"Version\":\"1.2.3\"}};
	//char ResponseStr[400];
	//GetNameValue(Input, "Response", ResponseStr);
	//
	jsmn_parser parser;
	jsmntok_t tokens[50];
	int numtok = 0;
	Result[0] = '\0';

	jsmn_init(&parser);//reset parser, parser must be reset for each new string

	numtok = jsmn_parse(&parser, JSONStr, strlen(JSONStr), tokens, 50);//tokenize JSON string

	if (check_JSMN_error(numtok, "MAIN")) {//Error Checking
		Result[0] = '\0';
		return;
		//continue;
	}

	extract_value(JSONStr,Name,Result,numtok,tokens);//the JSON value associated with the key 	}
}

void output_string(char* s) {//redefine this to fit whatever system you're working on
//this function needs to direct the string s to your desired output location for error information
	//putStr(&Tx,s,strlen(s));
}
