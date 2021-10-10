#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <queue>
#include <ctime>
#include <thread>
#include <unistd.h>
#include<sys/wait.h>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#pragma GCC optimize(2)

using namespace std;
vector <int> gvStr ; // copy vStr
int gint1 ;
int gint2 ;

class sorting {

    vector <int> vStr ;
    vector <int> vStrTemp ;
    int int1 = 0 ;
    int int2 = 0 ;

public :

    int getvStrSize() {
        return vStr.size() ;
    } // get data size

    bool ReadFile( string filename ) {
        fstream file ;
        string temp ;
        int num = -1 ;
        filename = filename + ".txt" ;
        file.open( filename ) ;
        if ( !file.is_open() ) {
            cout << filename << " does not exist!" << endl << endl ;
            return false ;
        } // if
        else { // file is open
            while ( getline( file, temp, '\n' ) ) {
                num = stoi(temp) ;
                vStr.push_back( num ) ;
                gvStr.push_back( num ) ;
            } // while
            file.close() ; // close file
            return true ;
        } // else
    } // read file

    void WriteFile( string filename, string taskNum, long cputime ) {
        FILE * fout ;
        int dataSize = getvStrSize() ;
        filename = filename + "_output" + taskNum + ".txt" ;
        fout = fopen( filename.c_str(), "w" ) ;
        if ( fout == NULL ){
            cout << "Fail to open file" << filename << endl ;
            return ;
        } // if
        fprintf( fout, "Sort : \n" ) ;
        for ( int i = 0 ; i < dataSize ; i++ ){
            fprintf( fout, "%d\n", vStr[i] ) ;
        } // for
        fprintf( fout, "CPU Time : %ld\n", cputime ) ;

        time_t now = time(NULL) ;
        char temp[80];
        strftime( temp, sizeof(temp), "%Y-%m-%d %X.%S %Z",localtime(&now) );

        fprintf( fout, "Output Time : %s\n", temp ) ;
        fclose(fout) ;
    } // write file

    void bubbleSort() { // ?勗??之??
        bool sorted = true ;
        int datasize = getvStrSize() ;
        for ( int i = 0 ; sorted && i < datasize - 1 ; i++ ){ // ??頞嚗orted?亦false?”隞交?摨???銝???摨?
            sorted = false ;
            for ( int j = 0 ; j < datasize - 1 - i ; j++ ){ // 瘥?瘥??活??
                if ( vStr[j] > vStr[j+1] ){
                    swap( vStr[j], vStr[j+1] ) ;
                    sorted = true ; // ?詨廣wap????摨?敹?
                } // if
            } // for
        } // for
    } // BubbleSort() 瘞?部??

    void mergesort( int first, int last ) {
        if ( first < last ) {
            int mid = ( first + last ) / 2 ;
            mergesort( first, mid ) ;
            mergesort( mid+1, last ) ;
            mergeSort_merge( first, mid, last ) ;
        } // if
    } // mergesort()

    void mergeSort_merge( int first, int mid, int last ){ // ?蔥?典?
        int first1 = first, last1 = mid ;
        int first2 = mid+1, last2 = last ;
        int index = first ;
        vStrTemp.assign( vStr.begin(), vStr.end() ) ;
        for( ; ( first1 <= last1 ) && (first2 <= last2 ) ; index++ ){
            int1 = vStr[first1] ;
            int2 = vStr[first2] ;
            if ( int1 <= int2 ){
                vStrTemp[index] = vStr[first1] ;
                first1++ ;
            } // if
            else{
                vStrTemp[index] = vStr[first2] ;
                first2++ ;
            } // else
        } // for

        for( ; first1 <= last1 ; index++ ){
            int1 = vStr[first1] ;
            vStrTemp[index] = vStr[first1] ;
            first1++ ;
        } // for

        for( ; first2 <= last2 ; index++ ){
            int2 = vStr[first2] ;
            vStrTemp[index] = vStr[first2] ;
            first2++ ;
        } // for

        vStr.assign( vStrTemp.begin(), vStrTemp.end() ) ;

    } // mergeSort_merge()

