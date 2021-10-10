#include <vector>
#include <string>
#include <stack>    // push, pop, top 
#include<fstream>
#include <iostream>
#include<stdlib.h>
#include<cmath>
#define PTR_NUM 3
#define KEY_NUM PTR_NUM - 1

using namespace std ;

static int numtocut[5] = { 1, 3, 4, 5, 8 } ;

struct AVLinfoType { // AVL
	int graNum ;
	vector <int> ids ;
	AVLinfoType * right ;
	AVLinfoType * left ;
} ;

typedef AVLinfoType * AVLinfoTypePtr ;

typedef struct infoType {
	int num ;
	string info ;
} infoType ;

typedef struct slotType { // 一個單位(一個學校名對應到的所有資料) 
	vector <int> rSet ;   // 學校名稱對應到的所有資料
	string key ;          // 學校名稱 
} slotType ;

typedef struct nodeType {             // 2-3 tree 的節點 
	slotType data[KEY_NUM] ;          // 兩個學校名稱 (0->小) 
	struct nodeType * link[PTR_NUM] ; // a list of pointers (左,右,中) 
	struct nodeType * parent ;        // a pointer to the parent node(用於分裂split) 
} nodeType ;

typedef struct pointType { // a point on the search path(樹根一路往下經過的節點們) 
	nodeType * pnode ;     // pointer to a parent node
	int pidx ;             // entrance index on the parent node(紀錄往下走是走左,右,中) 
} pointType ;

typedef struct blockType { // a data block received from a split (用於分裂-每次分裂都須把資訊往上帶) 
	slotType slot ;        // a pair of (record id, key) // 學校名稱-往上叫到父節點的時候 
	nodeType * link ;      // a pointer to a child on the right
} blockType ;

typedef struct readType {
	int id ;
	int granum ;
	string name ;
	string info ;
} readType ;

class Reader {
	
	public:
		int getsize() {
			return data.size() ;
		}
		readType getdata( int i ) {
			return data[i];
		}
		
		string findstr( string line, int j ) {
	        int tab = 0 ;
	        int nexttab = 0 ;
	        bool found = false ;
	        int tabnum = 0 ;
	        for ( int i = 0 ; i < line.length() && tabnum >= 0 ; i++ ) {
	            if ( line.c_str()[i] == '\t' ){
	                tabnum++ ;
	            }
	            if ( tabnum == j && !found ){
	                tab = i ;
	                found = true ;
	            }
	            if ( tabnum == j + 1 && found ){
	                nexttab = i ;
	                tabnum = -1 ;
	            }
	        } // for
	        string str = line.substr( tab + 1 , nexttab - tab - 1 ) ;
	        return str ;
	    } // classHeap::findstr()
	
		string cut( string line ) {
			int i = 0 ;
			string restr ;
			restr = findstr( line, numtocut[i] ) ;
	
			for(i = 1 ; i < 5 ; i++ ){
				//cout << restr << endl ;
				restr += ", " ;
				restr += findstr( line, numtocut[i] ) ;
			}
	
			return restr ;
				
		}
		
	    bool Read( string fileName ) {
	        string temp ;
	        fstream file ;
	        fileName = "input" + fileName + ".txt" ;
	        file.open( fileName.c_str() ,ios::in ) ;
	        if ( !file.is_open() ) { // check if file is open
	            cout << endl << "### " << fileName << " does not exist! " << "###" << endl << endl ;
	            return false ;
	        } // if
	        else {
	            int num = 0 ;
	            for ( int i = 0 ; i < 3 ; i++ ) {
	                getline( file, temp, '\n' ) ;
	            }
	
	            while( !file.eof() ) {
	            	
	                getline( file, temp, '\n' ) ;
	                
	                
	                if ( temp != "" ){
	                    item.name = findstr( temp, 1 ).c_str() ;
						item.info = cut(temp) ;
	                    item.id = num + 1 ;
	                    item.granum = atoi( findstr( temp, 8 ).c_str() ) ;
	                    data.push_back( item ) ;
	                }
	                num++ ;
	            }
	            file.close() ;
	            return true ;
	        } // else
	        
	    } // classHeap::Read()
	    
	    void initial() {
			data.clear() ;
		}
	private:
		vector <struct readType> data ;
		struct readType item ;
};

class Twothreetree {
	
	public:
		
	twothreetree() {
		root = NULL ;
	}
	
