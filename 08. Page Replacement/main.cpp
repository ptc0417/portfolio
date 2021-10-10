#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring> // memset
#include <vector>

using namespace std;

struct pageDetail {
  char ref = ' ' ;
  vector <char> frame ;
  bool fault = false ;
};

struct pageCurrent {
  char ref = ' ' ;
  int count = 0 ;
};

class Method {
private:
  int page_frames ;       // 1-9
  string page_reference ; // 0-9
  int page_faults ;       // 記錄頁錯誤次數
  int page_replaces ;     // 記錄頁置換次數
  int AllFaults[6] ;
  int AllReplaces[6] ;
  vector <pageDetail> result ; // 記錄各reference資訊
  vector <vector <pageDetail>> AllResult ; // 記錄各方法的答案
  vector <char> currentFrame ; // 紀錄目前的frame資訊
  vector <pageCurrent> counter ;
  pageDetail tempBlock ;
  pageCurrent tempCounter ;
  string methodName[6] = { "FIFO", "LRU", "Least Frequently Used Page Replacement",
                           "Most Frequently Used Page Replacement ",
                           "Least Frequently Used LRU Page Replacement",
                           "Most Frequently Used LRU Page Replacement " };

  int PageInCurrentFrame( char ch ) { // ch->page
    for ( int i = 0 ; i < currentFrame.size() ; i++ ) {
      if ( ch == currentFrame[i] ) {
        return i ;
      } // if
    } // for
    return -1 ;
  } // PageInCurrentFrame()

  int FindMinCountInCurrent() {
    int min_index = 0 ;
    int min = counter[0].count ;
    for ( int i = 0 ; i < counter.size() ; i++ ) {
      if ( min > counter[i].count ) {
        min = counter[i].count ;
        min_index = i ;
      } // if
    } // for

    return min_index ;
  } // FindMinCountInCurrent()

  int FindMaxCountInCurrent() {
    int max_index = 0 ;
    int max = counter[0].count ;
    for ( int i = 0 ; i < counter.size() ; i++ ) {
      if ( max < counter[i].count ) {
        max = counter[i].count ;
        max_index = i ;
      } // if
    } // for

    return max_index ;
  } // FindMaxCountInCurrent()

public:
  Method() {
    page_frames = 0 ;     // input
    page_reference = "" ; // input
    page_faults = 0 ;
    page_replaces = 0 ;
    vector <pageDetail>().swap( result ) ;
    vector <char>().swap( currentFrame ) ;
  } // Method()

  bool ReadFile( string filename ) {
    fstream file ;
    string temp ;
    filename = filename + ".txt" ;
    file.open( filename ) ;
    if ( !file.is_open() ) {
      cout << filename << " does not exist!" << endl << endl ;
      return false ;
    } // if
    else { // file is open
      getline( file, temp, '\n' ) ; // 讀取第一行(page_reference)
      page_frames = stoi(temp) ;
      getline( file, temp, '\n' ) ; // 讀取page reference串
      page_reference = temp ;
      file.close() ; // close file
      return true ;
    } // else
  } // read file

  void WriteFile( string filename ) {
    FILE * fout ;
    filename = "out_" + filename + ".txt" ;
    fout = fopen( filename.c_str(), "w" ) ;
    if ( fout == nullptr ){
      cout << "Fail to open file" << filename << endl ;
      return ;
    } // if
    for ( int k = 0 ; k < AllResult.size() ; k++ ) {
      // 輸出排程方法名稱
      fprintf( fout, "--------------%s-----------------------\n", methodName[k].c_str() ) ;
      for ( int i = 0 ; i < AllResult[k].size() ; i++ ) {
        fprintf( fout, "%c\t", AllResult[k][i].ref ) ;
        for ( int j = AllResult[k][i].frame.size()-1 ; j >= 0 ; j-- ) {
          fprintf( fout, "%c", AllResult[k][i].frame[j] ) ;
        } // for
        if ( AllResult[k][i].fault ) {
          fprintf( fout, "\tF\n" ) ;
        } // if
        else {
          fprintf( fout, "\n" ) ;
        } // else
      } // for
      fprintf( fout, "Page Fault = %d  Page Replaces = %d  Page Frames = %d\n",
               AllFaults[k], AllReplaces[k], page_frames ) ; // 輸出排程總結
      if ( k+1 != AllResult.size() && k != 3 ) { // 若目前不是最後一個方法
        fprintf( fout, "\n" ) ;
      } // if
    } // for

    fclose(fout) ;
  } // write file

