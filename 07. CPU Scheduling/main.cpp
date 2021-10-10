#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct fileContext{
    int ID ;
    int CPU_Burst ;
    int Arrival_Time ;
    int Priority ;
    int TurnAroundTime ; // turnaround time = done time - arrival time
    int WaitingTime ;    // waiting time = turnaround time - CPU Burst
};

struct IDInformation{
    int ID ; // process ID
    float remain_CPUBurst ; // 剩餘的CPU Burst
    int Priority ;
    float ResponseRatio ; // = (waiting time + CPU Burst) / CPU Burst =>變動的優先等級(HRRN)
    int indexInData ; // 在m_data內的index
};

class Method {
private:
    int Num[4] ; // 暫存str內(四個)數字
    int currentTime = 0 ; // 目前時間
    string methodName[5] = { "FCFS", "  RR", "SRTF", "PPRR", "HRRN" };
    vector <fileContext> m_data ; // 資料們
    vector <char> m_gantt ; // 甘特圖
    vector <IDInformation> m_readyVector ; // 排隊等待運作的process們
    vector <vector <char>> m_AllGantt ; // 各個method的甘特圖
    vector <vector <fileContext>> m_AllData ; // 各個method資料結果們

    bool IsWhiteSpace( char ch ){
        if( ch == ' ' || ch == '\t' || ch == '\r' ){
            return true ;
        } // if
        return false ;
    } // 是否為空白或tab

    void Cut( string str ) { // 取出string內數字
        int index = 0 ; // Num的index
        string temp ;
        // 取出str內(四個)數字
        for( int i = 0 ; i < str.size() ; i++ ){
            if ( !IsWhiteSpace(str[i]) ){ // 遇到數字時
                temp = temp + str[i] ;
            } // if
            else{ // 遇到空白時
                if (!temp.empty()){ // 有數字於暫存區
                    Num[index] = stoi(temp) ; // 存入數字
                    index++ ;
                    temp = "" ;
                } // if
            } // else
        } // for
        if (!temp.empty()){ // 若第四個數字後即為'\n'(有數字於暫存區)
            Num[index] = stoi(temp) ; // 存入數字
        } // if
    } // cut a string to numbers

    void SaveOneBlock( string str ){ // (一個block的資料)並儲存於vector中
        fileContext block ;
        Cut(str) ;
        // 儲存一個block
        block.ID = Num[0] ;
        block.CPU_Burst = Num[1] ;
        block.Arrival_Time = Num[2] ;
        block.Priority = Num[3] ;
        m_data.push_back(block) ;
    } // Save a block in vector m_data

    void SaveNewProcess(int i ){
        IDInformation process ;
        process.ID = m_data[i].ID ;
        process.indexInData = i ;
        process.remain_CPUBurst = m_data[i].CPU_Burst ;
        process.Priority = m_data[i].Priority ;
        process.ResponseRatio = 1.0 ;
        m_readyVector.push_back(process ) ;
    } // push a new process into ready vector

    static bool CompareByArrivalTime( const fileContext &a, const fileContext &b ){
        return a.Arrival_Time < b.Arrival_Time ;
    } // compare by arrival time

    static bool CompareByID( const fileContext &a, const fileContext &b ){
        return a.ID < b.ID ;
    } // compare by process id

    static bool CompareByCPUBurst( const IDInformation &a, const IDInformation &b ){
        return a.remain_CPUBurst < b.remain_CPUBurst ;
    } // compare by CPU burst

    static bool CompareByPriority( const IDInformation &a, const IDInformation &b ){
        return a.Priority < b.Priority ;
    } // compare by Priority

    static bool CompareByResponseRatio( const IDInformation &a, const IDInformation &b ){ // compare by Response Ratio
        return a.ResponseRatio > b.ResponseRatio ; // 大的優先處理(HRRN)
    } // compare by CPU burst