	int getrootsize( int i ) {
		int re = root->data[i].rSet.size();
		return  re;
	}
	
	int getrootid( int i, int j ) {
		int re = 999999 ;
		if( root->data[i].rSet.size() != 0 )
			re = root->data[i].rSet.at(j) ;
		return  re;
	}
	
	void insert23tree( int newRid, string newKey ) {
		// add one record into 23 tree
		// input: tree root, a new record passed as a pair of (newKey, newRid)
		// output: tree root after update
		slotType newSlot ;
		newSlot.rSet.push_back( newRid ) ; // the content of a new record.
		newSlot.key = newKey ;
		
		if ( root == NULL ){ // create the first node with one record inserted
			root = createNode( NULL, NULL, NULL, newSlot ) ;

		}
		else {  // the root exists, ...
			stack <pointType> aPath ; // stack to keep the search path(紀錄走過節點) 
			pointType curP ;          // last-visited node at the top of stack
			blockType blockUp ;       // a data block received from a split
			
			searchPath( root, newKey, aPath ) ; // find a matched position on 23 tree
			
			if ( !aPath.empty() ) {
				
				curP = aPath.top() ; // reference to the last-visited node
				if ( ( curP.pnode->data[curP.pidx].rSet.size() != 0 ) && 
					 ( newKey.compare( curP.pnode->data[curP.pidx].key ) == 0 ) ) {	// same name (It's a duplicate key, so insert it directly)
				    curP.pnode->data[curP.pidx].rSet.push_back( newRid ) ;          // add a new record identifier
				}
				else if ( curP.pnode->data[KEY_NUM-1].rSet.size() == 0 ) {			// leaf and node has one data (at least one (rightmost) unused slot)	
					insertLeaf( newSlot, curP ) ;                                   // add a record into a leaf 
				}
				else { // split a full leaf
					splitLeaf( newSlot, curP, blockUp ) ; // split a leaf for an insertion
					if ( curP.pnode->parent == NULL )     // if a root is split, create a new root(往上創一節點) 
						root = createRoot( curP.pnode, blockUp.link, blockUp.slot ) ;
					else {                                // continue splittig a non-leaf for an insertion
						do {
							aPath.pop() ; 											// last ( forget the current node)
							curP = aPath.top() ;									// last's parent ( the next parent for an insertion)
							if ( curP.pnode->data[KEY_NUM-1].rSet.size() == 0 ) {	// 有空位 ( at least one (rightmost) unused slot
								insertNonleaf( blockUp, curP ) ; // add a slot into a non-leaf
								break ;                          // finish the insertion!
							}
							else {                                                  // split a full non-leaf
								splitNonleaf( curP, blockUp ) ;     // split a non-leaf for an insertion
								if ( curP.pnode->parent == NULL ) { // if a root is split, create a new root
									root = createRoot( curP.pnode, blockUp.link, blockUp.slot ) ;
									break ;                         // finish the insertion!
								}
							}
						} while ( true ) ;                          // repeat until the insertion is terminated
					}	
				}	  
			}
		}
		//cout << "done" ;	
	}
	
	nodeType * createNode( nodeType * left, nodeType * right, nodeType * pNode, slotType newS ) {
		// create a node with one record inserted
		// input: left child, right child, parent, a new record kept as a slot
		// output: a new-created node or NULL
		nodeType * newNode = NULL ;
		
		try {
			newNode = new nodeType ; // create a new node
			newNode->data[0].rSet = newS.rSet ; // put the record into the 1st slot
			newNode->data[1].rSet.clear() ;
			newNode->data[0].key = newS.key ;
			newNode->data[1].key = "" ;
			newNode->parent = pNode ; // set up a link to the parent
			newNode->link[0] = left ; // set up the leftmost link
			newNode->link[1] = right ; // set up the middle link
			newNode->link[2] = NULL ; // clear up the rightmost link
		}
		catch ( std::bad_alloc & ba ) { // unable to allocate space
			std::cerr << endl << "bad_alloc caught: " << ba.what() << endl ;
		} 
		
		return newNode ; // pass a pointer to the new-created node
	}
	
