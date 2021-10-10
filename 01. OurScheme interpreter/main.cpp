# include <iostream>
# include <sstream> // stringstream
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <string>
# include <vector>
# include <map>

using namespace std;


enum TokenType {
  LEFTPAREN, RIGHTPAREN, INTEGER, STRING, DOT, FLOATNUM, NIL, TRUE, QUOTE, SYMBOL,
  FILEEND, ERROR_STRENTER, ERROR_NOINPUT
};
// FILEEND : (exit) 正常結束
// ERROR_STRENTER : 換行(string不可跨行，輸出ERROR) // ERROR_num = 1
// ERROR_NOINPUT : 還未結束就無input了 // ERROR_num = 0
enum NodeType {
  LEAF, INTER
};

struct TokenNode {
  string str; // token內容
  int int_num ;
  float flo_num ;
  int line;
  int column;
  TokenType type;
};

struct TreeNode{
  string str;
  int int_num ;
  float flo_num ;
  bool already_eval ; // false
  TokenType tokenType;
  NodeType nodeType;
  TreeNode * right;
  TreeNode * left;
};
typedef TreeNode * TreeNodePtr ;

int gLine = 1;   // 下一個準備讀進來的char所在行數
int gColumn = 1; // 下一個準備讀進來的char所在列數
vector<TokenNode> gAllToken; // 暫存所有切好的tokens
vector<TokenNode> gStandardSexp; // 標準化後的token list(非原tokens內的token，str == "" )
map<string, TreeNodePtr> gMap; // 儲存define symbol(變數表)
int gCounterLevel = 0 ;

class Tree {
private:
  int m_getNextLine;   // 目前取得的char所在行數
  int m_getNextColumn; // 目前取得的char所在列數
  char m_getNextChar; // 目前取得的char內容
  bool m_EndOfFile; // 是否已讀完所有input

  bool IsWhiteSpace( char ch ) {
    if ( ch == ' ' || ch == '\n' || ch == '\t' )
      return true;
    else
      return false;
  } // IsWhiteSpace() 空白、tab、換行

  bool IsDigit( char ch ) {
    if ( ch - '0' >= 0 && ch - '0' <= 9 )
      return true;
    else
      return false;
  } // IsDigit() 0-9

  bool IsDigitStr( string str ) {
    for ( int i = 0; i < str.size() ; i++ ) {
      if ( !IsDigit( str[i] ) ) {
        return false;
      } // if
    } // for

    return true;
  } // IsDigitStr() 此string皆為數字

  bool IsFloatWithoutSign( string str ) {
    bool haveADot = false; // 有無一個'.'了
    for ( int i = 0; i < str.size() ; i++ ) {
      if ( ( !IsDigit( str[i] ) && str[i] != '.' ) || ( haveADot && str[i] == '.' ) ) {
        // 不是數字也不是'.'||已經有'.'卻又來一個'.'
        return false;
      } // if

      if ( str[i] == '.' ) {
        haveADot = true;
      } // if
    } // for

    return true;
  } // IsFloatWithoutSign() 此string為無正負號的float

  void SkipWhiteSpace( char &ch ) {
    while ( IsWhiteSpace( ch ) ) {
      ch = GetNextChar();
    } // while
  } // SkipWhiteSpace() 省略空白

  void ReadUntilSeparators( char &ch, string &str ) {
    while ( ch != ' ' && ch != '(' && ch != ')' && ch != '\'' && ch != '"' && ch != ';' && ch != '\0' ) {
      str = str + ch;
      ch = GetNextChar();
    } // while
  } // ReadUntilSeparators() 不斷讀，直到遇到separators // 多加'\0'作為separator

  char GetNextChar() {
    char temp = '\0';
    m_getNextLine = gLine;    // 更新目前要讀入的字元line和column
    m_getNextColumn = gColumn;
    if ( scanf( "%c", &temp ) == EOF ) {
      m_EndOfFile = true;
      return '\0';
    } // if
    else {
      if ( temp == '\n' ) {
        gLine++;
        gColumn = 1;
        return '\0';
      } // if
      else {
        gColumn++;
        return temp;
      } // else
    } // else
  } // GetNextChar() 讀取一個字元，並更新line和column相關資訊(若為換行'\n'，回傳'\0')
  // ============以上是get token所使用到的============
public :

  Tree() {
    m_getNextLine = 1;   // 目前取得的char所在行數
    m_getNextColumn = 1; // 目前取得的char所在列數
    m_getNextChar = ' '; // 目前取得的char內容
    m_EndOfFile = false; // 是否已讀完所有input
  } // Tree() initial

  TokenNode GetNextToken() { // **第一次運作時，get next char為初始值'空白'**
    TokenNode tempToken; // 暫存目前切到的token
    string str ;
    if ( IsWhiteSpace( m_getNextChar ) ) {
      SkipWhiteSpace( m_getNextChar );
      return GetNextToken();
    } // if
    else if ( m_getNextChar == ';' ) { // 註解
      m_getNextChar = GetNextChar();
      while ( m_getNextChar != '\0' ) { // 此行註解後的東西都不要
        m_getNextChar = GetNextChar();
      } // while

      if ( m_EndOfFile ) { // m_getNextChar == '\0'有此可能
        str = "ERROR_NOINPUT";
        throw str ;
      } // if

      m_getNextChar = GetNextChar(); // 先取好下一char
      return GetNextToken();
    } // else if
    else if ( m_getNextChar == '\0' ) { // 換行或沒input了
      if ( m_EndOfFile ) {
        str = "ERROR_NOINPUT" ;
        throw str ;
      } // if
      else { // 讀到換行
        m_getNextChar = GetNextChar(); // 先取好下一char
        return GetNextToken();
      } // else
    } // else if
    else if ( m_getNextChar == '(' ) { // '(' || () = NIL
      tempToken.line = m_getNextLine;
      tempToken.column = m_getNextColumn;
      tempToken.str = m_getNextChar; // 先存
      m_getNextChar = GetNextChar() ;
      if ( m_getNextChar == ')' ) { // nil
        tempToken.str = "nil" ;
        tempToken.type = NIL ;
        gAllToken.push_back( tempToken ) ;
        m_getNextChar = GetNextChar() ;
        return tempToken ;
      } // if
      else if ( m_getNextChar == '\0' || m_getNextChar == ' ' || m_getNextChar == '\t' ) {
        // 換行或nil
        while ( ( m_getNextChar == '\0' || m_getNextChar == ' ' || m_getNextChar == '\t' )
                && !m_EndOfFile ) { // 是換行而不是讀不到東西
          m_getNextChar = GetNextChar() ;
        } // while

        if ( m_EndOfFile ) { // 只是左括號(不用再先讀了)
          tempToken.type = LEFTPAREN ;
          gAllToken.push_back( tempToken ) ;
          return tempToken ;
        } // if
        else if ( m_getNextChar == ')' ) { // nil
          tempToken.str = "nil" ;
          tempToken.type = NIL ;
          gAllToken.push_back( tempToken ) ;
          m_getNextChar = GetNextChar() ;
          return tempToken ;
        } // else if
        else { // 只是左括號(不用再先讀了)
          tempToken.type = LEFTPAREN ;
          gAllToken.push_back( tempToken ) ;
          return tempToken ;
        } // else
      } // else if
      else { // 只是左括號(不用再先讀了)
        tempToken.type = LEFTPAREN ;
        gAllToken.push_back( tempToken ) ;
        return tempToken ;
      } // else
    } // else if
    else if ( m_getNextChar == ')' ) {
      tempToken.str = ")";
      tempToken.line = m_getNextLine;
      tempToken.column = m_getNextColumn;
      tempToken.type = RIGHTPAREN;
      gAllToken.push_back( tempToken );
      m_getNextChar = GetNextChar(); // 先取好下一char
      return tempToken;
    } // else if
    else if ( m_getNextChar == '"' ) { // 字串開頭
      tempToken.line = m_getNextLine;
      tempToken.column = m_getNextColumn;
      tempToken.type = STRING;
      tempToken.str = "\""; // 先存入'"'
      bool slash = false; // 斜槓'\'有無出現在前一字元
      m_getNextChar = GetNextChar();
      while ( m_getNextChar != '"' || slash ) { // 使得前一個字元是斜槓'\'，此時字元又剛好是'"'時也可繼續執行
        slash = false;
        if ( m_getNextChar == '\0' ) {
          if ( m_EndOfFile ) {
            str = "ERROR_NOINPUT";
            throw str ;
          } // if
          else { // 換行(string不可跨行，輸出ERROR)
            tempToken.type = ERROR_STRENTER;
            gAllToken.push_back( tempToken );
            string str1 = "ERROR_STRENTER" ;
            throw str1 ;
          } // else
        } // if
        else if ( m_getNextChar == '\\' ) { // 斜槓'\'
          slash = true;
        } // else if

        tempToken.str = tempToken.str + m_getNextChar;
        m_getNextChar = GetNextChar();
      } // while

      tempToken.str = tempToken.str + m_getNextChar; // "
      gAllToken.push_back( tempToken );
      m_getNextChar = GetNextChar();
      return tempToken;
    } // else if
    else if ( m_getNextChar == '.' || m_getNextChar == '+' || m_getNextChar == '-'
              || IsDigit( m_getNextChar ) ) {
      // maybe INTEGER || FLOATNUM || SYMBOL || DOT
      tempToken.line = m_getNextLine;
      tempToken.column = m_getNextColumn;
      tempToken.str = m_getNextChar; // 先存入

      string str; // 暫存擷取string
      if ( m_getNextChar == '.' ) { // FLOATNUM || SYMBOL || DOT
        m_getNextChar = GetNextChar();
        ReadUntilSeparators( m_getNextChar, tempToken.str ); // 一直讀到遇到separator為止
        // 接著在return前不用先get char了，已讀到一個separator要等下一輪處理了!
        if ( tempToken.str == "." ) { // DOT
          tempToken.type = DOT;
          gAllToken.push_back( tempToken );
          return tempToken;
        } // if

        str = tempToken.str.substr( 1, tempToken.str.size() - 1 ); // (start index, length)
        if ( IsDigitStr( str ) ) { // FLOATNUM(.XXX)
          tempToken.type = FLOATNUM;
          tempToken.flo_num = atof( tempToken.str.c_str() ) ;
          gAllToken.push_back( tempToken );
          return tempToken;
        } // if
        else { // SYMBOL
          tempToken.type = SYMBOL;
          gAllToken.push_back( tempToken );
          return tempToken;
        } // else
      } // if
      else if ( m_getNextChar == '+' || m_getNextChar == '-' ) { // INTEGER || FLOATNUM || SYMBOL
        m_getNextChar = GetNextChar();
        ReadUntilSeparators( m_getNextChar, tempToken.str ); // 一直讀到遇到separator為止
        // 接著在return前不用先get char了，已讀到一個separator要等下一輪處理了!
        str = tempToken.str.substr( 1, tempToken.str.size() - 1 ); // (start index, length)
        if ( str.size() != 0 && IsDigitStr( str ) ) { // INTEGER
          tempToken.type = INTEGER;
          tempToken.int_num = atoi( tempToken.str.c_str() ) ;
          gAllToken.push_back( tempToken );
          return tempToken;
        } // if
        else if ( str.size() != 0 && IsFloatWithoutSign( str ) ) { // FLOATNUM
          tempToken.type = FLOATNUM;
          tempToken.flo_num = atof( tempToken.str.c_str() ) ;
          gAllToken.push_back( tempToken );
          return tempToken;
        } // else if
        else { // SYMBOL // '+' || '-'
          tempToken.type = SYMBOL;
          gAllToken.push_back( tempToken );
          return tempToken;
        } // else
      } // else if
      else { // IsDigit(m_getNextChar) // INTEGER || FLOATNUM || SYMBOL
        m_getNextChar = GetNextChar();
        ReadUntilSeparators( m_getNextChar, tempToken.str ); // 一直讀到遇到separator為止
        // 接著在return前不用先get char了，已讀到一個separator要等下一輪處理了!
        if ( IsDigitStr( tempToken.str ) ) { // INTEGER
          tempToken.type = INTEGER;
          tempToken.int_num = atoi( tempToken.str.c_str() ) ;
          gAllToken.push_back( tempToken );
          return tempToken;
        } // if
        else if ( IsFloatWithoutSign( tempToken.str ) ) { // FLOATNUM
          tempToken.type = FLOATNUM;
          tempToken.flo_num = atof( tempToken.str.c_str() ) ;
          gAllToken.push_back( tempToken );
          return tempToken;
        } // else if
        else { // SYMBOL
          tempToken.type = SYMBOL;
          gAllToken.push_back( tempToken );
          return tempToken;
        } // else
      } // else
    } // else if
    else if ( m_getNextChar == '#' || m_getNextChar == 'n' || m_getNextChar == 't' ) {
      // NIL(#f、nil) || T(#t、t) ||SYMBOL
      tempToken.line = m_getNextLine;
      tempToken.column = m_getNextColumn;
      tempToken.str = m_getNextChar; // 先存入
      m_getNextChar = GetNextChar();
      ReadUntilSeparators( m_getNextChar, tempToken.str ); // 一直讀到遇到separator為止
      if ( tempToken.str == "#f" || tempToken.str == "nil" ) { // NIL(#f、nil) // 大小寫不同就不算
        tempToken.str = "nil";
        tempToken.type = NIL;
        gAllToken.push_back( tempToken );
        return tempToken;
      } // if
      else if ( tempToken.str == "#t" || tempToken.str == "t" ) { // T(#t、t) // 大小寫不同就不算
        tempToken.str = "#t";
        tempToken.type = TRUE;
        gAllToken.push_back( tempToken );
        return tempToken;
      } // else if
      else { // SYMBOL
        tempToken.type = SYMBOL;
        gAllToken.push_back( tempToken );
        return tempToken;
      } // else
    } // else if
    else if ( m_getNextChar == '\'' ) {
      tempToken.line = m_getNextLine;
      tempToken.column = m_getNextColumn;
      tempToken.str = m_getNextChar;
      tempToken.type = QUOTE;
      gAllToken.push_back( tempToken );
      m_getNextChar = GetNextChar();
      return tempToken;
    } // else if
    else { // Symbol
      tempToken.line = m_getNextLine;
      tempToken.column = m_getNextColumn;
      tempToken.str = m_getNextChar;
      tempToken.type = SYMBOL;
      m_getNextChar = GetNextChar();
      ReadUntilSeparators( m_getNextChar, tempToken.str ); // 一直讀到遇到separator為止
      gAllToken.push_back( tempToken );
      return tempToken;
    } // else
  } // GetNextToken() get next token(一定會給出一個token(type))

