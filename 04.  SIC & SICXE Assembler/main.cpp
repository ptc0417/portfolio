#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <string.h>
#include <algorithm>

#define TABLE_SIZE 80
using namespace std;

bool IsLetter( char ch ){
  if ( ( ch-'0' >= 17 && ch-'0' <= 42 ) || ( ch-'0' >= 49 && ch-'0' <= 74 ) ){
    return true;
  } // if
  else {
    return false;
  } // else
} // is letter()

bool IsDigit( char ch ){
  if ( ch - '0' >= 0 && ch - '0' <= 9 ){
    return true;
  } // if
  else {
    return false;
  } // else
} // is digit()

bool IsWhiteSpace( char ch ){
  if ( ch == ' ' || ch == '\n' || ch == '\t' ){
    return true;
  } // if
  else{
    return false;
  } // else
} // is white space()

bool apos = false ; // 判斷有無第一個撇了
bool comment = false ; // 判斷是否為註解，需跳至下一line

vector <string> inputData ;
vector <string> tokenpos ; //(token type,token value)
// table1-7
vector <string> T1_instr ;
vector <string> T2_psedo ;
vector <string> T3_reg ;
vector <string> T4_delim ;
vector <string> T5_sym ;
vector <string> T6_int ;
vector <string> T7_str ;

bool twoApos( int line ) {
    int num = 0 ;
    for( int i = 0 ; i != inputData.at(line).size() ; i++ ) {
        if( inputData.at(line).at(i) == 39 ) {
            num++ ;
        } // if
    } // for
    if ( num == 2 ) {
        return true ;
    } // if
    return false ;
} // two apos

bool allNumber( string token ) {
    for( int i = 0 ; i != token.size() ; i++ ) {
        if( !IsDigit(token.at(i)) ) {
            return false ;
        } // if
    } // for
    return true ;
} // allDigit()

typedef struct aline {
    int tablenum ;
    int position ;
    int line ;
    string tokenname ;
};
vector <struct aline> text ;

class Token {

  public :
    bool Read ( string fileName, vector <string> & temp ) { // fileName須完整檔名(包含檔案格式.txt等)
        string aLine ;
        fstream file ;
        file.open( fileName.c_str() ,ios::in ) ;
        if ( !file.is_open() ) { // check if file is open
            cout << endl << "### " << fileName << " does not exist! " << "###" << endl << endl ;
            return false ;
        } // if
        else {
            while( !file.eof() ) {
                getline( file, aLine, '\n' ) ;
                if ( aLine != "" ) {
                    aLine.erase( aLine.find_last_not_of( " " )+1 ) ;
                    temp.push_back(aLine) ;
                } // if
            } // while
            file.close() ;
            return true ;
        } //else
    } // Token::Read()

    void Write ( string inputfile ) {
        FILE * fout ;
        inputfile.resize(inputfile.size() - 4 ) ;
        inputfile = inputfile + "_output.txt" ;
        fout = fopen( inputfile.c_str(), "w" ) ;
        if ( fout == NULL ){
            cout << "Fail to open file" << inputfile << endl ;
            return ;
        } // if
        for( int i = 0 ; i != inputData.size() ; i++ ) {
            fprintf( fout, "%s\n", inputData.at(i).c_str() ) ;
            fprintf( fout, "%s\n", tokenpos.at(i).c_str() ) ;
        } // for
        fclose( fout ) ;
    } // Token::Write()

    void preTable() {
        Read("Table1.table", T1_instr) ;
        Read("Table2.table", T2_psedo) ;
        Read("Table3.table", T3_reg) ;
        Read("Table4.table", T4_delim) ;
        T5_sym.resize( TABLE_SIZE ) ;
        T6_int.resize( TABLE_SIZE ) ;
        T7_str.resize( TABLE_SIZE ) ;
        tokenpos.resize( inputData.size() ) ;
    } // Token::saveTable()