  void FIFO() { // 先進先出置換法
    for ( int i = 0 ; i < ( page_reference.size()-1 ) ; i++ ) {
      tempBlock.ref = page_reference[i] ;
      // 檢查有無目前page，若有 =>只存結果;
      // 若無 =>檢查frame大小(滿了->置換;未滿->加入)=>發生page fault
      if ( PageInCurrentFrame( page_reference[i] ) != -1 ) { // page在目前frame中
        tempBlock.frame = currentFrame ;
        tempBlock.fault = false ;
      } // if
      else { // 若不在
        page_faults++ ;
        if ( currentFrame.size() < page_frames ) { // 未滿->加入
          currentFrame.push_back( page_reference[i] ) ;
          tempBlock.frame = currentFrame ;
          tempBlock.fault = true ;
        } // if
        else { // 已達page_frames的大小上限(滿了)
          page_replaces++ ; // 置換
          currentFrame.erase( currentFrame.begin() ) ; // 移出第一個(最後)
          currentFrame.push_back( page_reference[i] ) ;
          tempBlock.frame = currentFrame ;
          tempBlock.fault = true ;
        } // else
      } // else

      result.push_back( tempBlock ) ;
    } // for

    AllResult.push_back( result ) ;
    AllFaults[0] = page_faults ;
    AllReplaces[0] = page_replaces ;
    initial() ;
  } // FIFO()

  void LRU() { // 最近罕用頁置換法
    for ( int i = 0 ; i < ( page_reference.size()-1 ) ; i++ ) {
      tempBlock.ref = page_reference[i] ;
      // 檢查有無目前page，若有 =>只存結果;
      // 若無 =>檢查frame大小(滿了->置換;未滿->加入)=>發生page fault
      int index = PageInCurrentFrame( page_reference[i] ) ;
      if ( index != -1 ) { // page在目前frame中
        // 此頁時間標記更新
        currentFrame.erase( currentFrame.begin()+index ) ;
        currentFrame.push_back( page_reference[i] ) ;
        tempBlock.frame = currentFrame ;
        tempBlock.fault = false ;
      } // if
      else { // 若不在
        page_faults++ ;
        if ( currentFrame.size() < page_frames ) { // 未滿->加入
          currentFrame.push_back( page_reference[i] ) ;
          tempBlock.frame = currentFrame ;
          tempBlock.fault = true ;
        } // if
        else { // 已達page_frames的大小上限(滿了)
          page_replaces++ ; // 置換
          currentFrame.erase( currentFrame.begin() ) ; // 移出第一個(最後)
          currentFrame.push_back( page_reference[i] ) ;
          tempBlock.frame = currentFrame ;
          tempBlock.fault = true ;
        } // else
      } // else

      result.push_back( tempBlock ) ;
    } // for

    AllResult.push_back( result ) ;
    AllFaults[1] = page_faults ;
    AllReplaces[1] = page_replaces ;
    initial() ;
  } // LRU()

  void LFU_FIFO() {
    for ( int i = 0 ; i < ( page_reference.size()-1 ) ; i++ ) {
      tempBlock.ref = page_reference[i] ;
      int index = PageInCurrentFrame( page_reference[i] ) ;
      // 檢查有無目前page，若有 =>只存結果;
      // 若無 =>檢查frame大小(滿了->置換;未滿->加入)=>發生page fault
      if ( index != -1 ) { // page在目前frame中
        tempBlock.frame = currentFrame ;
        tempBlock.fault = false ;
        counter[index].count++ ; // 設定count
      } // if
      else { // 若不在
        page_faults++ ;
        if ( currentFrame.size() < page_frames ) { // 未滿->加入
          currentFrame.push_back( page_reference[i] ) ;
          tempBlock.frame = currentFrame ;
          tempBlock.fault = true ;
          // 設定count
          tempCounter.ref = page_reference[i] ;
          tempCounter.count = 1 ;
          counter.push_back( tempCounter ) ;
        } // if
        else { // 已達page_frames的大小上限(滿了)
          page_replaces++ ; // 置換
          currentFrame.erase( currentFrame.begin() + FindMinCountInCurrent() ) ; // 移出counter最小的
          currentFrame.push_back( page_reference[i] ) ;
          tempBlock.frame = currentFrame ;
          tempBlock.fault = true ;
          // 設定count
          counter.erase( counter.begin() + FindMinCountInCurrent() ) ;
          tempCounter.ref = page_reference[i] ;
          tempCounter.count = 1 ;
          counter.push_back( tempCounter ) ;
        } // else
      } // else

      result.push_back( tempBlock ) ;
    } // for

    AllResult.push_back( result ) ;
    AllFaults[2] = page_faults ;
    AllReplaces[2] = page_replaces ;
    initial() ;
  } // LFU_FIFO()