    void initial() {
        vector <int> ().swap( vStr ) ;
        vector <int> ().swap( vStrTemp ) ;
        vector <int> ().swap( gvStr ) ;
        int1 = 0 ;
        int2 = 0 ;
        gint1 = 0;
        gint2 = 0;
    } // initial

}; // class sorting

void thread_bubbleSort( int start, int ending ) {
    bool sorted = true ;
    for ( int i = start ; sorted && i <= ending ; i++ ){ // ??頞嚗orted?亦false?”隞交?摨???銝???摨?
        int count_run_time = 0 ; // bubble頝?甈⊥
        sorted = false ;
        for ( int j = start ; j < ending - count_run_time ; j++ ){ // 瘥?瘥??活??
            if ( gvStr[j] > gvStr[j+1] ){
                swap( gvStr[j], gvStr[j+1] ) ;
                sorted = true ; // ?詨廣wap????摨?敹?
            } // if
        } // for
        count_run_time++ ;
    } // for
} // thread_bubbleSort

void process_bubbleSort( int start, int ending, void*a ) {
    bool sorted = true ;
    int temp[gvStr.size()] ;
    memcpy( temp, a, gvStr.size()*4 ) ;
    for ( int i = start ; sorted && i <= ending ; i++ ){ // ??頞嚗orted?亦false?”隞交?摨???銝???摨?
        int count_run_time = 0 ; // bubble頝?甈⊥
        sorted = false ;
        for ( int j = start ; j < ending - count_run_time ; j++ ){ // 瘥?瘥??活??
            if ( temp[j] > temp[j+1] ){
                swap( temp[j], temp[j+1] ) ;
                sorted = true ; // ?詨廣wap????摨?敹?
            } // if
        } // for
        count_run_time++ ;
    } // for
    memcpy( a, temp, gvStr.size()*4 ) ;
} // thread_bubbleSort

void thread_mergeSort_merge( int first, int mid, int last ){ // ?蔥?典?
    int first1 = first, last1 = mid ;
    int first2 = mid+1, last2 = last ;
    int index = first ;
    vector<int> temp ;
    for( ; ( first1 <= last1 ) && (first2 <= last2 ) ;  ){
        gint1 = gvStr[first1] ;
        gint2 = gvStr[first2] ;
        if ( gint1 <= gint2 ){
            temp.push_back( gvStr[first1] ) ;
            first1++ ;
        } // if
        else{
            temp.push_back( gvStr[first2] ) ;
            first2++ ;
        } // else
    } // for

    for( ; first1 <= last1 ;  ){
        temp.push_back( gvStr[first1] ) ;
        first1++ ;
    } // for

    for( ; first2 <= last2 ;  ){
        temp.push_back( gvStr[first2] ) ;
        first2++ ;
    } // for

    for ( int i = 0; i < temp.size() ; i++ ) {
        gvStr.at(index) = temp.at(i) ;
        index++ ;
    } // for

} // mergeSort_merge()

void thread_mergesort( int first, int last, int k, int portion_size ) { // ?
    thread threads[2] ;
    if ( k > 1 ) { // ?斗?臬閬匱蝥?
        int mid = first + (k/2)*portion_size-1  ;
        threads[0] = thread(thread_mergesort, first, mid, k/2, portion_size) ;
        if ( k%2 != 0 ){ // 憟隞賢??銝撅???隞賢?斗)
            k = k/2 + 1 ;
        } // if
        else {
            k = k/2 ;
        } // else
        threads[1] = thread(thread_mergesort, mid+1, last, k, portion_size) ;
        threads[0].join() ;
        threads[1].join() ;
        thread_mergeSort_merge( first, mid, last ) ;
    } // if
    else {
        thread_bubbleSort( first, last ) ; // ??拐?隞踝???bubble???隞?
    } // else

} // mergesort()