	void searchPath( nodeType * cur, string name, stack <pointType> & path ) {
		// find a matched position on 23 tree
		// input: the root, a name
		// output: the search path
		pointType oneP ;
		int pos ;
		
		while ( cur != NULL ) {
			oneP.pnode = cur ;
			for ( pos = 0 ; pos < KEY_NUM ; pos++ ) {
				if ( ( cur->data[pos].rSet.size() == 0 ) || ( (name.compare( cur->data[pos].key)) < 0 )) // unused slot, name > key or name < key
					break ; // search the next level
				else if ( name.compare( cur->data[pos].key ) == 0 ) { // name == key ( a duplicate! )
					
					oneP.pidx = pos ;   // keep track of the pointer
					path.push( oneP ) ; // visited node: (parent node, entrance index)
					return ;            // the last-visited node is at the top of stack
				}	
			}
			
			oneP.pidx = pos ;      // keep track of the pointer
			path.push( oneP ) ;    // visited node: (parent node, entrance index)
			cur = cur->link[pos] ; // recursive search at the next level
		}
	}
	
	void insertLeaf( slotType newS, pointType & aLeaf ) { // 將學校名稱放至正確位置 
		//cout << "insertleaf" << endl ;
		// add a record into a leaf
		// input: a new slot (rSet, key), the leaf to insert(pnode, pidx)
		// output: leaf after update
		for ( int i = KEY_NUM - 1 ;i >= aLeaf.pidx ; i-- ) // scan from right to left
			if ( i > aLeaf.pidx ) { // shift an existing record to the right
				aLeaf.pnode->data[i].rSet = aLeaf.pnode->data[i-1].rSet ;		// sort by the result of searchpath
				aLeaf.pnode->data[i].key = aLeaf.pnode->data[i-1].key ;
			}
			else if ( i == aLeaf.pidx )  {
				aLeaf.pnode->data[i].rSet = newS.rSet ; // save the new record in a new slot
				aLeaf.pnode->data[i].key = newS.key ; 
			}
			else 
				break ; // earlier termination
		
	}
	
	void splitLeaf( slotType newS, pointType & aLeaf, blockType & aBlock ) { // 樹葉節點已滿做分裂 
		//cout << "splitLeaf" << endl ;
		// split a non-leaf for an insertion
		// input: a new slot(rSet, key), the leaf to insert(pnode, pidx)
		// output: block after split to move upwards
		slotType buf[PTR_NUM] ; // a buffer to keep a full node plus a new record( 小, 中, 大) 
		int idx = 0 ; // index of the full node

		for ( int i = 0 ; i < PTR_NUM ; i++ ) { // 0.1.2 ( fill in the entire buffer)
			if ( i == aLeaf.pidx ) {
				buf[i].rSet = newS.rSet ;
				buf[i].key = newS.key ;
			}
			else {
				buf[i].rSet = aLeaf.pnode->data[idx].rSet ;
				buf[i].key = aLeaf.pnode->data[idx].key ;
				idx++ ;
			}	
		}
		
		aLeaf.pnode->data[0].rSet = buf[0].rSet ; // leave only the leftmost record(最小留著) 
		aLeaf.pnode->data[0].key = buf[0].key ;
		
		for ( int i = 1 ; i < KEY_NUM ; i++ ) { // the remains: unused slots
			aLeaf.pnode->data[i].rSet.clear() ;
		}
		aBlock.link = createNode( NULL, NULL, aLeaf.pnode->parent, buf[2] ) ; // create a sibling node on the right(最大) 
		
		aBlock.slot.rSet = buf[1].rSet ; // block to move upwards
		aBlock.slot.key = buf[1].key ;
	}
	
	nodeType * createRoot( nodeType * & left, nodeType * & right, slotType oneSlot ) {
		//cout << "createRoot" << endl ;
		// create a new root after a split
		// input: left child, right child, one slot in the new root
		// output: a new root in a well-connected tree
		nodeType * newRoot = createNode( left, right, NULL, oneSlot ) ;
		
		left->parent = newRoot ; // set their parent as the new root
		right->parent = newRoot ;
		return newRoot ;
	}
	
	void insertNonleaf( blockType oneB, pointType goal ) {
		// add a record into a non-leaf
		// input: a new block(slot, link), the non-leaf to insert (pnode, pidx)
		// output: non-leaf after update
		//cout << "insertNonleaf" << endl ;
		for ( int i = KEY_NUM - 1 ;i >= goal.pidx ; i-- ) { // 放置正確位置 
			if ( i > goal.pidx ) {
				goal.pnode->data[i].rSet = goal.pnode->data[i-1].rSet ;		// sort by the result of searchpath
				goal.pnode->data[i].key = goal.pnode->data[i-1].key ;
			}
			else if ( i == goal.pidx )  {
				goal.pnode->data[i].rSet = oneB.slot.rSet ;
				goal.pnode->data[i].key = oneB.slot.key ;
			}
			else 
				break ;
		}
		if( goal.pidx == 0 ) { //左升 
			goal.pnode->link[2] = goal.pnode->link[1] ; // 原右給中 
			goal.pnode->link[1] = oneB.link ; // 最大給右 
		}
		else if( goal.pidx == 1 ) { //右升 
			goal.pnode->link[2] = oneB.link ; // 最大給中 
		}
	}
	
