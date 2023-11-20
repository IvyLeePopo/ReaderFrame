#pragma once

///* d3des.h -
// *
// *	Headers and defines for d3des.c
// *	Graven Imagery, 1992.
// *
// * Copyright (c) 1988,1989,1990,1991,1992 by Richard Outerbridge
// *	(GEnie : OUTER; CIS : [71755,204])
// */


#include <cstring>
//#include <mem.h>

#ifndef d3desH
#define d3desH

#define D2_DES		/* include double-length support */ 
#define D3_DES		/* include triple-length support */ 
 
#ifdef D3_DES 
#ifndef D2_DES 
#define D2_DES		/* D2_DES is needed for D3_DES */ 
#endif
#endif 
 
#define EN0	0	/* MODE == encrypt */ 
#define DE1	1	/* MODE == decrypt */ 

//#ifdef  __cplusplus
//extern "C" {
//#endif
 
/* A useful alias on 68000-ish machines, but NOT USED HERE. */ 
 
//typedef union { 
//	unsigned long blok[2]; 
//	unsigned short word[4]; 
//	unsigned char byte[8]; 
//	} M68K; 

class D3Des {
private:
	unsigned long KnL[32]; 
	unsigned long KnR[32]; 
	unsigned long Kn3[32]; 

public:
	D3Des() { 
		memset(KnL, 0, sizeof(KnL));
		memset(KnR, 0, sizeof(KnR));
		memset(Kn3, 0, sizeof(Kn3));
	};
	~D3Des() { };

	void deskey(const unsigned char *, short); 
	/*		      hexkey[8]     MODE 
	 * Sets the internal key register according to the hexadecimal 
	 * key contained in the 8 bytes of hexkey, according to the DES, 
	 * for encryption or decryption according to MODE. 
	 */ 
 
	void usekey(const unsigned long *); 
	/*		    cookedkey[32] 
	 * Loads the internal key register with the data in cookedkey. 
	 */ 
 
	void cpkey(unsigned long *); 
	/*		   cookedkey[32] 
	 * Copies the contents of the internal key register into the storage 
	 * located at &cookedkey[0]. 
	 */ 
 
	void des(const unsigned char *, unsigned char *); 
	/*		    from[8]	      to[8] 
	 * Encrypts/Decrypts (according to the key currently loaded in the 
	 * internal key register) one block of eight bytes at address 'from' 
	 * into the block at address 'to'.  They can be the same. 
	 */ 
 
	#ifdef D2_DES 
 
	#define desDkey(a,b)	des2key((a),(b)) 
	void des2key(const unsigned char *, short); 
	/*		      hexkey[16]     MODE 
	 * Sets the internal key registerS according to the hexadecimal 
	 * keyS contained in the 16 bytes of hexkey, according to the DES, 
	 * for DOUBLE encryption or decryption according to MODE. 
	 * NOTE: this clobbers all three key registers! 
	 */ 
 
	void Ddes(const unsigned char *, unsigned char *); 
	/*		    from[8]	      to[8] 
	 * Encrypts/Decrypts (according to the keyS currently loaded in the 
	 * internal key registerS) one block of eight bytes at address 'from' 
	 * into the block at address 'to'.  They can be the same. 
	 */ 
 
	void D2des(const unsigned char *, unsigned char *); 
	/*		    from[16]	      to[16] 
	 * Encrypts/Decrypts (according to the keyS currently loaded in the 
	 * internal key registerS) one block of SIXTEEN bytes at address 'from' 
	 * into the block at address 'to'.  They can be the same. 
	 */ 
 
	void makekey(char *, unsigned char *); 
	/*		*password,	single-length key[8] 
	 * With a double-length default key, this routine hashes a nullptr-terminated 
	 * string into an eight-byte random-looking key, suitable for use with the 
	 * deskey() routine. 
	 */ 
 
	#define makeDkey(a,b)	make2key((a),(b)) 
	void make2key(char *, unsigned char *); 
	/*		*password,	double-length key[16] 
	 * With a double-length default key, this routine hashes a nullptr-terminated 
	 * string into a sixteen-byte random-looking key, suitable for use with the 
	 * des2key() routine. 
	 */ 
 
	#ifndef D3_DES	/* D2_DES only */ 
 
	#define useDkey(a)	use2key((a)) 
	#define cpDkey(a)	cp2key((a)) 
 
	void use2key(unsigned long *); 
	/*		    cookedkey[64] 
	 * Loads the internal key registerS with the data in cookedkey. 
	 * NOTE: this clobbers all three key registers! 
	 */ 
 
	void cp2key(unsigned long *); 
	/*		   cookedkey[64] 
	 * Copies the contents of the internal key registerS into the storage 
	 * located at &cookedkey[0]. 
	 */ 
 
	#else	/* D3_DES too */ 
 
	#define useDkey(a)	use3key((a)) 
	#define cpDkey(a)	cp3key((a)) 
 
	void des3key(const unsigned char *, short); 
	/*		      hexkey[24]     MODE 
	 * Sets the internal key registerS according to the hexadecimal 
	 * keyS contained in the 24 bytes of hexkey, according to the DES, 
	 * for DOUBLE encryption or decryption according to MODE. 
	 */ 
 
	void use3key(const unsigned long *); 
	/*		    cookedkey[96] 
	 * Loads the 3 internal key registerS with the data in cookedkey. 
	 */ 
 
	void cp3key(unsigned long *); 
	/*		   cookedkey[96] 
	 * Copies the contents of the 3 internal key registerS into the storage 
	 * located at &cookedkey[0]. 
	 */ 
 
	void make3key(char *, unsigned char *); 
	/*		*password,	triple-length key[24] 
	 * With a triple-length default key, this routine hashes a nullptr-terminated 
	 * string into a twenty-four-byte random-looking key, suitable for use with 
	 * the des3key() routine. 
	 */ 
#endif	/* D3_DES */
#endif	/* D2_DES */
private:
	void cookey(register const unsigned long *raw1);
	void D3des(unsigned char *, unsigned char *);
};
//#ifdef  __cplusplus
//}
//#endif


/* d3des.h V5.09 rwo 9208.04 15:06 Graven Imagery
 ********************************************************************/
#endif  /*d3desH*/