    bool cut( char walk, char fwalk, char & XC, string & temp, string & ano, int line ) {

        if ( IsWhiteSpace(walk) && temp == "" ) { // 遇到whitespace的情況一
            if (apos && twoApos(line)) { // string的一部分
                temp = temp + walk ;
            } // if
            return false ;
        } // if
        else if ( IsWhiteSpace(walk) && temp != "" ) { // 遇到whitespace的情況二，有token了
            if (apos && twoApos(line)) { // string的一部分
                temp = temp + walk ;
                return false ;
            } // if
            return true ;
        } // else if
        else if ( IsLetter(walk) ) {
            temp = temp + walk ;
            return false ;
        } // else if
        else if ( IsDigit(walk) ) {
            temp = temp + walk ;
            return false ;
        } // else if
        else if ( walk == 39 ) { // ' 撇
            if ( apos ) { // string 第二撇
                ano = walk ;
                return true ; // 先存前面字串,再用fwalk存第二撇
            } // if
            else { // string 第一撇
                if ( (temp == "X" || temp == "C" || temp == "x" || temp == "c" ) && twoApos(line) ) {
                    XC = temp.at(0) ;
                    temp = "" ;
                    temp = walk ;
                } // if
                else {
                    XC = ' ' ;
                    ano = walk ;
                } // else
                apos = true ;
                return true ;
            } // else

        } // else if
        else if ( ( walk <= 45 && walk >= 42 ) || walk == 47 || walk == 64 || walk == 35
                 || walk == 58 || walk == 59 || walk == 61 || walk == 63 || walk == 95 ) {
            // 減號、逗號、加號、乘號、除、@、#、:、;、=、?、_
            if (apos && twoApos(line)) { // string的一部分
                temp = temp + walk ;
                return false ;
            } // if
            if ( temp != "" ) { // 前面token和符號相連
                ano = walk ;
            } // if
            else {
                temp = walk ;
            } // else
            return true ;
        } // else if
        else if ( walk == 46 ) { // 句號。(於SIC內為註解的意思)
            if (apos && twoApos(line)) { // string的一部分
                temp = temp + walk ;
                return false ;
            } // if
            if ( temp != "" ) { // 前面token和符號相連
                ano = walk ;
            } // if
            else {
                temp = walk ;
            } // else
            comment = true ;
            return true ;
        } // else if

    } // Token::cut()

    void getToken() {
        string type ; // table number
        string value ; // table position
        string token ;
        string another ;
        struct aline t ;
        bool found = false ; // 是否於table中找到token
        for( int line = 0 ; line != inputData.size() ; line++ ){
            char frontwalk = ' ' ;
            char XC = ' ' ;
            int pos = 0 ;
            int tableN = 0 ;
            vector <struct aline> tokenline ;
            while ( pos <= inputData.at(line).size() ) {
                if ( ( pos == inputData.at(line).size() && token != "" )
                    || ( pos != inputData.at(line).size()
                        && cut( inputData.at(line).at(pos), frontwalk, XC, token, another, line ) ) ){
                    // 最後一個token後無換行 或 cut()得true得一token
                    if ( token != "" ) {
                        found = findTable( token, type, value, line, XC ) ;
                        if ( !found ) {
                            type = classified( token, frontwalk, line, XC ) ;
                            if ( type == "5" ) {
                                hashing( token, T5_sym, value ) ;
                            } // if
                            else if( type == "6" ){
                                string strhi ;
                                transform(token.begin(), token.end(), back_inserter(strhi), ::toupper); // 字母轉大寫
                                hashing( strhi, T6_int, value ) ;
                            } // else if
                            else if( type == "7" ){
                                hashing( token, T7_str, value ) ;
                            } // else if
                            found = false ;
                        } // if
                        tokenpos.at( line ) =  tokenpos.at( line ) + "(" + type + "," + value + ")" ;
                        int ty = stoi(type) ;
                        int v = stoi(value) ;
                        t.tablenum = ty ;
                        t.position = v ;
                        t.tokenname = token ;
                        t.line = line ;
                        tokenline.push_back(t) ;
                    } // if
                    token = "" ;
                    type = "" ;
                    value = "" ;
                    if ( another != "" ) {
                        found = findTable( another, type, value, line, XC ) ;
                        if ( !found ) {
                            type = classified( another, frontwalk, line, XC ) ;
                            if ( type == "5" ) {
                                hashing( token, T5_sym, value ) ;
                            } // if
                            else if( type == "6" ){
                                string strhi ;
                                transform(token.begin(), token.end(), back_inserter(strhi), ::toupper); // 字母轉大寫
                                hashing( strhi, T6_int, value ) ;
                            } // else if
                            else if( type == "7" ){
                                hashing( token, T7_str, value ) ;
                            } // else if
                            found = false ;
                        } // if
                        tokenpos.at( line ) =  tokenpos.at( line ) + "(" + type + "," + value + ")" ;
                        int ty = stoi(type) ;
                        int v = stoi(value) ;
                        t.tablenum = ty ;
                        t.position = v ;
                        t.line = line ;
                        t.tokenname = another ;
                        tokenline.push_back(t) ;
                        another = "" ;
                        type = "" ;
                        value = "" ;
                        apos = false ;
                    } // if

                    if (comment) {
                        pos = inputData.at(line).size() ;
                        comment = false ;
                    } // if
                } // if

                if ( pos != inputData.at(line).size() ) {
                    frontwalk = inputData.at(line).at(pos) ;
                } // if
                pos++ ;
            } //while
            text.push_back(tokenline) ;
        } // for
    } // Token::getToken()