    char ConvertIntToChar( int num ){ // 0-9,A-Z(10-35)
        if ( num == 0 ) return '0' ;
        else if ( num == 1 ) return '1' ;
        else if ( num == 2 ) return '2' ;
        else if ( num == 3 ) return '3' ;
        else if ( num == 4 ) return '4' ;
        else if ( num == 5 ) return '5' ;
        else if ( num == 6 ) return '6' ;
        else if ( num == 7 ) return '7' ;
        else if ( num == 8 ) return '8' ;
        else if ( num == 9 ) return '9' ;
        else if ( num == 10 ) return 'A' ;
        else if ( num == 11 ) return 'B' ;
        else if ( num == 12 ) return 'C' ;
        else if ( num == 13 ) return 'D' ;
        else if ( num == 14 ) return 'E' ;
        else if ( num == 15 ) return 'F' ;
        else if ( num == 16 ) return 'G' ;
        else if ( num == 17 ) return 'H' ;
        else if ( num == 18 ) return 'I' ;
        else if ( num == 19 ) return 'J' ;
        else if ( num == 20 ) return 'K' ;
        else if ( num == 21 ) return 'L' ;
        else if ( num == 22 ) return 'M' ;
        else if ( num == 23 ) return 'N' ;
        else if ( num == 24 ) return 'O' ;
        else if ( num == 25 ) return 'P' ;
        else if ( num == 26 ) return 'Q' ;
        else if ( num == 27 ) return 'R' ;
        else if ( num == 28 ) return 'S' ;
        else if ( num == 29 ) return 'T' ;
        else if ( num == 30 ) return 'U' ;
        else if ( num == 31 ) return 'V' ;
        else if ( num == 32 ) return 'W' ;
        else if ( num == 33 ) return 'X' ;
        else if ( num == 34 ) return 'Y' ;
        else if ( num == 35 ) return 'Z' ;
        else {
            cout << "Convert integer into character ERROR!" << endl ;
            return ' ' ;
        } // else
    } // convert integer into char

    void PreSortData(){
        sort( m_data.begin(), m_data.end(), CompareByArrivalTime ); // 依arrival time對資料排序
        // 若有arrival time相同的process，就依照process ID排序
        int count = 1 ; // 有多少process arrival time是一樣的
        int startIndex = 0 ; // 從哪個index開始arrival time相同
        for( int i = 0 ; i < m_data.size() ; i++ ) {
            if( i+1 < m_data.size() && m_data[i].Arrival_Time == m_data[i+1].Arrival_Time ) {
                if ( count == 1 ) // 設定開始相同arrival time的index
                    startIndex = i ;
                count++ ;
            } // if
            else if( count > 1 ){ // 換下一種數字，而之前有相同數組
                // 相同arrival time資料用ID排序
                sort( m_data.begin()+startIndex, m_data.begin()+i+1, CompareByID ) ; // range [first,last)
                count = 1 ;
            } // else if
        } // for
    } // sort by Arrival time and ID(if arrival time same)

    void FCFS(){ // 不可奪取的(Non Preemptive)=>不用考慮time slice
        for( int i = 0 ; i < m_data.size() ; i++ ) {
            while ( m_data[i].Arrival_Time - currentTime > 0 ) { // 目前時間無process
                m_gantt.push_back('-') ; // idel CPU閒置
                currentTime++ ;
            } // while

            for( int j = 0 ; j < m_data[i].CPU_Burst ; j++ ) // 加ID至甘特圖
                m_gantt.push_back(ConvertIntToChar( m_data[i].ID )) ;
            currentTime = currentTime + m_data[i].CPU_Burst ;
            m_data[i].TurnAroundTime = currentTime - m_data[i].Arrival_Time ;
            m_data[i].WaitingTime = m_data[i].TurnAroundTime - m_data[i].CPU_Burst ;
        } // for
        m_AllGantt.push_back(m_gantt) ;
    } // First Come First Served