  void MFU_FIFO() {
    for ( int i = 0 ; i < ( page_reference.size()-1 ) ; i++ ) {
      tempBlock.ref = page_reference[i] ;
      int index = PageInCurrentFrame( page_reference[i] ) ;
      // 檢查有無目前page，若有 =>只存結果;
      // 若無 =>檢查frame大小(滿了->置換;未滿->加入)=>發生page fault
      if ( index != -1 ) { // page在目前frame中
        tempBlock.frame = currentFrame ;
        tempBlock.fault = false ;
        counter[index].count++ ; // 設定count
      } // if
      else { // 若不在
        page_faults++ ;
        if ( currentFrame.size() < page_frames ) { // 未滿->加入
          currentFrame.push_back( page_reference[i] ) ;
          tempBlock.frame = currentFrame ;
          tempBlock.fault = true ;
          // 設定count
          tempCounter.ref = page_reference[i] ;
          tempCounter.count = 1 ;
          counter.push_back( tempCounter ) ;
        } // if
        else { // 已達page_frames的大小上限(滿了)
          page_replaces++ ; // 置換
          currentFrame.erase( currentFrame.begin() + FindMaxCountInCurrent() ) ; // 移出counter最小的
          currentFrame.push_back( page_reference[i] ) ;
          tempBlock.frame = currentFrame ;
          tempBlock.fault = true ;
          // 設定count
          counter.erase( counter.begin() + FindMaxCountInCurrent() ) ;
          tempCounter.ref = page_reference[i] ;
          tempCounter.count = 1 ;
          counter.push_back( tempCounter ) ;
        } // else
      } // else

      result.push_back( tempBlock ) ;
    } // for

    AllResult.push_back( result ) ;
    AllFaults[3] = page_faults ;
    AllReplaces[3] = page_replaces ;
    initial() ;
  } // MFU_FIFO()

  void LFU_LRU() {
    for ( int i = 0 ; i < ( page_reference.size()-1 ) ; i++ ) {
      tempBlock.ref = page_reference[i] ;
      // 檢查有無目前page，若有 =>只存結果;
      // 若無 =>檢查frame大小(滿了->置換;未滿->加入)=>發生page fault
      int index = PageInCurrentFrame( page_reference[i] ) ;
      if ( index != -1 ) { // page在目前frame中
        // 此頁時間標記更新
        currentFrame.erase( currentFrame.begin()+index ) ;
        currentFrame.push_back( page_reference[i] ) ;
        tempBlock.frame = currentFrame ;
        tempBlock.fault = false ;
        // 設定count
        tempCounter.count = counter[index].count + 1 ;
        counter.erase( counter.begin()+index ) ;
        tempCounter.ref = page_reference[i] ;
        counter.push_back( tempCounter ) ;
      } // if
      else { // 若不在
        page_faults++ ;
        if ( currentFrame.size() < page_frames ) { // 未滿->加入
          currentFrame.push_back( page_reference[i] ) ;
          tempBlock.frame = currentFrame ;
          tempBlock.fault = true ;
          // 設定count
          tempCounter.ref = page_reference[i] ;
          tempCounter.count = 1 ;
          counter.push_back( tempCounter ) ;
        } // if
        else { // 已達page_frames的大小上限(滿了)
          page_replaces++ ; // 置換
          currentFrame.erase( currentFrame.begin() + FindMinCountInCurrent() ) ; // 移出count最小的
          currentFrame.push_back( page_reference[i] ) ;
          tempBlock.frame = currentFrame ;
          tempBlock.fault = true ;
          // 設定count
          counter.erase( counter.begin() + FindMinCountInCurrent() ) ;
          tempCounter.ref = page_reference[i] ;
          tempCounter.count = 1 ;
          counter.push_back( tempCounter ) ;
        } // else
      } // else

      result.push_back( tempBlock ) ;
    } // for

    AllResult.push_back( result ) ;
    AllFaults[4] = page_faults ;
    AllReplaces[4] = page_replaces ;
    initial() ;
  } // LFU_LRU()