    bool findTable( string token, string & type, string & value, int line, char XC ) {
        string strhi ;
        string strlo ;
        transform(token.begin(), token.end(), back_inserter(strhi), ::toupper); // 字母轉大寫
        transform(token.begin(), token.end(), back_inserter(strlo), ::tolower); // 字母轉小寫
        for ( int i = 0 ; i != T1_instr.size() ; i++ ) {
            if ( T1_instr.at(i) == strlo && ( XC != 'C' && XC != 'X' && XC != 'c' && XC != 'x' ) ) {
                type = "1" ;
                value = to_string(i+1) ;
                return true ;
            } // if
        } // for
        for ( int i = 0 ; i != T2_psedo.size() ; i++ ) {
            if ( T2_psedo.at(i) == strhi && ( XC != 'C' && XC != 'X' && XC != 'c' && XC != 'x' ) ) {
                type = "2" ;
                value = to_string(i+1) ;
                return true ;
            } // if
        } // for
        for ( int i = 0 ; i != T3_reg.size() ; i++ ) {
            if ( T3_reg.at(i) == strhi && (!apos && XC != 'X' && XC!='C' ) ) { // 若是第一撇前面的X，line有兩個撇、就不翻
                type = "3" ;
                value = to_string(i+1) ;
                return true ;
            } // if
        } // for
        for ( int i = 0 ; i != T4_delim.size() ; i++ ) {
            if ( T4_delim.at(i) == token ) {
                type = "4" ;
                value = to_string(i+1) ;
                return true ;
            } // if
        } // for
        for ( int i = 0 ; i != T5_sym.size() ; i++ ) {
            if ( T5_sym.at(i) == token && !apos ) {
                type = "5" ;
                value = to_string(i) ;
                return true ;
            } // if
        } // for
        for ( int i = 0 ; i != T6_int.size() ; i++ ) {
            if ( T6_int.at(i) == token && XC != 'C' && XC != 'c' && (XC == 'X' || XC != 'x') ) {
                type = "6" ;
                value = to_string(i) ;
                return true ;
            } // if
        } // for
        for ( int i = 0 ; i != T7_str.size() ; i++ ) {
            if ( T7_str.at(i) == token && XC != 'X' && XC != 'x' && (XC == 'C' || XC != 'c') ) {
                type = "7" ;
                value = to_string(i) ;
                return true ;
            } // if
        } // for
        return false ;
    } // Token::findTable()

    string classified( string token, char fchar, int line, char XC ) { // 分類token為5-7table的哪一種
        if( apos && twoApos(line) && XC != 'X' && XC != 'x' &&( XC == 'C'|| XC == 'c' ) ) {
            apos = false ;
            return "7" ;
        } // if
        else if( allNumber(token) || ( apos && twoApos(line) && ( XC == 'X' || XC == 'x' ) ) ) {
            apos = false ;
            return "6" ;
        } // else if
        else {
            return "5" ;
        } // else
    } // Token::classified()

    void hashing( string token, vector <string> & temp, string & value ) {
        int sum = 0 ;
        bool done = false ;
        for( int i = 0 ; i != token.size() ; i++ ) {
            sum = sum + token.at(i) ;
        } // for
        sum = sum%100 ;
        while( !done ) {
            if ( sum < TABLE_SIZE && temp.at(sum) != "" ) { // 已有存物
                sum = sum + 1 ;
            } // if
            else if ( sum >= TABLE_SIZE) {
                sum = sum - TABLE_SIZE ;
            } // else if
            else { // 位置為空
                temp.at(sum) = token ;
                value = to_string(sum) ;
                done = true ;
            } // else
        } // while
    } // Token::hashing()

};
static Token token = Token() ;