void task4_mergesort( int first, int last, int k, int portion_size ) { // ?
    if ( k > 1 ) { // ?斗?臬閬匱蝥?
        int temp = k/2 ;
        int mid = first + ( portion_size*temp-1 ) ;
        task4_mergesort( first, mid, temp, portion_size ) ;
        if ( k%2 == 1 ){ // 憟隞賢??銝撅?
            temp++ ;
        } // if
        task4_mergesort( mid+1, last, temp, portion_size ) ;
        thread_mergeSort_merge( first, mid, last ) ;
    } // if
    else {
        thread_bubbleSort( first, last ) ; // ??拐?隞踝???bubble???隞?
    } // else

} // mergesort()

void* create_shared_memory(size_t size) {
    // Our memory buffer will be readable and writable:
    int protection = PROT_READ | PROT_WRITE;

    // The buffer will be shared (meaning other processes can access it), but
    // anonymous (meaning third-party processes cannot obtain an address for it),
    // so only this process and its children will be able to use it:
    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    // The remaining parameters to `mmap()` are not important for this use case,
    // but the manpage for `mmap` explains their purpose.
    return mmap(NULL, size, protection, visibility, -1, 0);
}

void process_mergeSort_merge( int first, int mid, int last, void* a ){ // ?蔥?典?
    int first1 = first, last1 = mid ;
    int first2 = mid+1, last2 = last ;
    int index = first ;
    int tempp[gvStr.size()] ;
    memcpy( tempp, a, gvStr.size()*4 ) ;
    vector<int> temp ;
    for( ; ( first1 <= last1 ) && (first2 <= last2 ) ;  ){
        gint1 = tempp[first1] ;
        gint2 = tempp[first2] ;
        if ( gint1 <= gint2 ){
            temp.push_back( tempp[first1] ) ;
            first1++ ;
        } // if
        else{
            temp.push_back( tempp[first2] ) ;
            first2++ ;
        } // else
    } // for

    for( ; first1 <= last1 ;  ){
        temp.push_back( tempp[first1] ) ;
        first1++ ;
    } // for

    for( ; first2 <= last2 ;  ){
        temp.push_back( tempp[first2] ) ;
        first2++ ;
    } // for

    for ( int i = 0; i < temp.size() ; i++ ) {
        tempp[index] = temp.at(i) ;
        index++ ;
    } // for
    memcpy( a, tempp, gvStr.size()*4 ) ;

} // mergeSort_merge()

void task3_mergesort( int first, int last, int k, int portion_size, void* a ) {
    if ( k>1 ){
        pid_t pid = fork() ;
        int temp = k/2 ;
        int mid = first + ( portion_size*temp-1 ) ;
        if ( pid > 0 ) { // parent process
            if ( k%2 == 1 ){ // 若為奇數份右邊多做一層
                temp++ ;
            } // if
            wait(0) ;
            task3_mergesort( mid+1, last, temp, portion_size, a ) ;
            process_mergeSort_merge( first, mid, last, a ) ;
        } // if
        else if ( pid == 0 ) { // child process
            task3_mergesort( first, mid, temp, portion_size, a ) ;
            exit(0) ;
        } // else if
        else { // fail to create process
            cout << "ERROR TO CREATE PROCESS" << endl ;
        } // else
    } // if
    else {
        process_bubbleSort( first, last, a ) ; // ??拐?隞踝???bubble???隞?
    } // else
}

void gWriteFile( string filename, string taskNum, long cputime ) {
    FILE * fout ;
    int dataSize = gvStr.size() ;
    filename = filename + "_output" + taskNum + ".txt" ;
    fout = fopen( filename.c_str(), "w" ) ;
    if ( fout == NULL ){
        cout << "Fail to open file" << filename << endl ;
        return ;
    } // if
    fprintf( fout, "Sort : \n" ) ;
    for ( int i = 0 ; i < dataSize ; i++ ){
        fprintf( fout, "%d\n", gvStr[i] ) ;
    } // for
    fprintf( fout, "CPU Time : %ld\n", cputime ) ;

    time_t now = time(NULL) ;
    char temp[80];
    strftime( temp, sizeof(temp), "%Y-%m-%d %X.%S %Z",localtime(&now) );

    fprintf( fout, "Output Time : %s\n", temp ) ;
    fclose(fout) ;
} // write file