    void RR( int timeSlice ){ // 可奪取的(Preemptive)=>考慮time slice
        bool unfinish = false ; // 到下一個time slice有尚未做完的process
        IDInformation temp ; // 尚未做完的process
        for( int i = 0 ; i < m_data.size() ; i++ ) {
            while ( m_data[i].Arrival_Time - currentTime > 0 && m_readyVector.size() == 0) { // 目前時間無任何process
                m_gantt.push_back('-') ; // idel CPU閒置
                currentTime++ ;
            } // while

            SaveNewProcess(i) ; // 將新來的process加進ready queue
            // 把目前時間抵達的process都加入ready queue內(後方)
            while( i+1 < m_data.size() && currentTime >= m_data[i+1].Arrival_Time ) {
                SaveNewProcess(i + 1) ;
                i++ ;
            } // while

            // 當(下一個process尚未抵達)或(已無process準備抵達且ready queue還有process)
            // ，則對目前於ready queue的process們進行排程
            while ( (i+1 < m_data.size() && currentTime < m_data[i+1].Arrival_Time && m_readyVector.size() != 0 )
                 || ( i+1 == m_data.size() && m_readyVector.size() != 0 ) ){
                if ( unfinish ) { // 此時有process剛結束time slice且未做完(必須加在新進的process後面)
                    m_readyVector.erase(m_readyVector.begin()) ; // 刪除第一個元素
                    m_readyVector.push_back( temp ) ; // 尚未做完的process回到queue的最後排隊
                } // if

                for( int j = timeSlice ; j != 0 ; j-- ) { // 使用一個time slice
                    if ( m_readyVector.front().remain_CPUBurst == 0 ) // 已完成但仍有time slice的process(讓給其他人使用)
                        break ;
                    m_gantt.push_back( ConvertIntToChar(m_readyVector.front().ID) ) ;
                    m_readyVector.front().remain_CPUBurst-- ;
                    currentTime++ ;
                } // for

                if ( m_readyVector.front().remain_CPUBurst == 0 ) { // 已完成的process
                    unfinish = false ;
                    m_data[m_readyVector.front().indexInData].TurnAroundTime =
                            currentTime - m_data[m_readyVector.front().indexInData].Arrival_Time ;
                    m_data[m_readyVector.front().indexInData].WaitingTime =
                            m_data[m_readyVector.front().indexInData].TurnAroundTime - m_data[m_readyVector.front().indexInData].CPU_Burst ;
                    m_readyVector.erase(m_readyVector.begin()) ; // 刪除第一個元素
                } // if
                else { // 未完成的process
                    unfinish = true ;
                    temp = m_readyVector.front() ;
                } // else
            } // while
        } // for
        m_AllGantt.push_back(m_gantt) ;
    } // Round Robin

    void SRTF( int timeSlice ){ // 可奪取的(Preemptive)=>考慮time slice
        for( int i = 0 ; i < m_data.size() ; i++ ) {
            while ( m_data[i].Arrival_Time - currentTime > 0 && m_readyVector.size() == 0) { // 目前時間無任何process
                m_gantt.push_back('-') ; // idel CPU閒置
                currentTime++ ;
            } // while

            SaveNewProcess(i) ; // 將新來的process加進ready vector
            // 把目前時間抵達的process都加入ready vector內(後方)
            while( i+1 < m_data.size() && currentTime >= m_data[i+1].Arrival_Time ) {
                SaveNewProcess(i + 1) ;
                i++ ;
            } // while

            // 當(下一個process尚未抵達)或(已無process準備抵達且ready vector還有process)
            // ，則對目前於ready vector的process們進行排程
            while ( (i+1 < m_data.size() && currentTime < m_data[i+1].Arrival_Time && m_readyVector.size() != 0)
                    || ( i+1 == m_data.size() && m_readyVector.size() != 0 ) ){
                sort(m_readyVector.begin(), m_readyVector.end(), CompareByCPUBurst ) ;

                for( int j = timeSlice ; j != 0 ; j-- ) { // 使用一個time slice
                    if ( m_readyVector.front().remain_CPUBurst == 0 ) // 已完成但仍有time slice的process(讓給其他人使用)
                        break ;
                    if ( i+1 < m_data.size() && currentTime >= m_data[i+1].Arrival_Time ) { // 過程中有process來要看能否奪走執行權力
                        i++ ;
                        SaveNewProcess(i) ; // 將新來的process加進ready vector
                        // 把目前時間抵達的process都加入ready vector內(後方)
                        while( i+1 < m_data.size() && currentTime >= m_data[i+1].Arrival_Time ) {
                            SaveNewProcess(i + 1) ;
                            i++ ;
                        } // while
                        int now = m_readyVector.front().ID ;
                        sort(m_readyVector.begin(), m_readyVector.end(), CompareByCPUBurst ) ;
                        if (now != m_readyVector.front().ID)
                            break ;
                    } // if
                    m_gantt.push_back( ConvertIntToChar(m_readyVector.front().ID) ) ;
                    m_readyVector.front().remain_CPUBurst-- ;
                    currentTime++ ;
                } // for

                if ( m_readyVector.front().remain_CPUBurst == 0 ) { // 已完成的process
                    m_data[m_readyVector.front().indexInData].TurnAroundTime =
                            currentTime - m_data[m_readyVector.front().indexInData].Arrival_Time ;
                    m_data[m_readyVector.front().indexInData].WaitingTime =
                            m_data[m_readyVector.front().indexInData].TurnAroundTime - m_data[m_readyVector.front().indexInData].CPU_Burst ;
                    m_readyVector.erase(m_readyVector.begin()) ; // 刪除第一個元素
                } // if
            } // while
        } // for
        m_AllGantt.push_back(m_gantt) ;
    } // Shortest Remaining Time First