class SIC {
    vector <string> instr = { "ADD", "AND", "COMP", "DIV", "J", "JEQ", "JGT", "JLT", "JSUB", "LDA", "LDCH", "LDL", "LDX", "MUL",
                               "OR", "RD", "RSUB", "STA", "STCH", "STL", "STSW", "STX", "SUB", "TD", "TIX", "WD" };
    vector <string> opcode = { "18", "40", "28", "24", "3C", "30", "34", "38", "48", "00", "50", "08", "04", "20",
                               "44", "D8", "4C", "0C", "54", "14", "E8", "10", "1C", "E0", "2C", "DC" };

    bool findopc( int num, string& opc ) {
        string strhi = "" ;
        transform(T1_instr.at(num-1).begin(), T1_instr.at(num-1).end(), back_inserter(strhi), ::toupper); // 字母轉大寫
        for (int i = 0 ; i != instr.size() ; i++ ){
            if ( instr.at(i) == strhi ) {
                opc = opcode.at(i) ;
                return true ;
            } // if
        } // for
        return false ;
    } // find opcode

  public :
/*
    bool Format( string line, string & opc ) { // 只有三種可能<INS>+ <symbol> | <symbol>, X | 無(0)
        string temp = "" ;
        for ( int pos = 0 ; pos != line.size() ; pos++ ) {
            if ( line.at(pos) == '(' ) {
                char tnum = line.at(pos+1) ;
                if ( tnum == '1' ) {
                    //cout << stoi(line.substr( pos+3, line.find(")", pos)-pos-3)) << endl ;
                    if ( !findopc( stoi(line.substr( pos+3, line.find(")", pos)-pos-3)), opc ) ) { // instr in table's number
                        return false ;
                    } // if
                } // if
                if ( tnum == '4' && line.find("(", pos+1) != -1 ) { // 是否為逗號(非註解)
                    if ( stoi(line.substr( pos+3, line.find(")", pos)-pos-3)) != 1 ) { // 非逗號
                        return false ;
                    } // if
                } // if
                if ( tnum == '4' && line.find("(", pos+1) == -1 ) { // 是否為註解
                    if ( stoi(line.substr( pos+3, line.find(")", pos)-pos-3)) != 9 ) { // 非註解
                        return false ;
                    } // if
                } // if

                temp = temp + tnum ;

                if ( (temp == "51543" || temp == "1543") && (line.at(pos+3) != '2' ) ) { // 有reg但非X
                    opc = "" ;
                    return false ;
                } // if
            } // if
        } // for

        if (temp != "51" && temp != "515" && temp != "51543" && temp != "1" && temp != "15" && temp != "1543"
            && temp != "514" && temp != "5154" && temp != "515434" && temp != "14" && temp != "154" && temp != "15434" ) {
            opc = "" ;
            return false ;
        } // if

        return true ;
    } // Format()
*/
};
static SIC sic = SIC() ;

class SICXE {
    vector <string> f1ins = { "FIX", "FLOAT", "HIO", "NORM", "SIO", "TIO" };
    vector <string> f1opc = { "C4", "C0", "F4", "C8", "F0", "F8" };
    vector <string> f2ins = { "ADDR", "CLEAR", "COMPR", "DIVR", "MULR", "RMO", "SHIFTL", "SHIFTR", "SUBR", "SVC", "TIXR" };
    vector <string> f2opc = { "90", "B4", "A0", "9C", "98", "AC", "A4", "A8", "94", "B0", "B8" };
    vector <string> f34ins = { "ADD", "ADDF", "AND", "COMP", "COMPF", "DIV", "DIVF", "J", "JEQ", "JGT", "JLT", "JSUB", "LDA",
                               "LDB", "LDCH", "LDF", "LDL", "LDS", "LDT", "LDX", "LPS", "MUL", "MULF", "OR", "RD", "RSUB",
                               "SSK", "STA", "STB", "STCH", "STF", "STI", "STL", "STS", "STSW", "STT", "STX", "SUB", "SUBF",
                               "TD", "TIX", "WD" };
    vector <string> f34opc = { "18", "58", "40", "28", "88", "24", "64", "3C", "30", "34", "38", "48", "00", "68", "50",
                               "70", "08", "6C", "74", "04", "D0", "20", "60", "44", "D8", "4C", "EC", "0C", "78", "54",
                               "80", "D4", "14", "7C", "E8", "84", "10", "1C", "5C", "E0", "2C", "DC" };

    bool findopc( int num, string& opc, vector <string> v, vector<string> vop, bool & RSUB ) {
        string strhi = "" ;
        transform(T1_instr.at(num-1).begin(), T1_instr.at(num-1).end(), back_inserter(strhi), ::toupper); // 字母轉大寫
        for (int i = 0 ; i != v.size() ; i++ ){
            if ( v.at(i) == strhi ) {
                if ( strhi == "RSUB" ) {
                    RSUB = true ;
                } // if
                opc = vop.at(i) ;
                return true ;
            } // if
        } // for
        return false ;
    } // find opcode