	void splitNonleaf( pointType goal , blockType & oneB ) { // split a non-leaf for an insertion
		// input: the leaf to insert(pnode, pidx)
		// input/output: blocks before and after split
		//cout << "splitNonleaf" << endl ;
		slotType buf[PTR_NUM] ;    // a buffer to keep a full node plus a new record
		nodeType *ptr[PTR_NUM+1] ; // a buffer to keep pointer of children
		int idx = 0 ;              // index of the full node

		for ( int i = 0 ; i < PTR_NUM ; i++ ) { // fill in the entire buffer
			if ( i == goal.pidx ) {
				buf[i].rSet = oneB.slot.rSet ;
				buf[i].key = oneB.slot.key ;
			}
			else {
				buf[i].rSet = goal.pnode->data[idx].rSet ;
				buf[i].key = goal.pnode->data[idx].key ;
				idx++ ;
			}	
		}
		
		goal.pnode->data[0].rSet = buf[0].rSet ;
		goal.pnode->data[0].key = buf[0].key ;
		ptr[0] = goal.pnode->link[0] ;
		if( goal.pidx == 0 ) {
			ptr[1] = oneB.link ;
			ptr[2] = goal.pnode->link[1] ;
			ptr[3] = goal.pnode->link[2] ;
		}
		else if( goal.pidx == 1 ) {
			ptr[1] = goal.pnode->link[1] ;
			ptr[2] = oneB.link ;
			ptr[3] = goal.pnode->link[2] ;
		}
		else if( goal.pidx == 2 ) {
			ptr[1] = goal.pnode->link[1] ;
			ptr[2] = goal.pnode->link[2] ;
			ptr[3] = oneB.link ;
		}
		
		
		for ( int i = 1 ; i < KEY_NUM ; i++ ) {
			goal.pnode->data[i].rSet.clear() ;
		}
		goal.pnode->link[0] = ptr[0];
		goal.pnode->link[1] = ptr[1];
		goal.pnode->link[2] = NULL;
		oneB.link = createNode( ptr[2], ptr[3], goal.pnode->parent, buf[2] ) ;
		oneB.slot.rSet = buf[1].rSet ; // block to move upwards
		oneB.slot.key = buf[1].key ;
	}
	
	void free23tree( nodeType * root ) { // release the space of 23 tree
	// input : tree root
	// output: empty tree
		if ( root != NULL ) {
			free23tree( root->link[0] ) ; // recursively traverse the leftmost subtree
			for ( int i = 0 ; i < KEY_NUM ; i++ ) { // scan the slots in a node from left to right
				if ( !root->data[i].rSet.size() ) // encounter the first unused slot
					break ; // the remaining slots are also unused
				free23tree( root->link[i+1] ) ;
			}
			
			delete root ; // post-order tree traversal
		}
	}
	
	int getheight() {
		nodeType * temp = root ;
		return heightrecursion( temp ) ;
	}
	
	int heightrecursion( nodeType * temp ){
		
		if ( temp == NULL )
			return 0 ;
    	else if( temp->link[2] == NULL ) {
    		if( heightrecursion( temp->link[0] ) >= heightrecursion( temp->link[1] ) )
    			return heightrecursion( temp->link[0] ) + 1 ;
    		else 
    			return heightrecursion( temp->link[1] ) + 1 ;	
		}
		else {
			if( heightrecursion( temp->link[0] ) >= heightrecursion( temp->link[1] ) && 
				heightrecursion( temp->link[0] ) >= heightrecursion( temp->link[2] ) )
    			return heightrecursion( temp->link[0] ) + 1 ;
    		else if( heightrecursion( temp->link[1] ) >= heightrecursion( temp->link[0] ) && 
					 heightrecursion( temp->link[1] ) >= heightrecursion( temp->link[2] ) )
    			return heightrecursion( temp->link[1] ) + 1 ;
			else if( heightrecursion( temp->link[2] ) >= heightrecursion( temp->link[0] ) && 
					 heightrecursion( temp->link[2] ) >= heightrecursion( temp->link[1] ) )
    			return heightrecursion( temp->link[2] ) + 1 ;	
		}
    	
	}
	