    void PPRR( int timeSlice ){ // 可奪取的(Preemptive)=>考慮time slice
        bool unfinish = false ; // 到下一個time slice有尚未做完的process
        bool preemptive = false ; // 是否有被奪取
        IDInformation temp ; // 尚未做完的process
        for( int i = 0 ; i < m_data.size() ; i++ ) {
            while ( m_data[i].Arrival_Time - currentTime > 0 && m_readyVector.size() == 0) { // 目前時間無任何process
                m_gantt.push_back('-') ; // idel CPU閒置
                currentTime++ ;
            } // while

            SaveNewProcess(i) ; // 將新來的process加進ready queue
            // 把目前時間抵達的process都加入ready queue內(後方)
            while( i+1 < m_data.size() && currentTime >= m_data[i+1].Arrival_Time ) {
                SaveNewProcess(i + 1) ;
                i++ ;
            } // while

            // 當(下一個process尚未抵達)或(已無process準備抵達且ready queue還有process)
            // ，則對目前於ready queue的process們進行排程
            while ( (i+1 < m_data.size() && currentTime < m_data[i+1].Arrival_Time && m_readyVector.size() != 0)
                    || ( i+1 == m_data.size() && m_readyVector.size() != 0 ) ){
                if ( unfinish ) { // 此時有process剛結束time slice且未做完(必須加在新進的process後面)
                    if (!preemptive)
                        m_readyVector.erase(m_readyVector.begin()) ; // 刪除第一個元素
                    m_readyVector.push_back( temp ) ; // 尚未做完的process回到queue的最後排隊
                    preemptive = false ;
                } // if
                sort(m_readyVector.begin(), m_readyVector.end(), CompareByPriority ) ;

                for( int j = timeSlice ; j != 0 ; j-- ) { // 使用一個time slice
                    if ( m_readyVector.front().remain_CPUBurst == 0 ) // 已完成但仍有time slice的process(讓給其他人使用)
                        break ;
                    if (i+1 < m_data.size() && currentTime >= m_data[i+1].Arrival_Time ) { // 過程中有process來要看能否奪走執行權力
                        i++ ;
                        SaveNewProcess(i) ; // 將新來的process加進ready queue
                        // 把目前時間抵達的process都加入ready queue內(後方)
                        while( i+1 < m_data.size() && currentTime >= m_data[i+1].Arrival_Time ) {
                            SaveNewProcess(i + 1) ;
                            i++ ;
                        } // while
                        temp = m_readyVector.front() ; // 暫存目前做到一半的
                        m_readyVector.erase(m_readyVector.begin()) ; // 刪除第一個元素
                        sort( m_readyVector.begin(), m_readyVector.end(), CompareByPriority ) ;
                        if ( temp.Priority > m_readyVector.front().Priority ){ // 要被奪取了
                            preemptive = true ;
                            unfinish = true ;
                            break ;
                        } // if
                        else {
                            preemptive = false ;
                            m_readyVector.insert( m_readyVector.begin(), temp ) ;
                        } // else
                    } // if
                    m_gantt.push_back( ConvertIntToChar(m_readyVector.front().ID) ) ;
                    m_readyVector.front().remain_CPUBurst-- ;
                    currentTime++ ;
                } // for

                if ( m_readyVector.front().remain_CPUBurst == 0 ) { // 已完成的process
                    unfinish = false ;
                    m_data[m_readyVector.front().indexInData].TurnAroundTime =
                            currentTime - m_data[m_readyVector.front().indexInData].Arrival_Time ;
                    m_data[m_readyVector.front().indexInData].WaitingTime =
                            m_data[m_readyVector.front().indexInData].TurnAroundTime - m_data[m_readyVector.front().indexInData].CPU_Burst ;
                    m_readyVector.erase(m_readyVector.begin()) ; // 刪除第一個元素
                } // if
                else if (!preemptive){ // 未完成的process
                    unfinish = true ;
                    temp = m_readyVector.front() ;
                } // else if
            } // while
        } // for
        m_AllGantt.push_back(m_gantt) ;
    } // Preemptive Priority + Round Robin