  public :
      bool format(int line, string &opc) {
          int i = 0 ;
          bool isRSUB = false ;
          if ( text.at(line).at(i).tokenname == "+" ) { // +
              i++ ;
              if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 1 && findopc(text.at(line).at(i).position, opc, f34ins, f34opc, isRSUB ) ) {
                  i++ ;
                  if (isRSUB) {
                      if ( i == text.at(line).size() || text.at(line).at(i).tokenname == "." ) {
                          return true ;
                      } // if
                  } // if
                  if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "#" ) {
                      i++ ;
                      if ( i < text.at(line).size() &&( text.at(line).at(i).tablenum == 5 || text.at(line).at(i).tablenum == 6 )
                          && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                          return true ;
                      } // if
                  } // if
                  else if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "@" ) {
                      i++ ;
                      if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 5
                          && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                          return true ;
                      } // if
                  } // else if
                  else if( i < text.at(line).size() && text.at(line).at(i).tokenname == "=" ) {
                      i++ ;
                      if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "'" ) {
                          i++ ;
                          if ( i < text.at(line).size() && (text.at(line).at(i).tablenum == 6 || text.at(line).at(i).tablenum == 7) ) {
                              i++ ;
                              if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "'"
                                  && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                                  return true ;
                              } // if
                          } // if
                      } // if
                  } // else if
                  else if( i < text.at(line).size() && text.at(line).at(i).tablenum == 5 ) {
                      i++ ;
                      if ( i == text.at(line).size() || text.at(line).at(i).tokenname == "." ) {
                          return true ;
                      } // if
                      else if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "," ) {
                          i++ ;
                          if ( i < text.at(line).size() && (text.at(line).at(i).tokenname == "x" || text.at(line).at(i).tokenname == "X")
                              && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                              return true ;
                          } // if
                      } // else if
                  } // else if
                  else if( i < text.at(line).size() && text.at(line).at(i).tablenum == 6 ) {
                      if ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) {
                          return true ;
                      } // if
                  } // else if
              } // if
          } // if
          else if ( text.at(line).at(i).tablenum == 1 ) { // instruction
              if ( findopc( text.at(line).at(i).position, opc, f1ins, f1opc, isRSUB ) ) {
                  i++ ;
                  if ( i == text.at(line).size() || text.at(line).at(i).tokenname == ".") {
                      return true ;
                  } // if
              } // if
              else if ( findopc( text.at(line).at(i).position, opc, f2ins, f2opc, isRSUB ) ) {
                  i++ ;
                  if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 3 ) {
                      i++ ;
                      if ( i == text.at(line).size() || text.at(line).at(i).tokenname == "." ) {
                          return true ;
                      } // if
                      else if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "," ) {
                          i++ ;
                          if ( i < text.at(line).size()
                              && (text.at(line).at(i).tablenum == 3 || text.at(line).at(i).tablenum == 6)
                              && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." )) {
                              return true ;
                          } // if
                      } // else if
                  } // if
              } // else if
              else if ( findopc( text.at(line).at(i).position, opc, f34ins, f34opc, isRSUB ) ) { //type3
                  i++ ;
                  if (isRSUB) {
                      if ( i == text.at(line).size() || text.at(line).at(i).tokenname == "." ) {
                          return true ;
                      } // if
                  } // if
                  if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "#" ) {
                      i++ ;
                      if ( i < text.at(line).size() &&( text.at(line).at(i).tablenum == 5 || text.at(line).at(i).tablenum == 6 )
                          && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                          return true ;
                      } // if
                  } // if
                  else if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "@" ) {
                      i++ ;
                      if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 5
                          && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                          return true ;
                      } // if
                  } // else if
                  else if( i < text.at(line).size() && text.at(line).at(i).tokenname == "=" ) {
                      i++ ;
                      if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "'" ) {
                          i++ ;
                          if ( i < text.at(line).size() && (text.at(line).at(i).tablenum == 6 || text.at(line).at(i).tablenum == 7) ) {
                              i++ ;
                              if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "'"
                                  && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                                  return true ;
                              } // if
                          } // if
                      } // if
                  } // else if
                  else if( i < text.at(line).size() && text.at(line).at(i).tablenum == 5 ) {
                      i++ ;
                      if ( i == text.at(line).size() || text.at(line).at(i).tokenname == "." ) {
                          return true ;
                      } // if
                      else if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "," ) {
                          i++ ;
                          if ( i < text.at(line).size() && (text.at(line).at(i).tokenname == "x" || text.at(line).at(i).tokenname == "X")
                              && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                              return true ;
                          } // if
                      } // else if
                  } // else if
                  else if( i < text.at(line).size() && text.at(line).at(i).tablenum == 6 ) {
                      if ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) {
                          return true ;
                      } // if
                  } // else if
              } // else if
          } // else if
          else if ( text.at(line).at(i).tablenum == 5 ) { // label
              i++ ;
              if ( text.at(line).at(i).tablenum == 1 ) { // instruction
                  if ( findopc( text.at(line).at(i).position, opc, f1ins, f1opc, isRSUB ) ) {
                      i++ ;
                      if ( i == text.at(line).size() || text.at(line).at(i).tokenname == ".") {
                          return true ;
                      } // if
                  } // if
                  else if ( findopc( text.at(line).at(i).position, opc, f2ins, f2opc, isRSUB ) ) {
                      i++ ;
                      if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 3 ) {
                          i++ ;
                          if ( i == text.at(line).size() || text.at(line).at(i).tokenname == "." ) {
                              return true ;
                          } // if
                          else if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "," ) {
                              i++ ;
                              if ( i < text.at(line).size()
                                  && (text.at(line).at(i).tablenum == 3 || text.at(line).at(i).tablenum == 6)
                                  && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." )) {
                                  return true ;
                              } // if
                          } // else if
                      } // if
                  } // else if
                  else if ( findopc( text.at(line).at(i).position, opc, f34ins, f34opc, isRSUB ) ) { //type3
                      i++ ;
                      if (isRSUB) {
                          if ( i == text.at(line).size() || text.at(line).at(i).tokenname == "." ) {
                              return true ;
                          } // if
                      } // if
                      if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "#" ) {
                          i++ ;
                          if ( i < text.at(line).size() &&( text.at(line).at(i).tablenum == 5 || text.at(line).at(i).tablenum == 6 )
                              && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                              return true ;
                          } // if
                      } // if
                      else if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "@" ) {
                          i++ ;
                          if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 5
                              && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                              return true ;
                          } // if
                      } // else if
                      else if( i < text.at(line).size() && text.at(line).at(i).tokenname == "=" ) {
                          i++ ;
                          if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "'" ) {
                              i++ ;
                              if ( i < text.at(line).size() && (text.at(line).at(i).tablenum == 6 || text.at(line).at(i).tablenum == 7) ) {
                                  i++ ;
                                  if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "'"
                                      && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                                      return true ;
                                  } // if
                              } // if
                          } // if
                      } // else if
                      else if( i < text.at(line).size() && text.at(line).at(i).tablenum == 5 ) {
                          i++ ;
                          if ( i == text.at(line).size() || text.at(line).at(i).tokenname == "." ) {
                              return true ;
                          } // if
                          else if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "," ) {
                              i++ ;
                              if ( i < text.at(line).size() && (text.at(line).at(i).tokenname == "x" || text.at(line).at(i).tokenname == "X")
                                  && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                                  return true ;
                              } // if
                          } // else if
                      } // else if
                      else if( i < text.at(line).size() && text.at(line).at(i).tablenum == 6 ) {
                          if ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) {
                              return true ;
                          } // if
                      } // else if
                  } // else if
              } // if
              else if ( text.at(line).at(i).tokenname == "+" ) { // +
                  i++ ;
                  if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 1 && findopc(text.at(line).at(i).position, opc, f34ins, f34opc, isRSUB ) ) {
                      i++ ;
                      if (isRSUB) {
                          if ( i == text.at(line).size() || text.at(line).at(i).tokenname == "." ) {
                              return true ;
                          } // if
                      } // if
                      if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "#" ) {
                          i++ ;
                          if ( i < text.at(line).size() &&( text.at(line).at(i).tablenum == 5 || text.at(line).at(i).tablenum == 6 )
                              && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                              return true ;
                          } // if
                      } // if
                      else if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "@" ) {
                          i++ ;
                          if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 5
                              && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                              return true ;
                          } // if
                      } // else if
                      else if( i < text.at(line).size() && text.at(line).at(i).tokenname == "=" ) {
                          i++ ;
                          if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "'" ) {
                              i++ ;
                              if ( i < text.at(line).size() && (text.at(line).at(i).tablenum == 6 || text.at(line).at(i).tablenum == 7) ) {
                                  i++ ;
                                  if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "'"
                                      && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                                      return true ;
                                  } // if
                              } // if
                          } // if
                      } // else if
                      else if( i < text.at(line).size() && text.at(line).at(i).tablenum == 5 ) {
                          i++ ;
                          if ( i == text.at(line).size() || text.at(line).at(i).tokenname == "." ) {
                              return true ;
                          } // if
                          else if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "," ) {
                              i++ ;
                              if ( i < text.at(line).size() && (text.at(line).at(i).tokenname == "x" || text.at(line).at(i).tokenname == "X")
                                  && ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                                  return true ;
                              } // if
                          } // else if
                      } // else if
                      else if( i < text.at(line).size() && text.at(line).at(i).tablenum == 6 ) {
                          if ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) {
                              return true ;
                          } // if
                      } // else if
                  } // if
              } // else if
          } // else if
          else if ( text.at(line).at(i).tokenname == "." ) { // 只有註解的行
              return true ;
          } // else if
          return false ;
      } // format()

      bool pseudoformat(int line, string &name) {
          int i = 0 ;
          string strhi = "" ;
          if ( text.at(line).at(i).tablenum == 2 ) {
            transform(T2_psedo.at(text.at(line).at(i).position-1).begin(), T2_psedo.at(text.at(line).at(i).position-1).end(), back_inserter(strhi), ::toupper); // 字母轉大寫
          } // if
          if ( strhi == "START" || strhi == "RESB" || strhi == "RESW" ) {
              i++ ;
              if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 6
                  && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == ".") ) {
                  name = strhi ;
                  return true ;
              } // if
          } // if
          else if ( strhi == "END" ) {
              i++ ;
              if ( i == text.at(line).size() || text.at(line).at(i).tokenname == "." ) {
                  name = "END" ;
                  return true ;
              } // if
              else if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 5
                  && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == ".") ) {
                  name = "END" ;
                  return true ;
              } // else if
          } // else if
          else if ( strhi == "BYTE" || strhi == "WORD" ) {
              i++ ;
              if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 5
                  && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." )) {
                  name = strhi ;
                  return true ;
              } // if
              else if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "'" ) {
                  i++ ;
                  if ( i < text.at(line).size() && (text.at(line).at(i).tablenum == 6 || text.at(line).at(i).tablenum == 7)) {
                      i++ ;
                      if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "'"
                          && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                          name = strhi ;
                          return true ;
                      } // if
                  } // if
              } // else if
          } // else if
          else if (strhi == "BASE") {
              i++ ;
              if ( i < text.at(line).size() && (text.at(line).at(i).tablenum == 5 || text.at(line).at(i).tablenum == 6)
                  && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                  name = "BASE" ;
                  return true ;
              } // if
          } // else if
          else if ( strhi == "LTORG" ) {
              if ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) {
                  name = "LTORG" ;
                  return true ;
              } // if
          } // else if
          else if ( text.at(line).at(i).tablenum == 5 ) {
              i++ ;
              string strhi2 = "" ;
              if ( text.at(line).at(i).tablenum == 2 ) {
                transform(T2_psedo.at(text.at(line).at(i).position-1).begin(), T2_psedo.at(text.at(line).at(i).position-1).end(), back_inserter(strhi2), ::toupper); // 字母轉大寫
              } // if
              if ( strhi2 == "START" || strhi2 == "RESB" || strhi2 == "RESW" ) {
                  i++ ;
                  if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 6
                      && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == ".") ) {
                      name = strhi2 ;
                      return true ;
                  } // if
              } // if
              else if ( strhi2 == "END" ) {
              i++ ;
              if ( i == text.at(line).size() || text.at(line).at(i).tokenname == "." ) {
                  name = "END" ;
                  return true ;
              } // if
              else if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 5
                  && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == ".") ) {
                  name = "END" ;
                  return true ;
              } // else if
          } // else if
              else if ( strhi2 == "BYTE" || strhi2 == "WORD" ) {
              i++ ;
              if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 5
                  && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." )) {
                  name = strhi2 ;
                  return true ;
              } // if
              else if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "'" ) {
                  i++ ;
                  if ( i < text.at(line).size() && (text.at(line).at(i).tablenum == 6 || text.at(line).at(i).tablenum == 7)) {
                      i++ ;
                      if ( i < text.at(line).size() && text.at(line).at(i).tokenname == "'"
                          && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                          name = strhi2 ;
                          return true ;
                      } // if
                  } // if
              } // else if
          } // else if
              else if (strhi2 == "BASE") {
              i++ ;
              if ( i < text.at(line).size() && (text.at(line).at(i).tablenum == 5 || text.at(line).at(i).tablenum == 6)
                  && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                  name = "BASE"  ;
                  return true ;
              } // if
          } // else if
              else if ( strhi2 == "LTORG" ) {
                  if ( (i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) {
                      name = "LTORG" ;
                      return true ;
                  } // if
              } // else if
              else if ( strhi2 == "EQU" ) {
                  i++ ;
                  if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 6
                      && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                      name = "EQU" ;
                      return true ;
                  } // if
                  else if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 5 ) {
                        i++ ;
                      if ( i < text.at(line).size()
                          && (text.at(line).at(i).tokenname == "+" || text.at(line).at(i).tokenname == "-"
                              ||text.at(line).at(i).tokenname == "*" || text.at(line).at(i).tokenname == "/") ) {
                          if ( i < text.at(line).size() && text.at(line).at(i).tablenum == 5
                              && ((i+1) == text.at(line).size() || text.at(line).at(i+1).tokenname == "." ) ) {
                              name = "EQU" ;
                              return true ;
                          } // if
                      } // if
                  } // else if
              } // else if
          } // else if

          return false ;
      } // pseudo format

};
static SICXE sicxe = SICXE() ;