  bool ATOM( TokenType temp ) {
    if ( temp == SYMBOL || temp == INTEGER || temp == FLOATNUM || temp == STRING ||
         temp == NIL || temp == TRUE ) {
      return true;
    } // if

    return false;
  } // ATOM() (文法)

  TreeNodePtr Sexp( TokenNode temp, TreeNodePtr walk ) {
    /*
    if ( temp == FILEEND ) { // 正常結束(無error)||(exit)
      END_OF_FILE = true;
      return true ;
    } // if
    */
    TreeNodePtr one = NULL ;
    TreeNodePtr two = NULL ;
    string str ;
    if ( walk == NULL ) {
      walk = new TreeNode ;
      walk->already_eval = false ;
      walk->str = "(" ;
      walk->tokenType = LEFTPAREN ;
      walk->left = NULL ;
      walk->right = NULL ;
    } // if

    if ( ATOM( temp.type ) ) {
      walk->str = temp.str ;
      walk->flo_num = temp.flo_num ;
      walk->int_num = temp.int_num ;
      walk->tokenType = temp.type ;
      return walk;
    } // if
    else if ( temp.type == LEFTPAREN ) {
      // <S-exp>{<S-exp>}(重複0到多次)
      temp = GetNextToken(); // 讀一個token
      one = Sexp( temp, walk->left ) ;
      walk->left = one ;

      temp = GetNextToken(); // 讀一個token
      TreeNodePtr right = walk ;
      while ( temp.type != DOT && temp.type != RIGHTPAREN ) {
        right->right = new TreeNode ;
        right = right->right ;
        right->already_eval = false ;
        right->str = "(" ;
        right->tokenType = LEFTPAREN ;
        right->right = NULL ;
        right->left = NULL ;
        one = Sexp( temp, right->left ) ;
        right->left = one ;
        temp = GetNextToken(); // 讀一個token(防止無窮迴圈!!!)
      } // while

      // [DOT<S-exp>](要或不要)
      if ( temp.type == DOT ) { // 要(前一個也不可為'(')
        temp = GetNextToken(); // 讀一個token
        one = Sexp( temp, right->right ) ;
        right->right = one ;
        temp = GetNextToken() ;
      } // if
      else {
        two = new TreeNode ;
        two->already_eval = false ;
        two->str = "nil" ;
        two->tokenType = NIL ;
        two->right = NULL ;
        two->left = NULL ;
        right->right = two ;
      } // else

      // 不要
      if ( temp.type == RIGHTPAREN ) {
        return walk ;
      } // if
      else { // 非右括號做結束時
        str = "ERROR (unexpected token) = )" ;
        throw str ;
      } // else
    } // else if
    else if ( temp.type == QUOTE ) {
      TreeNodePtr three = walk ;
      three->left = new TreeNode ;
      three->left->already_eval = false ;
      three->left->str = "quote" ;
      three->left->tokenType = QUOTE ;
      three->left->left = NULL ;
      three->left->right = NULL ;
      three->right = new TreeNode ;
      three = three->right ;
      three->already_eval = false ;
      three->str = "(" ;
      three->tokenType = LEFTPAREN ;
      three->left = NULL ;
      three->right = NULL ;
      temp = GetNextToken() ;
      three->left = Sexp( temp, three->left ) ;
      three->right = new TreeNode ;
      three = three->right ;
      three->already_eval = false ;
      three->str = "nil" ;
      three->tokenType = NIL ;
      three->right = NULL ;
      three->left = NULL ;
      return walk ;
    } // else if
    else { // ATOM錯 || '('錯
      str = "ERROR (unexpected token) = (" ;
      throw str ;
    } // else
  } // Sexp() (文法)

  void InitialToGetWhiteSpace( int &whiteSpaceNum ) { // 前一個文法沒錯,取與下一文法開頭之間有幾個whiteSpace
    // 在下一個文法也在前一個文法同行的情況才會計算
    if ( m_getNextChar != '\0' ) { // 先取好的下一字元也不為換行
      while ( m_getNextChar == ' ' || m_getNextChar == '\t' ) { // 若為white space
        whiteSpaceNum++ ;
        m_getNextChar = GetNextChar() ;
      } // while

      if ( m_getNextChar == ';' ) { // 當取完white space發現之後是註解
        whiteSpaceNum = 0 ;
        while ( m_getNextChar != '\0' ) {
          m_getNextChar = GetNextChar() ;
        } // while
      } // if

      if ( m_getNextChar == '\0' ) { // 遇到換行(下一文法不在此行)
        whiteSpaceNum = 0 ;
        if ( !m_EndOfFile ) {
          m_getNextChar = GetNextChar() ;
        } // if

      } // if

    } // if
    else { // 讓先取好地跳過這行吧(先取好的為\n時)
      if ( !m_EndOfFile ) {
        m_getNextChar = GetNextChar() ;
      } // if
    } // else
  } // InitialToGetWhiteSpace()

  void DeleteERRORCurrentLine() { // 刪除發生ERROR的line之後的東西直到換行

    if ( m_getNextChar != '\0' ) { // 提前取的下一個char也不能是換行
      while ( m_getNextChar != '\0' ) { // 不為換行 或 EOF
        m_getNextChar = GetNextChar() ;
      } // while

      if ( !m_EndOfFile ) { // 是換行
        m_getNextChar = GetNextChar() ;
      } // if

    } // if
    else {
      if ( !m_EndOfFile ) {
        m_getNextChar = GetNextChar() ;
      } // if

    } // else
  } // DeleteERRORCurrentLine()