void infortable() {
    cout << "***** Process & Thread *****" << endl ;
    cout << "** 0. Quit                **" << endl ;
    cout << "** 1. task1               **" << endl ;
    cout << "** 2. task2               **" << endl ;
    cout << "** 3. task3               **" << endl ;
    cout << "** 4. task4               **" << endl ;
    cout << "****************************" << endl ;
    cout << "Input a command(0, 1, 2, 3, 4): " ;
} // information table

int main()
{
    string cmd ;
    string filename ;
    int portion ; // 隞賣k
    int portion_size = -1 ; // 瘥遢???鞈???
    int k = 1 ;
    sorting cSort ;
    clock_t start, ending;
    long time ;

    infortable() ;
    cin >> cmd ;

    while ( cmd != "0" ) { // if command is 0 -> Quit
        if ( cmd == "1" ) { // task1
            cout << "Please input filename : " ;
            cin >> filename ;
            while ( !cSort.ReadFile( filename ) ) {
                cout << "Please input filename : " ;
                cin >> filename ;
            } // while
            start = clock() ;
            cSort.bubbleSort() ;
            ending = clock() ;
            time = ending - start ;
            cSort.WriteFile( filename, cmd, time ) ;
        } // if
        else if ( cmd == "2" ) { // task2
            cout << "Please input filename : " ;
            cin >> filename ;
            while ( !cSort.ReadFile( filename ) ) {
                cout << "Please input filename : " ;
                cin >> filename ;
            } // while
            cout << "Please input how many pieces you want to cut : k = " ;
            cin >> portion ;
            if ( portion >= 1 ) {
                k = portion ;
            } // if

            portion_size = cSort.getvStrSize()/k ;

            start = clock() ;
            thread_mergesort( 0, gvStr.size()-1, k, portion_size ) ;
            ending = clock() ;
            time = ending - start ;

            gWriteFile( filename, cmd, time ) ;
        } // else if
        else if ( cmd == "3" ) {
            cout << "Please input filename : " ;
            cin >> filename ;
            while ( !cSort.ReadFile( filename ) ) {
                cout << "Please input filename : " ;
                cin >> filename ;
            } // while
            cout << "Please input how many pieces you want to cut : k = " ;
            cin >> portion ;
            if ( portion >= 1 ) {
                k = portion ;
            } // if
            int a[cSort.getvStrSize()] ;

            for ( int i = 0 ; i < cSort.getvStrSize() ; i++ ) {
                a[i] = gvStr[i] ;
            } // for

            portion_size = cSort.getvStrSize()/k ;

            void * shmem = create_shared_memory(gvStr.size()*4) ;
            int array[gvStr.size()] ;
            for ( int i = 0 ; i < gvStr.size() ; i++ ) {
                array[i] = gvStr[i] ;
            } // for
            memcpy( shmem, array, gvStr.size()*4 ) ;

            start = clock() ;
            task3_mergesort( 0, gvStr.size()-1, k, portion_size, shmem ) ;
            ending = clock() ;
            time = ending - start ;
            memcpy( array, shmem, gvStr.size()*4 ) ;
            for ( int i = 0 ; i < gvStr.size() ; i++ ) {
                gvStr[i] = array[i] ;
            } // for
            gWriteFile( filename, cmd, time ) ;
        } // else if
        else if ( cmd == "4" ) {
            cout << "Please input filename : " ;
            cin >> filename ;
            while ( !cSort.ReadFile( filename ) ) {
                cout << "Please input filename : " ;
                cin >> filename ;
            } // while
            cout << "Please input how many pieces you want to cut : k = " ;
            cin >> portion ;
            if ( portion >= 1 ) {
                k = portion ;
            } // if

            portion_size = cSort.getvStrSize()/k ;

            start = clock() ;
            task4_mergesort( 0, gvStr.size()-1, k, portion_size ) ;
            ending = clock() ;
            time = ending - start ;
            gWriteFile( filename, cmd, time ) ;
        } // else if
        else {
            cout << endl << "Command does not exist!" << endl ;
        } // else

        cSort.initial() ;
        infortable() ;
        cin >> cmd ;
    } // while

    return 0;
} // main