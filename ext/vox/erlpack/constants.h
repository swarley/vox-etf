/************************************************
 *   MIT License
 *
 *    Copyright (c) 2017 Discord
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 ************************************************/

#define FORMAT_VERSION 131
#define NEW_FLOAT_EXT 'F'       // 70  [Float64:IEEE float]
#define BIT_BINARY_EXT 'M'      // 77  [UInt32:Len, UInt8:Bits, Len:Data]
#define COMPRESSED 'P'          // 80  [UInt4:UncompressedSize, N:ZlibCompressedData]
#define SMALL_INTEGER_EXT 'a'   // 97  [UInt8:Int]
#define INTEGER_EXT 'b'         // 98  [Int32:Int]
#define FLOAT_EXT 'c'           // 99  [31:Float String] Float in string format (formatted "%.20e", sscanf "%lf"). Superseded by NEW_FLOAT_EXT
#define ATOM_EXT 'd'            // 100 [UInt16:Len, Len:AtomName] max Len is 255
#define REFERENCE_EXT 'e'       // 101 [atom:Node, UInt32:ID, UInt8:Creation]
#define PORT_EXT 'f'            // 102 [atom:Node, UInt32:ID, UInt8:Creation]
#define PID_EXT 'g'             // 103 [atom:Node, UInt32:ID, UInt32:Serial, UInt8:Creation]
#define SMALL_TUPLE_EXT 'h'     // 104 [UInt8:Arity, N:Elements]
#define LARGE_TUPLE_EXT 'i'     // 105 [UInt32:Arity, N:Elements]
#define NIL_EXT 'j'             // 106 empty list
#define STRING_EXT 'k'          // 107 [UInt16:Len, Len:Characters]
#define LIST_EXT 'l'            // 108 [UInt32:Len, Elements, Tail]
#define BINARY_EXT 'm'          // 109 [UInt32:Len, Len:Data]
#define SMALL_BIG_EXT 'n'       // 110 [UInt8:n, UInt8:Sign, n:nums]
#define LARGE_BIG_EXT 'o'       // 111 [UInt32:n, UInt8:Sign, n:nums]
#define NEW_FUN_EXT 'p'         // 112 [UInt32:Size, UInt8:Arity, 16*Uint6-MD5:Uniq, UInt32:Index, UInt32:NumFree, atom:Module, int:OldIndex, int:OldUniq, pid:Pid, NunFree*ext:FreeVars]
#define EXPORT_EXT 'q'          // 113 [atom:Module, atom:Function, smallint:Arity]
#define NEW_REFERENCE_EXT 'r'   // 114 [UInt16:Len, atom:Node, UInt8:Creation, Len*UInt32:ID]
#define SMALL_ATOM_EXT 's'      // 115 [UInt8:Len, Len:AtomName]
#define MAP_EXT 't'             // 116 [UInt32:Airty, N:Pairs]
#define FUN_EXT 'u'             // 117 [UInt4:NumFree, pid:Pid, atom:Module, int:Index, int:Uniq, NumFree*ext:FreeVars]
#define ATOM_UTF8_EXT 'v'       // 118 [UInt16:Len, Len:AtomName] max Len is 255 characters (up to 4 bytes per)
#define SMALL_ATOM_UTF8_EXT 'w' // 119 [UInt8:Len, Len:AtomName]