  TreeNodePtr ReadSExp( bool &END_OF_FILE, int &ERRORNum, int whiteSpaceNum, bool first_run ) {
    // error輸出
    // 初始化line & column
    if ( m_getNextChar == '\0' && !m_EndOfFile ) { // 換行
      gLine = 2 ; // 行目前char是換行的那行
    } // if
    else {
      gLine = 1;   // 下一個準備讀進來的char所在行數
    } // else

    if ( whiteSpaceNum != 0 ) {
      gColumn = whiteSpaceNum + 1 ;
    } // if
    else {
      gColumn = 1 ;
    } // else
    // 目前已經在換行後的第一個char(若上一個文法和目前文法中間有換行)
    m_getNextLine = gLine ;
    if ( first_run ) {
      gColumn-- ;
    } // if

    m_getNextColumn = gColumn ;
    gColumn++ ; // 因為是下一個的column值
    try {
      TokenNode nextToken = GetNextToken(); // 第一個token
      TreeNodePtr root = Sexp( nextToken, NULL ) ;
      return root ;
    } // try
    catch( string str ) {
      ERRORNum = 1 ;
      if ( str == "ERROR_NOINPUT" ) {
        printf( "ERROR (no more input) : END-OF-FILE encountered" ) ;
        END_OF_FILE = true ;
      } // if
      else if ( str == "ERROR_STRENTER" ) {
        printf( "ERROR (no closing quote) : END-OF-LINE encountered at Line %d Column %d\n\n",
                m_getNextLine, m_getNextColumn ) ; // 那個換行的位置
        gAllToken.clear() ;
      } // else if
      else if ( str == "ERROR (unexpected token) = (" ) {
        printf( "ERROR (unexpected token) : " ) ;
        printf( "atom or '(' expected when token at Line %d Column %d is >>%s<<\n\n",
                gAllToken[gAllToken.size()-1].line, gAllToken[gAllToken.size()-1].column,
                gAllToken[gAllToken.size()-1].str.c_str() ) ;
        gAllToken.clear() ;
      } // else if
      else if ( str == "ERROR (unexpected token) = )" ) {
        printf( "ERROR (unexpected token) : " ) ;
        printf( "')' expected when token at Line %d Column %d is >>%s<<\n\n",
                gAllToken[gAllToken.size()-1].line, gAllToken[gAllToken.size()-1].column,
                gAllToken[gAllToken.size()-1].str.c_str() ) ;
        gAllToken.clear() ;
      } // else if
      /*
      else { // 2
        ERRORNum = 2 ;
        printf( "> ERROR (unexpected token) : " ) ;
        printf( "atom or '(' expected when token at Line %d Column %d is >>%s<<\n\n",
                gAllToken[gAllToken.size()-1].line, gAllToken[gAllToken.size()-1].column,
                gAllToken[gAllToken.size()-1].str.c_str() ) ;
        gAllToken.clear() ;
      } // else
      */
      return NULL ;
    } // catch()
  } // ReadSExp() read s-exp

  void PreorderLeft( TreeNodePtr &tree, int level, bool &PreIsLeftP ) { // 前序遍歷往左邊走
    bool needRightP = false ; // 是否需要印出對應右括號
    if ( tree == NULL ) {
      return ;
    } // if
    else if ( tree->tokenType == INTEGER || tree->tokenType == FLOATNUM ) {
      NumHasPlus( tree->str ) ;

      if ( tree->tokenType == FLOATNUM ) {
        FloatThreeNum( tree->str ) ;
      } // if

      if ( !PreIsLeftP ) {
        for ( int i = 0 ; i < level*2 ; i++ ) {
          printf( " " ) ;
        } // for
      } // if
      else {
        PreIsLeftP = false ;
      } // else

      printf( "%s\n", tree->str.c_str() ) ;
    } // else if
    else if ( tree->tokenType == STRING ) {
      int nextP = 0 ; // 下一次要印的起始點index
      if ( !PreIsLeftP ) {
        for ( int i = 0 ; i < level*2 ; i++ ) {
          printf( " " ) ;
        } // for
      } // if
      else {
        PreIsLeftP = false ;
      } // else

      SpecialInStr( tree->str, nextP ) ;

      if ( nextP == 0 ) { // 沒印過
        printf( "%s\n", tree->str.c_str() ) ;
      } // if
      else { // 印過
        printf( "%s\n", tree->str.substr( nextP, tree->str.size()-1 ).c_str() ) ;
      } // else
    } // else if
    else if ( tree->str == "(" ) {
      // 非標準化加的左括號(本來輸入就有的)
      needRightP = true ;
      if ( !PreIsLeftP ) { // 若前一個非左括號
        for ( int i = 0 ; i < level*2 ; i++ ) {
          printf( " " ) ;
        } // for
      } // if
      else {
        PreIsLeftP = false ;
      } // else
      /*
      if ( ( tree->str == "quote" && tree->tokenType == QUOTE ) ) {
        PreIsLeftP = false ;
      } // if
      else {
        PreIsLeftP = true ;
      } // else

      if ( tree->str == "quote" && tree->tokenType == QUOTE ) {
        printf( "( %s\n", tree->str.c_str() ) ;
      } // if
      else {
       // printf( "%s ", tree->str.c_str() ) ;
      } // else
      */
      printf( "%s ", tree->str.c_str() ) ;
      PreIsLeftP = true ;
      level++ ; // 左括號層數+1
    } // else if
    else if ( tree->tokenType == SYMBOL && IsInterFun( tree->str ) && tree->already_eval ) {
      if ( !PreIsLeftP ) {
        for ( int i = 0 ; i < level*2 ; i++ ) {
          printf( " " ) ;
        } // for
      } // if
      else {
        PreIsLeftP = false ;
      } // else

      cout << "#<procedure " << tree->str << ">" << endl ;
    } // else if
    else { // nil要輸出(輸入時就有的) & 其他
      if ( !PreIsLeftP ) {
        for ( int i = 0 ; i < level*2 ; i++ ) {
          printf( " " ) ;
        } // for
      } // if
      else {
        PreIsLeftP = false ;
      } // else

      printf( "%s\n", tree->str.c_str() ) ;
    } // else if

    PreorderLeft( tree->left, level, PreIsLeftP ) ;
    PreorderRight( tree->right, level, PreIsLeftP ) ;

    if ( needRightP ) {
      for ( int i = 0 ; i < ( level-1 ) *2 ; i++ ) {
        printf( " " ) ;
      } // for

      printf( ")\n" ) ;
    } // if

  }  // PreorderLeft()

  void PreorderRight( TreeNodePtr & tree, int level, bool &PreIsLeftP ) {
    // 前序遍歷往右邊走 // 右邊的龍骨'('和nil都不可以輸出(是標準化時加的) // 輸出右邊的東西時要先輸出點
    if ( tree == NULL ) {
      return ;
    } // if
    else if ( tree->tokenType == INTEGER || tree->tokenType == FLOATNUM ) {
      NumHasPlus( tree->str ) ;

      if ( tree->tokenType == FLOATNUM ) {
        FloatThreeNum( tree->str ) ;
      } // if
      // 先輸出點'.'
      for ( int i = 0 ; i < level*2 ; i++ ) {
        printf( " " ) ;
      } // for

      printf( ".\n" ) ;
      // 再輸出後面東西
      if ( !PreIsLeftP ) {
        for ( int i = 0 ; i < level*2 ; i++ ) {
          printf( " " ) ;
        } // for
      } // if
      else {
        PreIsLeftP = false ;
      } // else

      printf( "%s\n", tree->str.c_str() ) ;
    } // if
    else if ( tree->tokenType == STRING ) {
      int nextP = 0 ; // 下一次要印的起始點index

      // 先輸出點'.'
      for ( int i = 0 ; i < level*2 ; i++ ) {
        printf( " " ) ;
      } // for

      printf( ".\n" ) ;
      // 再輸出後面東西
      SpecialInStr( tree->str, nextP ) ;

      if ( nextP == 0 ) { // 沒印過
        if ( !PreIsLeftP ) {
          for ( int i = 0 ; i < level*2 ; i++ ) {
            printf( " " ) ;
          } // for
        } // if
        else {
          PreIsLeftP = false ;
        } // else

        printf( "%s\n", tree->str.c_str() ) ;
      } // if
      else { // 印過
        printf( "%s\n", tree->str.substr( nextP, tree->str.size()-1 ).c_str() ) ;
      } // else
    } // else if
    else if ( tree->tokenType == SYMBOL && IsInterFun( tree->str ) && tree->already_eval ) {
      // 先輸出點'.'
      for ( int i = 0 ; i < level*2 ; i++ ) {
        printf( " " ) ;
      } // for

      printf( ".\n" ) ;

      if ( !PreIsLeftP ) {
        for ( int i = 0 ; i < level*2 ; i++ ) {
          printf( " " ) ;
        } // for
      } // if
      else {
        PreIsLeftP = false ;
      } // else

      cout << "#<procedure " << tree->str << ">" << endl ;
    } // else if
    else if ( tree->str != "" && tree->tokenType != LEFTPAREN && tree->tokenType != NIL ) {
      // 右邊的龍骨'('和nil都不可以輸出(是標準化時加的 => str == "" ) // 輸出其他
      // 先輸出點'.'
      for ( int i = 0 ; i < level*2 ; i++ ) {
        printf( " " ) ;
      } // for

      printf( ".\n" ) ;
      // 再輸出後面東西
      if ( !PreIsLeftP ) {
        for ( int i = 0 ; i < level*2 ; i++ ) {
          printf( " " ) ;
        } // for
      } // if
      else {
        PreIsLeftP = false ;
      } // else

      printf( "%s\n", tree->str.c_str() ) ;
    } // else if

    PreorderLeft( tree->left, level, PreIsLeftP ) ;
    PreorderRight( tree->right, level, PreIsLeftP ) ;
  } // PreorderRight()

  void PrintSExp( TreeNodePtr tree ) {
    int treeLevel = 0 ; // 樹的層數 (1,2,...)
    bool preIsLeftP = false ; // 此token的上一個是否為左括號
    if ( ( gStandardSexp.size() == 5 && gStandardSexp[0].str == "(" && gStandardSexp[1].str == "exit"
           && gStandardSexp[2].type == DOT && gStandardSexp[3].type == NIL && gStandardSexp[4].str == ")" )
         || ( gStandardSexp.size() == 1 && gStandardSexp[0].str == "(exit)" ) ) {
      // 若為"(exit)"則不印並結束
      return ;
    } // if

    PreorderLeft( tree, treeLevel, preIsLeftP ) ; // 印出整棵樹
  } // PrintSExp() print s-exp