  void MFU_LRU() {
    for ( int i = 0 ; i < ( page_reference.size()-1 ) ; i++ ) {
      tempBlock.ref = page_reference[i] ;
      // 檢查有無目前page，若有 =>只存結果;
      // 若無 =>檢查frame大小(滿了->置換;未滿->加入)=>發生page fault
      int index = PageInCurrentFrame( page_reference[i] ) ;
      if ( index != -1 ) { // page在目前frame中
        // 此頁時間標記更新
        currentFrame.erase( currentFrame.begin()+index ) ;
        currentFrame.push_back( page_reference[i] ) ;
        tempBlock.frame = currentFrame ;
        tempBlock.fault = false ;
        // 設定count
        tempCounter.count = counter[index].count + 1 ;
        counter.erase( counter.begin()+index ) ;
        tempCounter.ref = page_reference[i] ;
        counter.push_back( tempCounter ) ;
      } // if
      else { // 若不在
        page_faults++ ;
        if ( currentFrame.size() < page_frames ) { // 未滿->加入
          currentFrame.push_back( page_reference[i] ) ;
          tempBlock.frame = currentFrame ;
          tempBlock.fault = true ;
          // 設定count
          tempCounter.ref = page_reference[i] ;
          tempCounter.count = 1 ;
          counter.push_back( tempCounter ) ;
        } // if
        else { // 已達page_frames的大小上限(滿了)
          page_replaces++ ; // 置換
          currentFrame.erase( currentFrame.begin() + FindMaxCountInCurrent() ) ; // 移出count最小的
          currentFrame.push_back( page_reference[i] ) ;
          tempBlock.frame = currentFrame ;
          tempBlock.fault = true ;
          // 設定count
          counter.erase( counter.begin() + FindMaxCountInCurrent() ) ;
          tempCounter.ref = page_reference[i] ;
          tempCounter.count = 1 ;
          counter.push_back( tempCounter ) ;
        } // else
      } // else

      result.push_back( tempBlock ) ;
    } // for

    AllResult.push_back( result ) ;
    AllFaults[5] = page_faults ;
    AllReplaces[5] = page_replaces ;
    initial() ;
  } // MFU_LRU()

  void initial() {
    page_faults = 0 ;
    page_replaces = 0 ;
    vector <pageDetail>().swap( result ) ;
    vector <char>().swap( currentFrame ) ;
    vector <pageCurrent>().swap( counter ) ;
  } // initial()

  void Initialization() {
    page_frames = 0 ;     // input
    page_reference = "" ; // input
    vector <vector<pageDetail>>().swap(AllResult) ;
    memset( AllFaults, 0, sizeof(AllFaults) ) ; // 清空char陣列
    memset( AllReplaces, 0, sizeof(AllReplaces) ) ;
  } // Initialization()
};

void UserInformation(){
  cout << "***** OS HW3 Page Replacement *****" << endl ;
  cout << "** 0. QUIT                     **" << endl ;
  cout << "** 1. START                    **" << endl ;
  cout << "*********************************" << endl ;
  cout << "Input a command(0, 1): " ;
}  // 使用者介面

int main() {
  string cmd ;
  string filename ;
  Method method = Method() ;

  UserInformation() ;
  cin >> cmd ;
  while ( cmd != "0" ) {
    if( cmd == "1" ){
      cout << "Please input filename : " ;
      cin >> filename ;
      while ( !method.ReadFile( filename ) ) {
        cout << "Please input filename : " ;
        cin >> filename ;
      } // while

      method.FIFO() ;
      method.LRU() ;
      method.LFU_FIFO() ;
      method.MFU_FIFO() ;
      method.LFU_LRU() ;
      method.MFU_LRU() ;
      method.WriteFile( filename ) ;

    } // if
    else {
      cout << endl << "Command does not exist!" << endl ;
    } // else

    method.Initialization() ;
    UserInformation() ;
    cin >> cmd ;
  } // while

  return 0;
}
