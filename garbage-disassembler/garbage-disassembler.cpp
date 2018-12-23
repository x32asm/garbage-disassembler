#include <iostream>

int main(int argc, char ** argv) {
	_asm {
		mov eax, 10
		mov bl, al
	}

	return 0;
}

/*


.DATA
		undec DD ?    // uninitialized 32-bit value
		bt DB 255     // max 8-bit value
		sh DW 32767   // max 16-bit value
		it DD 1000000 // 1 million

		arr_int DD 5 DUP(?)		 // 5 32-bit values all uninitialized
		arr_word DW 100 DUP(255) // 100 16-bit words starting at arr_word all initialized to 255
		str DB 'hello',0		 // 6 bytes starting at str containing 'h','e','l','l','\0' respectively
		arr_byte DB 1, 2, 3, 4, 5, 255


*/