#ifndef   _DES_ENCRYPT_DECRYPT 
#define   _DES_ENCRYPT_DECRYPT 
 
#define BYTE   unsigned char 
#define LPBYTE   BYTE* 
#define LPCBYTE   const BYTE* 
#define BOOL   int 
 
class DES 
{ 
public: 
  BOOL CDesEnter(LPCBYTE in, LPBYTE out, int datalen, const BYTE key[8], BOOL type); 
  BOOL CDesMac(LPCBYTE mac_data, LPBYTE mac_code, int datalen, const BYTE key[8]); 
private: 
  void XOR(const BYTE in1[8], const BYTE in2[8], BYTE out[8]); 
  LPBYTE Bin2ASCII(const BYTE byte[64], BYTE bit[8]); 
  LPBYTE ASCII2Bin(const BYTE bit[8], BYTE byte[64]); 
  void GenSubKey(const BYTE oldkey[8], BYTE newkey[16][8]); 
  void endes(const BYTE m_bit[8], const BYTE k_bit[8], BYTE e_bit[8]); 
  void undes(const BYTE m_bit[8], const BYTE k_bit[8], BYTE e_bit[8]); 
  void SReplace(BYTE s_bit[8]); 
}; 
#endif