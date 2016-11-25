
#include "builtin.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>

#define MIN_ASCII_VALUE 32  // min value to be recognized as an ASCII assocring to specs


eError substr(const dtStr *s, int32_t beginIndex, int32_t endIndex, dtStrPtr *subStr) {
	if (s == NULL || s->str == NULL) {
		printError(ERR_INTERN, "In substr: Parameter or its member is NULL\n");
		return ERR_INTERN;
	}

	if (beginIndex < 0 || endIndex < 0) {
		printError(ERR_OTHER, "In substr: String index is out of range\n");
		return ERR_OTHER;
	}
	uint32_t uBeginIndex, uEndIndex;
	uBeginIndex = (uint32_t) beginIndex;
	uEndIndex = (uint32_t) endIndex;

	if (uEndIndex > s->uiLength) {
		printError(ERR_OTHER, "In substr: String index is out of range\n");
		return ERR_OTHER;
	}
	
	if (uBeginIndex > uEndIndex) {
		printError(ERR_OTHER, "In substr: String index is out of range\n");
		return ERR_OTHER;
	}

	*subStr = strNew();
	if (*subStr == NULL) {
		printError(ERR_INTERN, "In substr: Cannot create string. Out of memory.\n");
		return ERR_INTERN;
	}
	for (uint32_t i = uBeginIndex; i < uEndIndex; i++) {
		strAddChar(*subStr, s->str[i]);	
	}
	return ERR_OK;
}

eError readData(tSymbolPtr symbol, tSymbolData* data) {
	int32_t c;
	switch (symbol->Type) {
		case eINT:
		case eDOUBLE:
			
			c = getchar();			

			if ( isspace(c) || c == EOF || c == '\n') {
				if (c != EOF && c != '\n') {
					do {
						c = getchar();
					} while (c != EOF && c != '\n');
				}
				printError(ERR_RUN_INPUT, "Error while reading from stdin: unexpected data (expected integer)\n");
				return ERR_RUN_INPUT;
			}

			// Create temporary string, which will be converted to int/double
			dtStr *tmpStr = NULL;
			if ((tmpStr = strNew()) == NULL) {
				printError(ERR_INTERN, "In readData: Cannot create string. Out of memory.\n");
				strFree(tmpStr);
				return ERR_INTERN;
			}

			for (; !isspace(c) && c < 128 && c != EOF && c != '\n'; c = getchar()) {
				if (strAddChar(tmpStr, c) == STR_ERROR) {
					printError(ERR_INTERN, "In readData: Cannot add char to string.\n");
					strFree(tmpStr);
					return ERR_INTERN;
				}
			}

			char * ptr;
			if (symbol->Type == eINT) {
				data->Integer = (int32_t)strtol(tmpStr->str, &ptr, 10);
			} else {
				data->Double = strtod(tmpStr->str, &ptr);
			}

			if (*ptr != '\0' || (c != EOF && c != '\n')) {
				if (symbol->Type == eINT) {
					printError(ERR_RUN_INPUT, "Error while reading from stdin: unexpected data (expected integer)\n");
				} else {
					printError(ERR_RUN_INPUT, "Error while reading from stdin: unexpected data (expected double)\n");				
				}
				if (c != EOF && c != '\n') {
					do {
						c = getchar();
					} while (c != EOF && c != '\n');
				}
				strFree(tmpStr);
				return ERR_RUN_INPUT;
			}

			strFree(tmpStr);
			break;

		case eSTRING:
			if ((data->String = strNew()) == NULL) {
				printError(ERR_INTERN, "In readData: Cannot create string. Out of memory.\n");
				return ERR_INTERN;
			}
			while ((c = getchar()) != EOF && c != '\n') {
				if (c == '\\') {
					c = getchar();					 
					switch (c) {
						case 'n': {
							if (strAddChar(data->String, '\n') == STR_ERROR) {
								printError(ERR_INTERN, "In readData: Cannot add char to string.\n");
								return ERR_INTERN;
							}
							break;
						}
						case 't': {
							if (strAddChar(data->String, '\t') == STR_ERROR) {
								printError(ERR_INTERN, "In readData: Cannot add char to string.\n");
								return ERR_INTERN;
							}
							break;
						}
						case '\"': {
							if (strAddChar(data->String, '\"') == STR_ERROR) {
								printError(ERR_INTERN, "In readData: Cannot add char to string.\n");
								return ERR_INTERN;
							}
							break;
						}
						case '\\': {
							if (strAddChar(data->String, '\\') == STR_ERROR) {
								printError(ERR_INTERN, "In readData: Cannot add char to string.\n");
								return ERR_INTERN;
							}
							break;
						}
						default: {
							// Escape sequences
							if (c >= '0' && c <= '3') {
								char tmp;
								tmp = (c - '0') * 64;
								//reading 2nd sign
								c = getchar();
								if (c >= '0' && c <= '7') {
									tmp = tmp + (c - '0') * 8;
									//reading 3nd sign
									c = getchar();
									if (c >= '0' && c <= '7') {
										tmp = tmp + (c - '0');
										if (strAddChar(data->String, tmp) == STR_ERROR) {
											printError(ERR_INTERN, "In readData: Cannot add char to string.\n");
											return ERR_INTERN;
										}
									}
									else {
										printError(ERR_RUN_INPUT, "In readString: Unexpected escape sequences\n");
										return ERR_RUN_INPUT;
									}
								}
								else {
									printError(ERR_RUN_INPUT, "In readString: Unexpected escape sequences\n");
									return ERR_RUN_INPUT;
								}
							}							
							else {
								printError(ERR_RUN_INPUT, "In readString: Unexpected escape sequences\n");
								return ERR_RUN_INPUT;
							}
						}
					}
				}
				else if (c < MIN_ASCII_VALUE) {
					printError(ERR_RUN_INPUT, "In readString: Value of char is lower 32\n");
					return ERR_RUN_INPUT;
				}
				else if (strAddChar(data->String, c) == STR_ERROR) {
					printError(ERR_INTERN, "In readData: Cannot add char to string.\n");
					return ERR_INTERN;
				}
			}
			break;

		default:
			printError(ERR_RUN_INPUT, "Error while reading from stdin: unexpected data\n");
			return ERR_RUN_INPUT;
	}
	return ERR_OK;
}