    void HRRN(){ // 不可奪取的(Non Preemptive)=>不用考慮time slice
        for( int i = 0 ; i < m_data.size() ; i++ ) {
            while ( m_data[i].Arrival_Time - currentTime > 0 && m_readyVector.size() == 0) { // 目前時間無任何process
                m_gantt.push_back('-') ; // idel CPU閒置
                currentTime++ ;
            } // while

            SaveNewProcess(i) ; // 將新來的process加進ready vector
            // 把目前時間之前抵達的process都加入ready vector內(後方)
            while( i+1 < m_data.size() && currentTime >= m_data[i+1].Arrival_Time ) {
                SaveNewProcess(i + 1) ;
                i++ ;
            } // while

            // 計算queue(vector)內process們的Response Ratio
            for ( int i = 0 ; i < m_readyVector.size() ; i++ ) {
                int waitingTime = currentTime - m_data[m_readyVector[i].indexInData].Arrival_Time ;
                m_readyVector[i].ResponseRatio =
                        (waitingTime + m_readyVector[i].remain_CPUBurst)/m_readyVector[i].remain_CPUBurst ;
            } // for

            // 當(下一個process尚未抵達)或(已無process準備抵達且ready vector還有process)
            // ，則對目前於ready vector的process們進行排程
            while ( (i+1 < m_data.size() && currentTime < m_data[i+1].Arrival_Time && m_readyVector.size() != 0)
                    || ( i+1 == m_data.size() && m_readyVector.size() != 0 ) ){
                sort(m_readyVector.begin(), m_readyVector.end(), CompareByResponseRatio ) ;
                while( m_readyVector.front().remain_CPUBurst != 0 ) {
                    m_gantt.push_back( ConvertIntToChar(m_readyVector.front().ID) ) ;
                    m_readyVector.front().remain_CPUBurst-- ;
                    currentTime++ ;
                } // while

                if ( m_readyVector.front().remain_CPUBurst == 0 ) { // 已完成的process
                    m_data[m_readyVector.front().indexInData].TurnAroundTime =
                            currentTime - m_data[m_readyVector.front().indexInData].Arrival_Time ;
                    m_data[m_readyVector.front().indexInData].WaitingTime =
                            m_data[m_readyVector.front().indexInData].TurnAroundTime - m_data[m_readyVector.front().indexInData].CPU_Burst ;
                    m_readyVector.erase(m_readyVector.begin()) ; // 刪除第一個元素
                } // if

                // 計算queue(vector)內process們的Response Ratio
                for( int i = 0 ; i < m_readyVector.size() ; i++ ) {
                    float waitingTime = currentTime - m_data[m_readyVector[i].indexInData].Arrival_Time ;
                    m_readyVector[i].ResponseRatio = // float = float/float
                        (waitingTime + m_readyVector[i].remain_CPUBurst)/m_readyVector[i].remain_CPUBurst ;
                } // for
            } // while
        } // for
        m_AllGantt.push_back(m_gantt) ;
    } //  Highest Response Ratio Next

    void All( int timeSlice ){
        vector <fileContext> temp ; // 暫存原始資料
        temp.assign(m_data.begin(), m_data.end()) ; // 複製m_data資料至temp

        FCFS() ;
        m_AllData.push_back( m_data ) ;
        Initial() ;

        m_data.assign(temp.begin(), temp.end()) ;
        RR(timeSlice) ;
        m_AllData.push_back( m_data ) ;
        Initial() ;

        m_data.assign(temp.begin(), temp.end()) ;
        SRTF(timeSlice) ;
        m_AllData.push_back( m_data ) ;
        Initial() ;

        m_data.assign(temp.begin(), temp.end()) ;
        PPRR(timeSlice) ;
        m_AllData.push_back( m_data ) ;
        Initial() ;

        m_data.assign(temp.begin(), temp.end()) ;
        HRRN() ;
        m_AllData.push_back( m_data ) ;
        Initial() ;

        vector <fileContext>().swap(temp) ;// 清空temp
    } // Run All

public:

