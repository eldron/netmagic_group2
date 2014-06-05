/*...将十六进制转化为十进制处理*/

#include <stdio.h>
#include "nmac.h"

int hextodec(char x) {
	int n;
	switch (x) {
	case '0':
		n = 0;
		break;
	case '1':
		n = 1;
		break;
	case '2':
		n = 2;
		break;
	case '3':
		n = 3;
		break;
	case '4':
		n = 4;
		break;
	case '5':
		n = 5;
		break;
	case '6':
		n = 6;
		break;
	case '7':
		n = 7;
		break;
	case '8':
		n = 8;
		break;
	case '9':
		n = 9;
		break;
	case 'a':
		n = 10;
		break;
	case 'b':
		n = 11;
		break;
	case 'c':
		n = 12;
		break;
	case 'd':
		n = 13;
		break;
	case 'e':
		n = 14;
		break;
	case 'f':
		n = 15;
		break;
	}
	return n;
}