void initial() {
    vector <string> temp ;
    inputData = temp ;
    tokenpos = temp ;
    T1_instr = temp ;
    T2_psedo = temp ;
    T3_reg = temp ;
    T4_delim = temp ;
    T5_sym = temp ;
    T6_int = temp ;
    T7_str = temp ;
} // initial()

void UserInfor() { // Information for user
    cout << "\n** Homework1&2: SIC,SIC/XE Assembler **\n" ;
    cout << "* 0. QUIT                             *\n" ;
    cout << "* 1. Lexical Analysis (Token Report)  *\n" ;
    cout << "* 2. Syntax Analysis (SIC)            *\n" ;
    cout << "* 3. Syntax Analysis (SIC/XE)         *\n" ;
    cout << "***************************************\n" ;
    cout << "Input a choice(0, 1, 2, 3): " ;
} // UserInfor()

string inputfile ;

int main()
{
    bool setting = false ;
    string cmd ;
    UserInfor() ;
    cin >> cmd ;
    while ( cmd != "0" ) {
        if ( cmd == "1" ) {
            cout << endl << "Please input a filename (include name and extension) : " ;
            cin >> cmd ;
            inputfile = cmd ;
            if ( !token.Read(cmd,inputData) ) {
                cout << endl << "File does not exist!" << endl ;
            } // if
            else {
                token.preTable() ;
                token.getToken() ;
                token.Write(inputfile) ;
                setting = true ;
            /*
                for ( int i = 0 ; i != text.size() ; i++ ) {
                    for ( int j = 0 ; j != text.at(i).size() ; j++ ) {
                        cout << text.at(i).at(j).tablenum << "," << text.at(i).at(j).position << "," << text.at(i).at(j).tokenname << endl ;
                    } // for
                    cout << endl ;
                } // for
            */
            } // else
        } // if
        else if ( cmd == "2" ) {
            if ( !setting ) {
                cout << endl << "Please choose option 1 first!" ;
            } // if
            else {
                string opcode ;
               /*
                for ( int line = 0 ; line != tokenpos.size() ; line++ ) {
                    if ( sic.Format(tokenpos.at(line), opcode) ) { // 格式正確
                        cout << opcode << endl ;
                    } // if
                } // for
                */
            } // else
        } // else if
        else if ( cmd == "3" ) {
            if ( !setting ) {
                cout << endl << "Please choose option 1 first!" ;
            } // if
            else {
                for ( int line = 0 ; line != text.size() ; line++ ) {
                        string opcode ;
                        string name ;
                    if ( sicxe.format( line, opcode ) || sicxe.pseudoformat( line, name) ) { // 格式正確
                        cout << opcode << endl ;
                        cout << name << endl ;
                    } // if
                } // for
            } // else
        } // else if
        else {
            cout << endl << "Command does not exist!" << endl ;
        } // else

        UserInfor() ;
        cin >> cmd ;
    } // while
    initial() ;
    return 0;
} // main()