    bool ReadFile( string filename, int &method, int &timeSlice ) { // 讀取資料到vector m_data
        fstream file ;
        string temp ;
        filename = filename + ".txt" ;
        file.open( filename ) ;
        if ( !file.is_open() ) {
            cout << filename << " does not exist!" << endl << endl ;
            return false ;
        } // if
        else { // file is open
            getline( file, temp, '\n' ) ; // 讀取第一行(含method、time slice)
            Cut( temp ) ;
            method = Num[0] ;
            timeSlice = Num[1] ;
            getline( file, temp, '\n' ) ; // 讀取標題行
            while ( getline( file, temp, '\n' ) ) {
                if (temp != "\r")
                    SaveOneBlock( temp ); // 取出string內數字(一個block的資料)並儲存於vector中
            } // while
            file.close() ; // close file
            return true ;
        } // else
    } // read file

    void checkVectorData(){
        for ( int i = 0 ; i < m_readyVector.size() ; i++ ) {
            //cout << m_AllData[1][i].ID << '\t' << m_AllData[1][i].CPU_Burst << '\t'
            //     << m_AllData[1][i].Arrival_Time << '\t' << m_AllData[1][i].Priority << endl ;
            cout << m_readyVector[i].ID <<"," << m_readyVector[i].ResponseRatio << "//" ;
        } // for
        cout << endl ;
    } // check function

    void WriteFile( string filename, string method ) { // 適用前五項method(單一排程法輸出)
        sort( m_data.begin(), m_data.end(), CompareByID ); // 依ID對資料排序(寫檔需要ID排序)
        FILE * fout ;
        filename = "out_" + filename + ".txt" ;
        fout = fopen( filename.c_str(), "w" ) ;
        if ( fout == NULL ){
            cout << "Fail to open file" << filename << endl ;
            return ;
        } // if
        fprintf( fout, "%s\n", method.c_str() ) ; // 輸出排程方法名稱
        for ( int i = 0 ; i < m_gantt.size() ; i++ ){ // 輸出gantt chart
            fprintf( fout, "%c", m_gantt[i] ) ;
        } // for
        fprintf( fout, "\n===========================================================\n\nwaiting\n") ;
        fprintf( fout, "ID\t%s\n===========================================================\n", method.c_str() ) ;
        for ( int i = 0 ; i < m_data.size() ; i++ ) { // 輸出各個ID的waiting time
            fprintf( fout, "%d\t%d\n", m_data[i].ID, m_data[i].WaitingTime ) ;
        } // for
        fprintf( fout, "===========================================================\n\nTurnaround Time\n") ;
        fprintf( fout, "ID\t%s\n===========================================================\n", method.c_str() ) ;
        for ( int i = 0 ; i < m_data.size() ; i++ ) { // 輸出各個ID的turnaround time
            fprintf( fout, "%d\t%d\n", m_data[i].ID, m_data[i].TurnAroundTime ) ;
        } // for
        fclose(fout) ;
    } // write file