  void SpecialInStr( string &str, int &nextP ) { // 處理字串中的\n、\t、\"、\\ // nextP初始為0
    // str 為樹內的其中一個type是STRING的token之內容
    for ( int j = 0 ; j < str.size() ; j++ ) {
      if ( str[j] == '\\' ) { // 遇到'\'的時候
        if ( j+1 < str.size()
             && ( str[j+1] == 'n' || str[j+1] == 't' || str[j+1] == '"' || str[j+1] == '\\' ) ) {
          if ( str[j+1] == 'n' ) { // \n
            if ( nextP == j ) { // 剛好是\n
              printf( "\n" ) ;
            } // if
            else {
              printf( "%s\n", str.substr( nextP, j ).c_str() ) ; // \n前面輸出(含換行)
            } // else

            nextP = j + 2 ; // 下一次要印的起始點
            j++ ;
          } // if
          else if ( str[j+1] == 't' ) {
            if ( nextP == j ) { // 剛好是\t
              printf( "\t" ) ;
            } // if
            else {
              printf( "%s\t", str.substr( nextP, j ).c_str() ) ; // \t前面輸出(含\t)
            } // else

            nextP = j + 2 ; // 下一次要印的起始點
            j++ ;
          } // else if
          else if ( str[j+1] == '"' ) {
            str.erase( j, 1 ) ; // 刪掉j字元(刪j=>\)
          } // else if
          else if ( str[j+1] == '\\' ) {
            str.erase( j, 1 ) ; // 刪掉j字元(刪j=>\)
          } // else if

        } // if
      } // if
    } // for
  } // SpecialInStr()

  void NumHasPlus( string &str ) { // 將INTEGER or FLOATNUM token前的'+'刪掉
    // str 為樹內的其中一個type是INTEGER or FLOATNUM的token之內容
    if ( str[0] == '+' ) {
      str.erase( 0, 1 ); // 刪除str內第一個元素'+'
    } // if

  } // NumHasPlus()

  int FindDotPositionInFloat( string &str ) { // str 為樹內的其中一個type是FLOATNUM的token之內容
    int pos = 0 ; // '.'在str內的位置
    for ( int j = 0 ; j < str[j] ; j++ ) { // 找到'.'在哪裡
      if ( str[j] == '.' ) {
        pos = j ;
        return pos ;
      } // if
    } // for
    // 若沒有點(幫你加點在最後面!!)=>Eval()四則運算時用到
    str = str + '.' ;
    pos = str.size() - 1 ;
    return pos ;
  } // FindDotPositionInFloat()

  void FloatThreeNum( string &str ) { // 小數點後四捨五入成三位，不足者補0
    // str 為樹內的其中一個type是FLOATNUM的token之內容
    int pos = FindDotPositionInFloat( str ) ; // '.'在str內的位置
    int check = str.size() - 1 - pos ; // 確認小數點後面是否為三位數
    string temp = "" ; // 要轉float的小數
    if ( str[0] == '+' || str[0] == '-' ) { // 處理temp(無sign)
      temp = str.substr( 1, str.size()-1 ) ;
    } // if
    else {
      temp = str ;
    } // else

    if ( temp[0] == '.' ) {
      temp = "0" + temp ; // 小數點前面先補0再接小數點
    } // if

    float floatNum = atof( temp.c_str() ) ;
    stringstream sstream ;
    // 檢查'.'後有無三個數字
    if ( check > 3 ) { // 做四捨五入
      floatNum = ( int ) ( ( floatNum * 1000 ) + 0.5 ) / 1000.0; // 保留到小數點後三位
      sstream << floatNum ;
      if ( str[0] == '+' || str[0] == '-' ) {
        str = str[0] + sstream.str() ; // float to string
      } // if
      else {
        str = sstream.str() ; // float to string
      } // else
    } // if
    else if ( check < 3 ) { // 補0到有3位
      if ( str == "+." || str == "-." ) { // 小數只有一個'.' => 不用補000(?)
        return ;
      } // if
      else {
        pos = FindDotPositionInFloat( temp ) ;
        while ( ( temp.size() - 1 - pos ) < 3  ) {
          temp = temp + "0" ;
        } // while

        if ( str[0] == '+' || str[0] == '-' ) {
          str = str[0] + temp ;
        } // if
        else {
          str = temp ;
        } // else

      } // else
    } // else if
    else { // 剛好三位
      if ( str[0] == '+' || str[0] == '-' ) {
        str = str[0] + temp ;
      } // if
      else {
        str = temp ;
      } // else

      return ;
    } // else

  } // FloatThreeNum()

  bool IsInterFun( string str ) {
    if ( str == "cons" ) return true ;
    else if ( str == "list" ) return true ;
    else if ( str == "quote" || str == "'" ) return true ;
    else if ( str == "define" ) return true ;
    else if ( str == "car" ) return true ;
    else if ( str == "cdr" ) return true ;
    else if ( str == "atom?" ) return true ;
    else if ( str == "pair?" ) return true ;
    else if ( str == "list?" ) return true ;
    else if ( str == "null?" ) return true ;
    else if ( str == "integer?" ) return true ;
    else if ( str == "real?" ) return true ;
    else if ( str == "number?" ) return true ;
    else if ( str == "string?" ) return true ;
    else if ( str == "boolean?" ) return true ;
    else if ( str == "symbol?" ) return true ;
    else if ( str == "+" ) return true ;
    else if ( str == "-" ) return true ;
    else if ( str == "*" ) return true ;
    else if ( str == "/" ) return true ;
    else if ( str == "not" ) return true ;
    else if ( str == "and" ) return true ;
    else if ( str == "or" ) return true ;
    else if ( str == ">" ) return true ;
    else if ( str == ">=" ) return true ;
    else if ( str == "<" ) return true ;
    else if ( str == "<=" ) return true ;
    else if ( str == "=" ) return true ;
    else if ( str == "string-append" ) return true ;
    else if ( str == "string>?" ) return true ;
    else if ( str == "string<?" ) return true ;
    else if ( str == "string=?" ) return true ;
    else if ( str == "eqv?" ) return true ;
    else if ( str == "equal?" ) return true ;
    else if ( str == "begin" ) return true ;
    else if ( str == "if" ) return true ;
    else if ( str == "cond" ) return true ;
    else if ( str == "clean-environment" ) return true ;
    else if ( str == "exit" ) return true ;
    else ;

    return false ;
  } // IsInterFun()

  // Below are all Eval() function
  TreeNodePtr Cons( TreeNodePtr root ) {
    TreeNodePtr temp = new TreeNode ;
    temp->str = "(" ;
    temp->already_eval = true ;
    temp->tokenType = LEFTPAREN ;
    temp->left = NULL ;
    temp->right = NULL ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ;
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ;
    else if ( root->right->right->right->tokenType != NIL ) ;
    else {
      temp->left = Eval( root->right->left ) ;
      temp->right = Eval( root->right->right->left ) ;
      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : cons" << endl ;
    throw str ;
  } // Cons()

  TreeNodePtr List( TreeNodePtr root ) {
    if ( root->right->tokenType == NIL ) {
      TreeNodePtr temp = new TreeNode ;
      temp->str = "nil" ;
      temp->tokenType = NIL ;
      temp->already_eval = true ;
      temp->left = NULL ;
      temp->right = NULL ;
      return temp ;
    } // if
    else {
      TreeNodePtr temp = new TreeNode ;
      TreeNodePtr temp_walk = temp ;
      temp->str = "(" ;
      temp->tokenType = LEFTPAREN ;
      temp->already_eval = true ;
      temp->left = NULL ;
      temp->right = NULL ;
      TreeNodePtr walk = root ;
      walk = walk->right ;
      while ( walk->tokenType != NIL ) {
        temp_walk->left = Eval( walk->left ) ;
        temp_walk->right = new TreeNode ;
        if ( walk->right->tokenType == NIL ) {
          temp_walk->right->str = "nil" ;
          temp_walk->right->tokenType = NIL ;
          temp_walk->right->left = NULL ;
          temp_walk->right->right = NULL ;
        } // if
        else {
          temp_walk = temp_walk->right ;
          temp_walk->str = "(" ;
          temp_walk->tokenType = LEFTPAREN ;
          temp_walk->right = NULL ;
          temp_walk->left = NULL ;
        } // else

        walk = walk->right ;
      } // while

      return temp ;
    } // else
  } // List()

  TreeNodePtr Define( TreeNodePtr root ) {
    TreeNodePtr temp = new TreeNode ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ;
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ;
    else if ( root->right->right->right->tokenType != NIL ) ;
    else {
      if ( !IsInterFun( root->right->left->str )
           && ( root->right->left->right == NULL && root->right->left->left == NULL )
           && root->right->left->tokenType == SYMBOL ) {
        temp = Eval( root->right->right->left ) ;
        gMap[root->right->left->str] = temp ;
        cout << root->right->left->str << " defined" << endl ;
        string str = "no print" ;
        throw str ;
      } // if

    } // else

    string str = "no print" ;
    cout << "ERROR (DEFINE format) : " ;
    PrintSExp( root ) ;
    throw str ;
  } // Define()

  TreeNodePtr Quote( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType != NIL ) {
      temp = root->right->left ;
    } // if
    else {
      string str = "no print" ;
      cout << "ERROR (incorrect number of arguments) : quote" << endl ;
      throw str ;
    } // else

    return temp ;
  } // Quote()

  TreeNodePtr Car( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      temp = Eval( root->right->left ) ;
      if ( temp->right == NULL && temp->left == NULL ) { // 若第一個參數型別是樹葉
        string str = "no print" ;
        cout << "ERROR (car with incorrect argument type) : " ;
        PrintSExp( temp ) ;
        throw str ;
      } // if
      else {
        temp->left->already_eval = true ;
        return temp->left ;
      } // else
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : car" << endl ;
    throw str ;
  } // Car()

  TreeNodePtr Cdr( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      temp = Eval( root->right->left ) ;
      if ( temp->right == NULL && temp->left == NULL ) { // 若第一個參數型別是樹葉
        string str = "no print" ;
        cout << "ERROR (cdr with incorrect argument type) : " ;
        PrintSExp( temp ) ;
        throw str ;
      } // if
      else {
        temp->right->already_eval = true ;
        return temp->right ;
      } // else
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : cdr" << endl ;
    throw str ;
  } // Cdr()

