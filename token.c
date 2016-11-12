#include <stdlib.h>
#include "token.h"

Token* newToken() {
	Token *token = calloc(1, sizeof(Token));
	if (token != NULL) {
		token->type = TT_empty;
	}

	return token;
}

void freeToken(Token **pToken) {
	if (pToken != NULL) {
		if (*pToken != NULL) {
			if (((*pToken)->type == TT_fullIdentifier)
	        	|| ((*pToken)->type == TT_identifier)
			|| ((*pToken)->type == TT_string)) {
				strFree((*pToken)->str);
			}
			free(*pToken);
			*pToken = NULL;
		}
	}
}


void cleanToken(Token **pToken) {
	if (pToken != NULL) {
		if (*pToken != NULL) {
			if (((*pToken)->type == TT_fullIdentifier)
	        	|| ((*pToken)->type == TT_identifier)
			|| ((*pToken)->type == TT_string)) {
				strFree((*pToken)->str);
			}
			(*pToken)->type = TT_empty;
			(*pToken)->str  = NULL;
		}
	}
}