    void WriteFileAll( string filename, string method ) { // 全部排程法輸出
        for (int i = 0 ; i < m_AllData.size() ; i++) { // 依ID對資料排序(寫檔需要ID排序)
            sort( m_AllData[i].begin(), m_AllData[i].end(), CompareByID );
        } // for

        FILE * fout ;
        filename = "out_" + filename + ".txt" ;
        fout = fopen( filename.c_str(), "w" ) ;
        if ( fout == NULL ){
            cout << "Fail to open file" << filename << endl ;
            return ;
        } // if
        fprintf( fout, "%s", method.c_str() ) ; // 輸出排程方法名稱(ALL)
        for ( int j = 0 ; j < 5 ; j++ ) {
            fprintf( fout, "\n==        %s==\n", methodName[j].c_str() ) ; // 輸出排程方法名稱
            for ( int i = 0 ; i < m_AllGantt[j].size() ; i++ ){ // 輸出gantt chart
                fprintf( fout, "%c", m_AllGantt[j][i] ) ;
            } // for
        } // for
        fprintf( fout, "\n===========================================================\n\nwaiting\n") ;
        fprintf( fout, "ID\tFCFS\tRR\tSRTF\tPPRR\tHRRN\n===========================================================\n") ;
        for ( int i = 0 ; i < m_AllData[0].size() ; i++ ) { // 每個牌程法的id數量皆相同
            // 輸出各個ID的waiting time
            // checkVectorData() ;
            // cout << m_AllData[1].size() << endl ;
            // cout << i << endl ;
            fprintf( fout, "%d\t%d\t%d\t%d\t%d\t%d\n", m_AllData[0][i].ID,
                     m_AllData[0][i].WaitingTime, m_AllData[1][i].WaitingTime, m_AllData[2][i].WaitingTime,
                     m_AllData[3][i].WaitingTime, m_AllData[4][i].WaitingTime) ;
        } // for

        fprintf( fout, "===========================================================\n\nTurnaround Time\n") ;
        fprintf( fout, "ID\tFCFS\tRR\tSRTF\tPPRR\tHRRN\n===========================================================\n") ;
        for ( int i = 0 ; i < m_AllData[0].size() ; i++ ) { // 每個牌程法的id數量皆相同
            // 輸出各個ID的turnaround time
            fprintf( fout, "%d\t%d\t%d\t%d\t%d\t%d\n", m_AllData[0][i].ID,
                     m_AllData[0][i].TurnAroundTime, m_AllData[1][i].TurnAroundTime, m_AllData[2][i].TurnAroundTime,
                     m_AllData[3][i].TurnAroundTime, m_AllData[4][i].TurnAroundTime) ;
        } // for
        fprintf( fout, "===========================================================\n\n") ;
        fclose(fout) ;
    } // write all method in file

    void Schedule( int method, int timeSlice, string &methodName ){ // 進行各式排程
        PreSortData() ;
        if ( method == 1 ) { // First Come First Served
            methodName = "FCFS" ;
            FCFS();
        } // if
        else if ( method == 2 ) { // Round Robin
            methodName = "RR" ;
            RR( timeSlice ) ;
        } // else if
        else if ( method == 3 ) { // Shortest Remaining Time First
            methodName = "SRTF" ;
            SRTF( timeSlice ) ;
        } // else if
        else if ( method == 4 ) { // Preemptive Priority + Round Robin
            methodName = "PPRR" ;
            PPRR( timeSlice ) ;
        } // else if
        else if ( method == 5 ) { // Highest Response Ratio Next
            methodName = "HRRN" ;
            HRRN() ;
        } // else if
        else { // Run all, method == 6
            methodName = "All" ;
            All( timeSlice ) ;
        } // else
    } // do schedule

    void Initial(){
        currentTime = 0 ;
        vector <fileContext>().swap(m_data) ;
        vector <char>().swap(m_gantt) ;
        vector <IDInformation>().swap(m_readyVector) ;
    } // 初始化

    void Initial_All(){
        vector <vector<fileContext>>().swap(m_AllData) ;
        vector <vector<char>>().swap(m_AllGantt) ;
    } // 初始化AllData vector
};

void UserInformation(){
    cout << "***** OS HW2 CPU Scheduling *****" << endl ;
    cout << "** 0. QUIT                     **" << endl ;
    cout << "** 1. START                    **" << endl ;
    cout << "*********************************" << endl ;
    cout << "Input a command(0, 1): " ;
}  // 使用者介面

int main() {
    string cmd ;
    string filename ;
    string methodName ;
    int method = 0 ;  // 排程方法編號
    int timeSlice = 0 ; // time slice(範圍不定)
    Method l_method ;

    UserInformation() ;
    cin >> cmd ;
    while( cmd != "0" ){
        if( cmd == "1" ){
            cout << "Please input filename : " ;
            cin >> filename ;
            while ( !l_method.ReadFile( filename, method, timeSlice ) ) {
                cout << "Please input filename : " ;
                cin >> filename ;
            } // while

            l_method.Schedule( method, timeSlice, methodName ) ;
            if ( method != 6 )
                l_method.WriteFile( filename, methodName ) ;
            else
                l_method.WriteFileAll( filename, methodName ) ;
        } // if
        else {
            cout << endl << "Command does not exist!" << endl ;
        } // else

        l_method.Initial() ;
        l_method.Initial_All() ;
        UserInformation() ;
        cin >> cmd ;
    } // while

    return 0;
} // main