  TreeNodePtr IsAtom( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      TreeNodePtr ans = new TreeNode ;
      ans->right = NULL ;
      ans->left = NULL ;
      ans->already_eval = true ;
      temp = Eval( root->right->left ) ;
      if ( ATOM( temp->tokenType ) ) {
        ans->str = "#t" ;
        ans->tokenType = TRUE ;
      } // if
      else {
        ans->str = "nil" ;
        ans->tokenType = NIL ;
      } // else

      return ans ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : atom?" << endl ;
    throw str ;
  } // IsAtom()

  TreeNodePtr IsPair( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      TreeNodePtr ans = new TreeNode ;
      ans->right = NULL ;
      ans->left = NULL ;
      ans->already_eval = true ;
      temp = Eval( root->right->left ) ;
      if ( temp->right != NULL && temp->left != NULL ) { // 不是樹葉
        ans->str = "#t" ;
        ans->tokenType = TRUE ;
      } // if
      else {
        ans->str = "nil" ;
        ans->tokenType = NIL ;
      } // else

      return ans ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : pair?" << endl ;
    throw str ;
  } // IsPair()

  TreeNodePtr IsList( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      TreeNodePtr ans = new TreeNode ;
      ans->right = NULL ;
      ans->left = NULL ;
      ans->already_eval = true ;
      temp = Eval( root->right->left ) ;
      while ( temp->right != NULL ) {
        temp = temp->right ;
      } // while

      if ( temp->tokenType == NIL ) { // 最右下角是NIL
        ans->str = "#t" ;
        ans->tokenType = TRUE ;
      } // if
      else {
        ans->str = "nil" ;
        ans->tokenType = NIL ;
      } // else

      return ans ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : list?" << endl ;
    throw str ;
  } // IsList()

  TreeNodePtr IsNull( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      TreeNodePtr ans = new TreeNode ;
      ans->right = NULL ;
      ans->left = NULL ;
      ans->already_eval = true ;
      temp = Eval( root->right->left ) ;
      if ( temp->tokenType == NIL ) {
        ans->str = "#t" ;
        ans->tokenType = TRUE ;
      } // if
      else {
        ans->str = "nil" ;
        ans->tokenType = NIL ;
      } // else

      return ans ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : null?" << endl ;
    throw str ;
  } // IsNull()

  TreeNodePtr IsInteger( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      TreeNodePtr ans = new TreeNode ;
      ans->right = NULL ;
      ans->left = NULL ;
      ans->already_eval = true ;
      temp = Eval( root->right->left ) ;
      if ( temp->tokenType == INTEGER ) {
        ans->str = "#t" ;
        ans->tokenType = TRUE ;
      } // if
      else {
        ans->str = "nil" ;
        ans->tokenType = NIL ;
      } // else

      return ans ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : integer?" << endl ;
    throw str ;
  } // IsInteger()

  TreeNodePtr IsReal( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      TreeNodePtr ans = new TreeNode ;
      ans->right = NULL ;
      ans->left = NULL ;
      ans->already_eval = true ;
      temp = Eval( root->right->left ) ;
      if ( temp->tokenType == INTEGER || temp->tokenType == FLOATNUM ) {
        ans->str = "#t" ;
        ans->tokenType = TRUE ;
      } // if
      else {
        ans->str = "nil" ;
        ans->tokenType = NIL ;
      } // else

      return ans ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : real?" << endl ;
    throw str ;
  } // IsReal()

  TreeNodePtr IsNumber( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      TreeNodePtr ans = new TreeNode ;
      ans->right = NULL ;
      ans->left = NULL ;
      ans->already_eval = true ;
      temp = Eval( root->right->left ) ;
      if ( temp->tokenType == INTEGER || temp->tokenType == FLOATNUM ) {
        ans->str = "#t" ;
        ans->tokenType = TRUE ;
      } // if
      else {
        ans->str = "nil" ;
        ans->tokenType = NIL ;
      } // else

      return ans ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : number?" << endl ;
    throw str ;
  } // IsNumber()

  TreeNodePtr IsString( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      TreeNodePtr ans = new TreeNode ;
      ans->right = NULL ;
      ans->left = NULL ;
      ans->already_eval = true ;
      temp = Eval( root->right->left ) ;
      if ( temp->tokenType == STRING ) {
        ans->str = "#t" ;
        ans->tokenType = TRUE ;
      } // if
      else {
        ans->str = "nil" ;
        ans->tokenType = NIL ;
      } // else

      return ans ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : string?" << endl ;
    throw str ;
  } // IsString()

  TreeNodePtr IsBoolean( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      TreeNodePtr ans = new TreeNode ;
      ans->right = NULL ;
      ans->left = NULL ;
      ans->already_eval = true ;
      temp = Eval( root->right->left ) ;
      if ( temp->tokenType == NIL || temp->tokenType == TRUE ) {
        ans->str = "#t" ;
        ans->tokenType = TRUE ;
      } // if
      else {
        ans->str = "nil" ;
        ans->tokenType = NIL ;
      } // else

      return ans ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : boolean?" << endl ;
    throw str ;
  } // IsBoolean()

  TreeNodePtr IsSymbol( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      TreeNodePtr ans = new TreeNode ;
      ans->right = NULL ;
      ans->left = NULL ;
      ans->already_eval = true ;
      temp = Eval( root->right->left ) ;
      if ( temp->tokenType == SYMBOL ) {
        ans->str = "#t" ;
        ans->tokenType = TRUE ;
      } // if
      else {
        ans->str = "nil" ;
        ans->tokenType = NIL ;
      } // else

      return ans ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : symbol?" << endl ;
    throw str ;
  } // IsSymbol()
  /*
  void Rounding( float &num ) { // 四捨五入o
    stringstream sstream ;
    string str
    if ( num - ( int ) num != 0 ) { // 需要四捨五入o
      sstream << num ;

    } // if

  } // Rounding()
  */

  TreeNodePtr IsAdd( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    float sum = 0 ;
    bool haveFlo = false ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != INTEGER && temp->tokenType != FLOATNUM ) {
          string str = "no print" ;
          cout << "ERROR (+ with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else if ( temp->tokenType == INTEGER ) sum = sum + temp->int_num ;
        else if ( temp->tokenType == FLOATNUM ) {
          haveFlo = true ;
          sum = sum + temp->flo_num ;
        } // else if
        else ;
      } // while