	int getnum() {
		nodeType * temp = root ;
		return numrecursion( temp ) ;
	}
	
	int numrecursion( nodeType * temp ){

		if ( temp == NULL )
			return 0 ;
    	else if( temp->link[2] == NULL ) {
    		return numrecursion( temp->link[0] ) + numrecursion( temp->link[1] ) + 1 ;
		}
		else {
    		return numrecursion( temp->link[0] ) + numrecursion( temp->link[1] ) + numrecursion( temp->link[2] ) + 1 ;	
		}
    	
	}
	
	void initial() {
		root = NULL ;
	}
	
    private:
    	nodeType * root ; 
};

class AVLtree{

  public:

      int getrootsize( AVLinfoTypePtr root ) {
          return root->ids.size() ;
      }

      int getrootid( int i, AVLinfoTypePtr root ) {
	      int re = 999999 ;
	      if( root->ids.size() != 0 )
		      re = root->ids.at(i) ;
		  return  re;
	  }

      void insertAVLtree( AVLinfoTypePtr & root, int newgra, int newid ) {
          if ( root == NULL ) {
              root = new AVLinfoType ;
              root->left = NULL ;
              root->right = NULL ;
              root->graNum = newgra ;
              root->ids.push_back( newid ) ;
          } // if
          else if( newgra == root->graNum ){
              root->ids.push_back( newid ) ;
          } // else if
          else if( newgra > root->graNum )
              insertAVLtree( root->right, newgra, newid ) ;
          else if( newgra < root->graNum )
              insertAVLtree( root->left, newgra, newid ) ;
          keepbalance( root ) ;
      } // AVLtree::insertAVLtree()

      int getHeight( AVLinfoTypePtr root ){
          if ( root == NULL )// 空的
              return 0 ;
           else { // 尚未找完
              int numR = getHeight( root->right ) ;
              int numL = getHeight( root->left ) ;
              if ( numR > numL )
                return numR + 1 ;
              else
                return numL + 1 ;
           } // else
      } // AVLtree::getHeight()

      int getnodenum( AVLinfoTypePtr root ){
          int num = 1 ;
          if ( root == NULL )
             return 0 ;
          else {
             num = num + getnodenum( root->left ) ;
             num = num + getnodenum( root->right ) ;
          } // else
          return num ;
      } // AVLtree::getnodenum()

      void keepbalance( AVLinfoTypePtr & root ){
          if ( ( getHeight( root->left )- getHeight( root->right ) ) == 2 &&
              (( getHeight( root->left->left )- getHeight( root->left->right ) ) == 1 ||
              ( getHeight( root->left->left )- getHeight( root->left->right ) ) == 0) ){
              root = rotateLL( root ) ;
          } // if
          else if ( ( getHeight( root->left )- getHeight( root->right ) ) == -2 &&
                    (( getHeight( root->right->left )- getHeight( root->right->right ) ) == -1 ||
                    ( getHeight( root->right->left )- getHeight( root->right->right ) ) == 0) ){
              root = rotateRR( root ) ;
          } // else if
          else if ( ( getHeight( root->left )- getHeight( root->right ) ) == 2 &&
                    ( getHeight( root->left->left )- getHeight( root->left->right ) ) == -1 ){
              root = rotateLR( root ) ;
          } // else if
          else if ( ( getHeight( root->left )- getHeight( root->right ) ) == -2 &&
                      (getHeight( root->right->left )- getHeight( root->right->right ) ) == 1 ){
              root = rotateRL( root ) ;
          } // else if
      } // AVLtree::keepbalance()

      AVLinfoTypePtr rotateLL( AVLinfoTypePtr x ) { // rotate x with its left child
          AVLinfoTypePtr y = x->left ;
          x->left = y->right ;
          y->right = x ;
          return y ;
      } // AVLtree::rotateLL()

      AVLinfoTypePtr rotateRR( AVLinfoTypePtr x ) { // rotate x with its right child
          AVLinfoTypePtr y = x->right ;
          x->right = y->left ;
          y->left = x ;
          return y ;
      } // AVLtree::rotateRR()