      stringstream sstream ;
      sstream << sum ;
      temp = NULL ;
      temp = new TreeNode ;
      temp->right = NULL ;
      temp->left = NULL ;
      temp->already_eval = true ;
      temp->str = sstream.str() ;
      if ( sum - ( int ) sum != 0 || haveFlo ) {
        temp->flo_num = sum ;
        temp->tokenType = FLOATNUM ;
      } // if
      else {
        temp->int_num = sum ;
        temp->tokenType = INTEGER ;
      } // else

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : +" << endl ;
    throw str ;
  } // IsAdd()

  TreeNodePtr IsMinus( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    float sum = 0 ;
    bool initial = true ; // sum的值有無被動過
    bool haveFlo = false ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != INTEGER && temp->tokenType != FLOATNUM ) {
          string str = "no print" ;
          cout << "ERROR (- with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else if ( initial ) { // 給初始值
          if ( temp->tokenType == INTEGER ) sum = temp->int_num ;
          else if ( temp->tokenType == FLOATNUM ) {
            haveFlo = true ;
            sum = temp->flo_num ;
          } // else if

          initial = false ;
        } // else if
        else if ( temp->tokenType == INTEGER ) sum = sum - temp->int_num ;
        else if ( temp->tokenType == FLOATNUM ) {
          haveFlo = true ;
          sum = sum - temp->flo_num ;
        } // else if
        else ;
      } // while

      stringstream sstream ;
      sstream << sum ;
      temp = NULL ;
      temp = new TreeNode ;
      temp->right = NULL ;
      temp->left = NULL ;
      temp->already_eval = true ;
      temp->str = sstream.str() ;
      if ( sum - ( int ) sum != 0 || haveFlo ) {
        temp->flo_num = sum ;
        temp->tokenType = FLOATNUM ;
      } // if
      else {
        temp->int_num = sum ;
        temp->tokenType = INTEGER ;
      } // else

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : -" << endl ;
    throw str ;
  } // IsMinus()

  TreeNodePtr IsMultiply( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    float sum = 0 ;
    bool initial = true ; // sum的值有無被動過
    bool haveFlo = false ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != INTEGER && temp->tokenType != FLOATNUM ) {
          string str = "no print" ;
          cout << "ERROR (* with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else if ( initial ) { // 給初始值
          if ( temp->tokenType == INTEGER ) sum = temp->int_num ;
          else if ( temp->tokenType == FLOATNUM ) {
            haveFlo = true ;
            sum = temp->flo_num ;
          } // else if

          initial = false ;
        } // else if
        else if ( temp->tokenType == INTEGER ) sum = sum * temp->int_num ;
        else if ( temp->tokenType == FLOATNUM ) {
          haveFlo = true ;
          sum = sum * temp->flo_num ;
        } // else if
        else ;
      } // while

      stringstream sstream ;
      sstream << sum ;
      temp = NULL ;
      temp = new TreeNode ;
      temp->right = NULL ;
      temp->left = NULL ;
      temp->already_eval = true ;
      temp->str = sstream.str() ;
      if ( sum - ( int ) sum != 0 || haveFlo ) {
        temp->flo_num = sum ;
        temp->tokenType = FLOATNUM ;
      } // if
      else {
        temp->int_num = sum ;
        temp->tokenType = INTEGER ;
      } // else

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : *" << endl ;
    throw str ;
  } // IsMultiply()

  void FindIntDot( string &str ) {
    int pos = 0 ; // '.'在str內的位置
    for ( int j = 0 ; j < str[j] ; j++ ) { // 找到'.'在哪裡
      if ( str[j] == '.' ) {
        pos = j ;
      } // if
    } // for

    if ( pos != 0 ) {
      str = str.substr( 0, pos ) ;
    } // if

  } // FindIntDot()

  TreeNodePtr IsDivision( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    float sum = 0 ;
    bool initial = true ; // sum的值有無被動過
    bool haveFlo = false ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != INTEGER && temp->tokenType != FLOATNUM ) {
          string str = "no print" ;
          cout << "ERROR (/ with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else if ( initial ) { // 給初始值
          if ( temp->tokenType == INTEGER ) sum = temp->int_num ;
          else if ( temp->tokenType == FLOATNUM ) {
            haveFlo = true ;
            sum = temp->flo_num ;
          } // else if

          initial = false ;
        } // else if
        else if ( ( temp->tokenType == INTEGER && temp->int_num == 0 )
                  || ( temp->tokenType == FLOATNUM && temp->flo_num == 0 ) ) { // 若除數有出現0
          string str = "no print" ;
          cout << "ERROR (division by zero) : /" << endl ;
          throw str ;
        } // else if
        else if ( temp->tokenType == INTEGER ) sum = sum / temp->int_num ;
        else if ( temp->tokenType == FLOATNUM ) {
          haveFlo = true ;
          sum = sum / temp->flo_num ;
        } // else if
        else ;

      } // while

      stringstream sstream ;
      sstream << sum ;
      temp = NULL ;
      temp = new TreeNode ;
      temp->right = NULL ;
      temp->left = NULL ;
      temp->already_eval = true ;
      temp->str = sstream.str() ;
      if ( haveFlo ) {
        temp->flo_num = sum ;
        temp->tokenType = FLOATNUM ;
      } // if
      else {
        FindIntDot( temp->str ) ;
        temp->int_num = sum ;
        temp->tokenType = INTEGER ;
      } // else

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : /" << endl ;
    throw str ;
  } // IsDivision()

  TreeNodePtr Not( TreeNodePtr root ) {
    TreeNodePtr temp ;
    if ( root->right->tokenType == NIL ) ; // 沒有第一個參數
    else if ( root->right->right->tokenType != NIL ) ; // 超過一個參數了
    else {
      TreeNodePtr ans = new TreeNode ;
      ans->right = NULL ;
      ans->left = NULL ;
      ans->already_eval = true ;
      temp = Eval( root->right->left ) ;
      if ( temp->tokenType == NIL ) {
        ans->str = "#t" ;
        ans->tokenType = TRUE ;
      } // if
      else {
        ans->str = "nil" ;
        ans->tokenType = NIL ;
      } // else

      return ans ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : not" << endl ;
    throw str ;
  } // Not()

  TreeNodePtr And( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType == NIL ) {
          return temp ;
          string str = "no print" ;
          throw str ;
        } // if
        else ;
      } // while

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : and" << endl ;
    throw str ;
  } // And()

  TreeNodePtr Or( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != NIL ) {
          return temp ;
          string str = "no print" ;
          throw str ;
        } // if
        else ;
      } // while

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : or" << endl ;
    throw str ;
  } // Or()

  TreeNodePtr Bigger( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    float preNum = 0 ;
    bool initial = true ; // preNum的值有無被動過
    bool ans = true ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != INTEGER && temp->tokenType != FLOATNUM ) {
          string str = "no print" ;
          cout << "ERROR (> with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else if ( initial ) { // 給初始值
          if ( temp->tokenType == INTEGER ) preNum = temp->int_num ;
          else if ( temp->tokenType == FLOATNUM ) preNum = temp->flo_num ;

          initial = false ;
        } // else if
        else if ( ( temp->tokenType == INTEGER && preNum <= temp->int_num )
                  || ( temp->tokenType == FLOATNUM && preNum <= temp->flo_num ) ) {
          ans = false ;
        } // else if
        else {
          if ( temp->tokenType == INTEGER ) preNum = temp->int_num ;
          else preNum = temp->flo_num ;
        } // else
      } // while

      temp = NULL ;
      temp = new TreeNode ;
      temp->right = NULL ;
      temp->left = NULL ;
      temp->already_eval = true ;
      if ( ans ) {
        temp->str = "#t" ;
        temp->tokenType = TRUE ;
      } // if
      else {
        temp->str = "nil" ;
        temp->tokenType = NIL ;
      } // else

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : >" << endl ;
    throw str ;
  } // Bigger()

  TreeNodePtr BiggerEqual( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    float preNum = 0 ;
    bool initial = true ; // preNum的值有無被動過
    bool ans = true ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != INTEGER && temp->tokenType != FLOATNUM ) {
          string str = "no print" ;
          cout << "ERROR (>= with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else if ( initial ) { // 給初始值
          if ( temp->tokenType == INTEGER ) preNum = temp->int_num ;
          else if ( temp->tokenType == FLOATNUM ) preNum = temp->flo_num ;

          initial = false ;
        } // else if
        else if ( ( temp->tokenType == INTEGER && preNum < temp->int_num )
                  || ( temp->tokenType == FLOATNUM && preNum < temp->flo_num ) ) {
          ans = false ;
        } // else if
        else {
          if ( temp->tokenType == INTEGER ) preNum = temp->int_num ;
          else preNum = temp->flo_num ;
        } // else
      } // while

      temp = NULL ;
      temp = new TreeNode ;
      temp->right = NULL ;
      temp->left = NULL ;
      temp->already_eval = true ;
      if ( ans ) {
        temp->str = "#t" ;
        temp->tokenType = TRUE ;
      } // if
      else {
        temp->str = "nil" ;
        temp->tokenType = NIL ;
      } // else

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : >=" << endl ;
    throw str ;
  } // BiggerEqual()

  TreeNodePtr Smaller( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    float preNum = 0 ;
    bool initial = true ; // preNum的值有無被動過
    bool ans = true ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != INTEGER && temp->tokenType != FLOATNUM ) {
          string str = "no print" ;
          cout << "ERROR (< with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else if ( initial ) { // 給初始值
          if ( temp->tokenType == INTEGER ) preNum = temp->int_num ;
          else if ( temp->tokenType == FLOATNUM ) preNum = temp->flo_num ;

          initial = false ;
        } // else if
        else if ( ( temp->tokenType == INTEGER && preNum >= temp->int_num )
                  || ( temp->tokenType == FLOATNUM && preNum >= temp->flo_num ) ) {
          ans = false ;
        } // else if
        else {
          if ( temp->tokenType == INTEGER ) preNum = temp->int_num ;
          else preNum = temp->flo_num ;
        } // else
      } // while

      temp = NULL ;
      temp = new TreeNode ;
      temp->right = NULL ;
      temp->left = NULL ;
      temp->already_eval = true ;
      if ( ans ) {
        temp->str = "#t" ;
        temp->tokenType = TRUE ;
      } // if
      else {
        temp->str = "nil" ;
        temp->tokenType = NIL ;
      } // else

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : <" << endl ;
    throw str ;
  } // Smaller()

  TreeNodePtr SmallerEqual( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    float preNum = 0 ;
    bool initial = true ; // preNum的值有無被動過
    bool ans = true ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != INTEGER && temp->tokenType != FLOATNUM ) {
          string str = "no print" ;
          cout << "ERROR (<= with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else if ( initial ) { // 給初始值
          if ( temp->tokenType == INTEGER ) preNum = temp->int_num ;
          else if ( temp->tokenType == FLOATNUM ) preNum = temp->flo_num ;

          initial = false ;
        } // else if
        else if ( ( temp->tokenType == INTEGER && preNum > temp->int_num )
                  || ( temp->tokenType == FLOATNUM && preNum > temp->flo_num ) ) {
          ans = false ;
        } // else if
        else {
          if ( temp->tokenType == INTEGER ) preNum = temp->int_num ;
          else preNum = temp->flo_num ;
        } // else
      } // while

      temp = NULL ;
      temp = new TreeNode ;
      temp->right = NULL ;
      temp->left = NULL ;
      temp->already_eval = true ;
      if ( ans ) {
        temp->str = "#t" ;
        temp->tokenType = TRUE ;
      } // if
      else {
        temp->str = "nil" ;
        temp->tokenType = NIL ;
      } // else

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : <=" << endl ;
    throw str ;
  } // SmallerEqual()

  TreeNodePtr Equal( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    float preNum = 0 ;
    bool initial = true ; // preNum的值有無被動過
    bool ans = true ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != INTEGER && temp->tokenType != FLOATNUM ) {
          string str = "no print" ;
          cout << "ERROR (= with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else if ( initial ) { // 給初始值
          if ( temp->tokenType == INTEGER ) preNum = temp->int_num ;
          else if ( temp->tokenType == FLOATNUM ) preNum = temp->flo_num ;

          initial = false ;
        } // else if
        else if ( ( temp->tokenType == INTEGER && preNum != temp->int_num )
                  || ( temp->tokenType == FLOATNUM && preNum != temp->flo_num ) ) {
          ans = false ;
        } // else if
        else {
          if ( temp->tokenType == INTEGER ) preNum = temp->int_num ;
          else preNum = temp->flo_num ;
        } // else
      } // while

      temp = NULL ;
      temp = new TreeNode ;
      temp->right = NULL ;
      temp->left = NULL ;
      temp->already_eval = true ;
      if ( ans ) {
        temp->str = "#t" ;
        temp->tokenType = TRUE ;
      } // if
      else {
        temp->str = "nil" ;
        temp->tokenType = NIL ;
      } // else

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : =" << endl ;
    throw str ;
  } // Equal()

  TreeNodePtr StrAppend( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    string ans ;
    string strtemp ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != STRING ) {
          string str = "no print" ;
          cout << "ERROR (string-append with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else {
          strtemp = temp->str ;
          ans = ans + temp->str.substr( 1, strtemp.size() - 2  ) ; // 去掉""
        } // else
      } // while

      temp = NULL ;
      temp = new TreeNode ;
      temp->already_eval = true ;
      temp->str = "\"" + ans + "\"" ;
      temp->tokenType = STRING ;
      temp->left = NULL ;
      temp->right = NULL ;
      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : string-append" << endl ;
    throw str ;
  } // StrAppend()

  TreeNodePtr IsStrBigger( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    string preStr ;
    bool initial = true ; // preStr的值有無被動過
    bool ans = true ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != STRING ) {
          string str = "no print" ;
          cout << "ERROR (string>? with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else if ( initial ) { // 給初始值
          preStr = temp->str ;
          initial = false ;
        } // else if
        else if ( preStr.compare( temp->str ) <= 0 ) {
          ans = false ;
        } // else if
        else {
          preStr = temp->str ;
        } // else
      } // while

      temp = NULL ;
      temp = new TreeNode ;
      temp->already_eval = true ;
      temp->right = NULL ;
      temp->left = NULL ;
      if ( ans ) {
        temp->str = "#t" ;
        temp->tokenType = TRUE ;
      } // if
      else {
        temp->str = "nil" ;
        temp->tokenType = NIL ;
      } // else

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : string>?" << endl ;
    throw str ;
  } // IsStrBigger()

  TreeNodePtr IsStrSmaller( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    string preStr ;
    bool initial = true ; // preStr的值有無被動過
    bool ans = true ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != STRING ) {
          string str = "no print" ;
          cout << "ERROR (string<? with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else if ( initial ) { // 給初始值
          preStr = temp->str ;
          initial = false ;
        } // else if
        else if ( preStr.compare( temp->str ) >= 0 ) {
          ans = false ;
        } // else if
        else {
          preStr = temp->str ;
        } // else
      } // while

      temp = NULL ;
      temp = new TreeNode ;
      temp->already_eval = true ;
      temp->right = NULL ;
      temp->left = NULL ;
      if ( ans ) {
        temp->str = "#t" ;
        temp->tokenType = TRUE ;
      } // if
      else {
        temp->str = "nil" ;
        temp->tokenType = NIL ;
      } // else

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : string<?" << endl ;
    throw str ;
  } // IsStrSmaller()

  TreeNodePtr IsStrEqual( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    string preStr ;
    bool initial = true ; // preStr的值有無被動過
    bool ans = true ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 沒有第二個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right ;
        temp = Eval( walk->left ) ;
        if ( temp->tokenType != STRING ) {
          string str = "no print" ;
          cout << "ERROR (string=? with incorrect argument type) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // if
        else if ( initial ) { // 給初始值
          preStr = temp->str ;
          initial = false ;
        } // else if
        else if ( preStr.compare( temp->str ) != 0 ) {
          ans = false ;
        } // else if
        else {
          preStr = temp->str ;
        } // else
      } // while

      temp = NULL ;
      temp = new TreeNode ;
      temp->already_eval = true ;
      temp->right = NULL ;
      temp->left = NULL ;
      if ( ans ) {
        temp->str = "#t" ;
        temp->tokenType = TRUE ;
      } // if
      else {
        temp->str = "nil" ;
        temp->tokenType = NIL ;
      } // else

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : string=?" << endl ;
    throw str ;
  } // IsStrEqual()

  TreeNodePtr IsEqv( TreeNodePtr root ) {
    TreeNodePtr temp = new TreeNode ;
    temp->already_eval = true ;
    temp->right = NULL ;
    temp->left = NULL ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ;
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ;
    else if ( root->right->right->right->tokenType != NIL ) ;
    else {
      TreeNodePtr one = Eval( root->right->left ) ;
      TreeNodePtr two = Eval( root->right->right->left ) ;
      if ( one == two ) { // 樹一樣那一定一樣
        temp->str = "#t" ;
        temp->tokenType = TRUE ;
        return temp ;
      } // if
      else { // 比葉子
        if ( ( ATOM( one->tokenType ) && one->tokenType != STRING )
             && ( ATOM( two->tokenType ) && two->tokenType != STRING ) ) {
          if ( one->str == two->str ) {
            temp->str = "#t" ;
            temp->tokenType = TRUE ;
          } // if
          else {
            temp->str = "nil" ;
            temp->tokenType = NIL ;
          } // else

          return temp ;
        } // if
        else {
          temp->str = "nil" ;
          temp->tokenType = NIL ;
          return temp ;
        } // else
      } // else
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : eqv?" << endl ;
    throw str ;
  } // IsEqv()

  bool CompareTwoTree( TreeNodePtr &p1, TreeNodePtr &p2 ) {
    bool same = true ;
    if ( p1->tokenType == p2->tokenType ) { // 型別相同
      if ( p1->tokenType == STRING || p1->tokenType == SYMBOL ) {
        if ( p1->str == p2->str ) same = true ;
        else same = false ;
      } // if
      else if ( p1->tokenType == INTEGER ) {
        if ( p1->int_num == p2->int_num ) same = true ;
        else same = false ;
      } // else if
      else if ( p1->tokenType == FLOATNUM ) {
        if ( p1->flo_num == p2->flo_num ) same = true ;
        else same = false ;
      } // else if
      else if ( p1->tokenType == QUOTE ) {
        if ( p1->str == p2->str ) same = true ;
        else same = false ;
      } // else if
      else { // 對的 // LEFTPAREN || NIL || TRUE
        same = true ;
      } // else
    } // if
    else same = false ;

    if ( same ) {
      if ( p1->left == NULL && p2->left == NULL )
        return true ;
      else if ( p1->left != NULL && p2->left == NULL )
        return false ;
      else if ( p1->left == NULL && p2->left != NULL )
        return false ;
      else if ( p1->left != NULL && p2->left != NULL )
        same = CompareTwoTree( p1->left, p2->left ) ;

      if ( same ) {
        if ( p1->right == NULL && p2->right == NULL )
          return true ;
        else if ( p1->right != NULL && p2->right == NULL )
          return false ;
        else if ( p1->right == NULL && p2->right != NULL )
          return false ;
        else if ( p1->right != NULL && p2->right != NULL )
          same = CompareTwoTree( p1->right, p2->right ) ;
      } // if
      else return false ;
    } // if
    else return false ;

    return same ;
  } // CompareTwoTree()

  TreeNodePtr IsEqual( TreeNodePtr root ) {
    TreeNodePtr temp = new TreeNode ;
    temp->already_eval = true ;
    temp->right = NULL ;
    temp->left = NULL ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ;
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ;
    else if ( root->right->right->right->tokenType != NIL ) ;
    else {
      TreeNodePtr one = Eval( root->right->left ) ;
      TreeNodePtr two = Eval( root->right->right->left ) ;
      if ( one == two ) { // 樹一樣那一定一樣
        temp->str = "#t" ;
        temp->tokenType = TRUE ;
        return temp ;
      } // if
      else { // 比樹內所有內容，一樣就一樣
        bool ans = false ;
        ans = CompareTwoTree( one, two ) ;
        if ( ans ) {
          temp->str = "#t" ;
          temp->tokenType = TRUE ;
        } // if
        else {
          temp->str = "nil" ;
          temp->tokenType = NIL ;
        } // else

        return temp ;
      } // else
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : equal?" << endl ;
    throw str ;
  } // IsEqual()

  TreeNodePtr Begin( TreeNodePtr root ) {
    TreeNodePtr temp = NULL ;
    TreeNodePtr walk = root ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 沒有第一個參數
    else {
      while ( walk->right->tokenType != NIL && walk->right != NULL ) {
        walk = walk->right;
        temp = Eval( walk->left );
      } // while

      return temp ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : begin" << endl ;
    throw str ;
  } // Begin()

  TreeNodePtr If( TreeNodePtr root ) {
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ;
    else if ( root->right->right == NULL ) ;
    else if ( root->right->right->left == NULL ) ; // 第二個參數(至少要兩個，最多三)
    else {
      TreeNodePtr one = Eval( root->right->left ) ;
      if ( root->right->right->right != NULL
           && root->right->right->right->tokenType != NIL ) { // 未必有第三個參數(若有)
        if ( root->right->right->right->right != NULL
             && root->right->right->right->right->tokenType != NIL ) { // 若有第四個參數
          string str = "no print" ;
          cout << "ERROR (incorrect number of arguments) : if" << endl ;
          throw str ;
        } // if

        if ( one->tokenType != NIL ) {
          TreeNodePtr two = Eval( root->right->right->left ) ;
          return two ;
        } // if
        else {
          TreeNodePtr three = Eval( root->right->right->right->left ) ;
          return three ;
        } // else
      } // if
      else { // 若只有兩個參數
        if ( one->tokenType != NIL ) {
          TreeNodePtr two = Eval( root->right->right->left ) ;
          return two ;
        } // if
        else {
          string str = "no print" ;
          cout << "ERROR (no return value) : " ;
          PrintSExp( root ) ;
          throw str ;
        } // else
      } // else
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : if" << endl ;
    throw str ;
  } // If()

  TreeNodePtr Cond( TreeNodePtr root ) {
    TreeNodePtr temp = root ;
    TreeNodePtr preCheck = root ;
    if ( root->right == NULL ) ;
    else if ( root->right->left == NULL ) ; // 第一個參數(至少要一個)
    else {
      while ( preCheck->right != NULL && preCheck->right->tokenType != NIL ) { // 先檢查cond所有參數數量
        preCheck = preCheck->right ;
        TreeNodePtr walk = preCheck->left ;
        int count = 0 ;
        while ( walk->right != NULL ) {
          walk = walk->right ;
          count++ ;
        } // while

        if ( walk->tokenType == NIL && count >= 2 ) ;
        else {
          string str = "no print" ;
          cout << "ERROR (COND format) : " ;
          PrintSExp( root ) ;
          throw str ;
        } // else
      } // while

      while ( temp->right != NULL && temp->right->tokenType != NIL ) {
        temp = temp->right ;
        TreeNodePtr check = temp->left ;
        int countArg = 0 ; // (第一個)參數內含的參數量要大於等於2
        while ( check->right != NULL ) { // 看(第一個)參數最右下角是否為NIL
          check = check->right ;
          countArg++ ;
        } // while

        if ( check->tokenType == NIL && countArg >= 2 ) { // 檢查完成(第一個)參數
          TreeNodePtr temp2 = temp->left ; // 指向(第一個)參數
          if ( temp->right->tokenType != NIL ) { // 若現在不是最後一個cond參數
            TreeNodePtr condition = Eval( temp2->left ) ;
            if ( condition->tokenType != NIL ) { // 若此條件是正確的
              TreeNodePtr check2 = temp2->right ;
              while ( check2->right->tokenType != NIL ) { // 停在最後一個參數
                temp = Eval( check2->left ) ;
                check2 = check2->right ;
              } // while

              temp = Eval( check2->left ) ;
              return temp ;
            } // if

          } // if
          else { // 現在是最後一個cond參數
            if ( temp2->left->str == "else" ) { // 若是有意義的else(直接回傳答案)
              TreeNodePtr check2 = temp2 ;
              check2 = check2->right ;
              while ( check2->right->tokenType != NIL ) { // 停在最後一個參數
                temp = Eval( check2->left ) ;
                check2 = check2->right ;
              } // while

              temp = Eval( check2->left ) ;
              return temp ;
            } // if
            else { // 不是else(若條件不是NIL才回傳答案)
              TreeNodePtr condition1 = Eval( temp2->left ) ;
              if ( condition1->tokenType != NIL ) { // 若此條件是正確的
                TreeNodePtr check2 = temp2 ;
                while ( check2->right->tokenType != NIL ) { // 停在最後一個參數
                  temp = Eval( check2->left ) ;
                  check2 = check2->right ;
                } // while

                temp = Eval( check2->left ) ;
                return temp ;
              } // if
              else { // 已經最後了還沒有答案
                string str = "no print" ;
                cout << "ERROR (no return value) : " ;
                PrintSExp( root ) ;
                throw str ;
              } // else
            } // else
          } // else
        } // if
        else {
          string str = "no print" ;
          cout << "ERROR (non-list) : " ;
          PrintSExp( root ) ;
          throw str ;
        } // else

      } // while

    } // else

    string str = "no print" ;
    cout << "ERROR (COND format) : " ;
    PrintSExp( root ) ;
    throw str ;
  } // Cond()

  TreeNodePtr CleanEnvironment( TreeNodePtr root ) {
    if ( root->right != NULL && root->right->tokenType != NIL ) ;
    else {
      gMap.clear() ;
      string str = "no print" ;
      cout << "environment cleaned" << endl ;
      throw str ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : clean-environment" << endl ;
    throw str ;
  } // CleanEnvironment()

  TreeNodePtr Exit( TreeNodePtr root ) {
    if ( root->right != NULL && root->right->tokenType != NIL ) ;
    else {
      string str = "exit" ;
      throw str ;
    } // else

    string str = "no print" ;
    cout << "ERROR (incorrect number of arguments) : exit" << endl ;
    throw str ;
  } // Exit()

  TreeNodePtr DoInternalFun( string name, TreeNodePtr temp ) {
    if ( name == "cons" ) return Cons( temp ) ;
    else if ( name == "list" ) return List( temp ) ;
    else if ( name == "define" ) return Define( temp ) ;
    else if ( name == "quote" || name == "'" ) return Quote( temp ) ;
    else if ( name == "car" ) return Car( temp ) ;
    else if ( name == "cdr" ) return Cdr( temp ) ;
    else if ( name == "atom?" ) return IsAtom( temp ) ;
    else if ( name == "pair?" ) return IsPair( temp ) ;
    else if ( name == "list?" ) return IsList( temp ) ;
    else if ( name == "null?" ) return IsNull( temp ) ;
    else if ( name == "integer?" ) return IsInteger( temp ) ;
    else if ( name == "real?" ) return IsReal( temp ) ;
    else if ( name == "number?" ) return IsNumber( temp ) ;
    else if ( name == "string?" ) return IsString( temp ) ;
    else if ( name == "boolean?" ) return IsBoolean( temp ) ;
    else if ( name == "symbol?" ) return IsSymbol( temp ) ;
    else if ( name == "+" ) return IsAdd( temp ) ;
    else if ( name == "-" ) return IsMinus( temp ) ;
    else if ( name == "*" ) return IsMultiply( temp ) ;
    else if ( name == "/" ) return IsDivision( temp ) ;
    else if ( name == "not" ) return Not( temp ) ;
    else if ( name == "and" ) return And( temp ) ;
    else if ( name == "or" ) return Or( temp ) ;
    else if ( name == ">" ) return Bigger( temp ) ;
    else if ( name == ">=" ) return BiggerEqual( temp ) ;
    else if ( name == "<" ) return Smaller( temp ) ;
    else if ( name == "<=" ) return SmallerEqual( temp ) ;
    else if ( name == "=" ) return Equal( temp ) ;
    else if ( name == "string-append" ) return StrAppend( temp ) ;
    else if ( name == "string>?" ) return IsStrBigger( temp ) ;
    else if ( name == "string<?" ) return IsStrSmaller( temp ) ;
    else if ( name == "string=?" ) return IsStrEqual( temp ) ;
    else if ( name == "eqv?" ) return IsEqv( temp ) ;
    else if ( name == "equal?" ) return IsEqual( temp ) ;
    else if ( name == "begin" ) return Begin( temp ) ;
    else if ( name == "if" ) return If( temp ) ;
    else if ( name == "cond" ) return Cond( temp ) ;
    else if ( name == "clean-environment" ) return CleanEnvironment( temp ) ;
    else if ( name == "exit" ) return Exit( temp ) ;
    else return NULL ;
  } // DoInternalFun()

  TreeNodePtr Eval( TreeNodePtr root ) {
    gCounterLevel++ ;
    map<string, TreeNodePtr>::iterator it; // 在變數表中查詢是否有指定節點時使用
    if ( ( root->left == NULL && root->right == NULL )
         && root->tokenType != SYMBOL ) {
      TreeNodePtr temp = new TreeNode ;
      temp->str = root->str ;
      temp->int_num = root->int_num ;
      temp->flo_num = root->flo_num ;
      temp->already_eval = true ;
      temp->tokenType = root->tokenType ;
      temp->left = NULL ;
      temp->right = NULL ;
      return temp ;
    } // if
    else if ( ( root->left == NULL && root->right == NULL )
              && root->tokenType == SYMBOL ) {
      it = gMap.find( root->str ) ;
      if ( IsInterFun( root->str ) ) {
        TreeNodePtr temp = new TreeNode ;
        temp->str = root->str ;
        temp->already_eval = true ;
        temp->tokenType = root->tokenType ;
        temp->left = NULL ;
        temp->right = NULL ;
        return temp ;
      } // if
      else if ( it != gMap.end() ) { // 若有找到
        return it->second ;
      } // else if
      else {
        string str ;
        str = "no print" ;
        cout << "ERROR (unbound symbol) : " + root->str << endl ;
        throw str ;
      } // else
    } // else if
    else {
      TreeNodePtr temp = root ;
      while ( temp->right != NULL )
        temp = temp->right ;

      if ( temp->tokenType != NIL ) {
        string str ;
        str = "no print" ;
        cout << "ERROR (non-list) : " ;
        PrintSExp( root ) ; // (...)要pretty print
        throw str ;
      } // if
      else if ( ( root->left->left == NULL && root->left->right == NULL )
                && root->left->tokenType != SYMBOL && root->left->tokenType != QUOTE ) {
        string str = "no print" ;
        cout << "ERROR (attempt to apply non-function) : " + root->left->str << endl ;
        throw str ;
      } // else if
      else if ( ( root->left->left == NULL && root->left->right == NULL )
                && root->left->tokenType == SYMBOL ) {
        it = gMap.find( root->left->str ) ;
        if ( ( it != gMap.end() && IsInterFun( it->second->str ) )
             || ( it == gMap.end() && IsInterFun( root->left->str ) ) ) {
          string temp ;
          if ( it != gMap.end() ) {
            temp = it->second->str ;
          } // if
          else {
            temp = root->left->str ;
          } // else

          if ( gCounterLevel != 1 &&
               ( temp == "clean-environment" || temp == "define" || temp == "exit" ) ) {
            if ( temp == "clean-environment" ) {
              string str = "no print" ;
              cout << "ERROR (level of CLEAN-ENVIRONMENT)" << endl ;
              throw str ;
            } // if
            else if ( temp == "define" ) {
              string str = "no print" ;
              cout << "ERROR (level of DEFINE)" << endl ;
              throw str ;
            } // else if
            else {
              string str = "no print" ;
              cout << "ERROR (level of EXIT)" << endl ;
              throw str ;
            } // else

          } // if
          else {
            return DoInternalFun( temp, root ) ;
          } // else

        } // if
        else {
          string str = "no print" ;
          if ( it == gMap.end() ) {
            cout << "ERROR (unbound symbol) : " + root->left->str << endl ;
          } // if
          else {
            cout << "ERROR (attempt to apply non-function) : " ;
            PrintSExp( it->second ) ;
          } // else

          throw str ;
        } // else
      } // else if
      else {
        TreeNodePtr temp = Eval( root->left ) ;
        if ( ( temp->left == NULL && temp->right == NULL )
             && IsInterFun( temp->str ) ) {
          return DoInternalFun( temp->str, root ) ;
        } // if
        else {
          string str = "no print" ;
          cout << "ERROR (attempt to apply non-function) : " ;
          PrintSExp( temp ) ;
          throw str ;
        } // else

      } // else

    } // else

  } // Eval()

  void Check() {
    for ( int i = 0 ; i < gAllToken.size() ; i++ ) {
      cout << gAllToken[i].str << "  " << gAllToken[i].type << endl ;
    } // for
  } // Check()
};

int main() {
  Tree tree = Tree();
  int getTestNum = 0; // 測試數據編號
  bool end_of_file = false;
  bool exit = false ;
  int errorEnd = -1; // 0 : no more input
  int whiteSpaceNum = 0 ; // token到下一個token之間有多少空白
  bool first_run = true ;
  TreeNodePtr root = NULL ; // 樹
  scanf( "%d\n", &getTestNum );
  printf( "Welcome to OurScheme!\n\n" );

  while ( !exit && !end_of_file && errorEnd != 0 ) {
    errorEnd = -1 ;
    printf( "> " ) ;
    root = tree.ReadSExp( end_of_file, errorEnd, whiteSpaceNum, first_run ); // 輸出error
    first_run = false ;
    if ( root != NULL && !end_of_file ) { // no error
      gCounterLevel = 0 ;
      try {
        TreeNodePtr temp = tree.Eval( root ) ;
        tree.PrintSExp( temp ); // You must "pretty print" this data structure.
      } // try
      catch ( string str ) {
        if ( str == "no print" ) ;
        else if ( str == "exit" )
          exit = true ;
        else {
          cout << "ERROR!!!!" << endl ;
        } // else
      } // catch
    } // if

    if ( !exit && !end_of_file && errorEnd != 1 ) {
      cout << endl ;
    } // if

    delete root ;
    root = NULL ;

    if ( !exit && !end_of_file ) {
      whiteSpaceNum = 0 ;
      if ( errorEnd == -1 ) { // no error
        tree.InitialToGetWhiteSpace( whiteSpaceNum ) ;
      } // if
      else { // has error
        tree.DeleteERRORCurrentLine() ;
      } // else
    } // if

    // gCounterLevel = 0 ;
    gAllToken.clear() ;
    gStandardSexp.clear() ;
  } // while

  printf( "\nThanks for using OurScheme!" );
  return 0;
} // main()