      AVLinfoTypePtr rotateLR( AVLinfoTypePtr x ) { // rotate x with its right child
          AVLinfoTypePtr y = x->left ;
          AVLinfoTypePtr z = y->right ;
          // RR rotation on y
          y->right = z->left ;
          z->left = y ;
          x->left = z ;
          // LL rotation on x
          x->left = z->right ;
          z->right = x ;
          return z ;
      } // AVLtree::rotateLR()

      AVLinfoTypePtr rotateRL( AVLinfoTypePtr x ) { // rotate x with its left child
          AVLinfoTypePtr y = x->right ;
          AVLinfoTypePtr z = y->left ;
          // LL rotation on y
          y->left = z->right ;
          z->right = y ;
          x->right = z ;
          // RR rotation on x
          x->right = z->left ;
          z->left = x ;
          return z ;
      } // AVLtree::rotateRL()

};

void UserInfor() { // Information for user
    cout << "\n*** Search Tree Utilities **\n" ;
    cout << "* 0. QUIT                  *\n" ;
    cout << "* 1. Build 2-3 tree        *\n" ;
    cout << "* 2. Build AVL tree        *\n" ;
    cout << "****************************\n" ;
    cout << "Input a choice(0, 1, 2): " ;
} // UserInfor()

static Reader reader = Reader();

void creat23tree( Twothreetree tree, Reader reader ) {
	readType temp ;
	for ( int i = 0 ; i < reader.getsize() ; i++ ) {
		//cout << data.at(i).name << "++" << endl ;
		temp = reader.getdata(i) ;
		tree.insert23tree( temp.id, temp.name ) ;
	}	
    cout << "Tree height = " << tree.getheight() << endl ;
	cout << "Number of nodes = " << tree.getnum() << endl ;	
	int num = 1;
	if (tree.getnum() == 0 ) {
		cout << "No data" ;
	}
	else {
		for( int i = 0 ; i < KEY_NUM ; i++ ) {
			int a = i ;
			if( tree.getrootid(0,0) > tree.getrootid(1,0) ) // id由小到大印(判斷印0或1) 
				a = 1 - i ;
			for( int j = 0 ; j < tree.getrootsize( a ) ; j++ ) {
				int idx = tree.getrootid(a,j) ;
				cout << num << ": [" << idx << "] " << reader.getdata(idx-1).info << endl ;	
				num++;	
			}
			//cout << endl ;
		}
	}

}

void creatavltree( AVLtree atree, Reader reader, AVLinfoTypePtr root ) {
	readType temp ;
	for ( int i = 0 ; i < reader.getsize() ; i++ ) {
		temp = reader.getdata(i) ;
		atree.insertAVLtree( root, temp.granum, temp.id ) ;
	} // for
	cout << "Tree height = " << atree.getHeight( root ) << endl ;
	cout << "Number of nodes = " << atree.getnodenum( root ) << endl ;
	int num = 1 ;
	for ( int i = 0 ; i < atree.getrootsize( root ) ; i++ ){
		cout << num << ": [" << atree.getrootid(i, root ) << "] " << reader.getdata(atree.getrootid(i, root)-1).info << endl ;
		num++ ;
	} // for
	root = NULL ;

}

int main()
{
    Twothreetree tree = Twothreetree() ;
    AVLtree atree = AVLtree() ;
    AVLinfoTypePtr root = NULL ;

    bool build23 = false ;

    string cmd ;

    UserInfor() ;
    cin >> cmd ;
    while( cmd != "0" ) {
        if ( cmd == "1" ) {
        	reader.initial() ;
            cout << endl << "Input a file number ([0] Quit): " ;
            cin >> cmd ;
            if ( cmd != "0" ) {
                while ( !reader.Read( cmd ) ) { // fail to read file
                    cout << "Input a file number ([0] Quit): " ;
                    cin >> cmd ;
                    if ( cmd == "0" )
                        break ;
                } // while
                if ( cmd != "0" ) { // mission1
                	
                	tree.initial() ;
                	creat23tree(tree, reader) ;

					cout << endl ;
					build23 = true ;
                } // if
            } // if
        } // if
        else if ( cmd == "2" ) {

            if ( build23 ) { // mission2
	            creatavltree(atree, reader, root) ;
	            cout << endl ;
            } // if
            else{
            	cout << "### Choose 1 first. ###" << endl ;
			} // else
        } // else if
        else {
            cout << endl << "Command does not exist!" << endl ;
        } // else

        UserInfor() ;
        cin >> cmd ;

    } // while
}

