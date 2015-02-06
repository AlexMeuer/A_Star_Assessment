#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <queue>
#include <utility> // for STL pair


using namespace std;

template <class NodeType, class ArcType> class GraphArc;
template <class NodeType, class ArcType> class GraphNode;


template<class NodeType, class ArcType>
class NodeSearchCostComparer {

public:
	bool operator()(GraphNode<NodeType, ArcType> * n1, GraphNode<NodeType, ArcType> * n2) {
		return n1->data().second > n2->data().second;
	}
};

template<class NodeType, class ArcType>
class NodeHeuristicCostComparer {

public:
	bool operator()(GraphNode<NodeType, ArcType> * n1, GraphNode<NodeType, ArcType> * n2) {
		cout << "cmp | " << n1->data().first << "["<<n1->F_Value<<"]" << " vs " << n2->data().first << "["<<n1->F_Value<<"]" << endl;
		return n1->F_Value < n2->F_Value;
	}
};


// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    This is the graph class, it contains all the
//                  nodes.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
class Graph {
private:

    // typedef the classes to make our lives easier.
    typedef GraphArc<NodeType, ArcType> Arc;
    typedef GraphNode<NodeType, ArcType> Node;

// ----------------------------------------------------------------
//  Description:    An array of all the nodes in the graph.
// ----------------------------------------------------------------
    Node** m_pNodes;

// ----------------------------------------------------------------
//  Description:    The maximum number of nodes in the graph.
// ----------------------------------------------------------------
    int m_maxNodes;


// ----------------------------------------------------------------
//  Description:    The actual number of nodes in the graph.
// ----------------------------------------------------------------
    int m_count;


public:           
    // Constructor and destructor functions
    Graph( int size );
    ~Graph();

    // Accessors
    Node** nodeArray() const {
       return m_pNodes;
    }

    // Public member functions.
	bool addNode( NodeType data, int index, sf::Font const &font );
    void removeNode( int index );
    bool addArc( int from, int to, ArcType weight );
	bool addDualArc( int from, int to, ArcType weight, int startX, int startY, int endX, int endY, sf::Font const &font );
    void removeArc( int from, int to );
    Arc* getArc( int from, int to );        
    void clearMarks();
    void depthFirst( Node* pNode, void (*pProcess)(Node*) );
    void breadthFirst( Node* pNode, void (*pProcess)(Node*) );
	void breadthFirstSearch( Node* pNode, void (*pProcess)(Node*), Node* nodeToFind);
	void ucs( Node* pStart, Node* pDest, void (*pVisitFunc)(Node*),std::vector<Node *>& path );
	void aStar( Node* pStart, Node* pDest, void (*pProcess)(Node*), std::vector<Node *>& path );
	ArcType heuristic_eval( Node* A, Node* B, float grainOfSalt = 0.9f);
	int getTotalNodes();
	void resetMarked();
};

// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    Constructor, this constructs an empty graph
//  Arguments:      The maximum number of nodes.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::Graph( int size ) : m_maxNodes( size ) {
   int i;
   m_pNodes = new Node * [m_maxNodes];
   // go through every index and clear it to null (0)
   for( i = 0; i < m_maxNodes; i++ ) {
        m_pNodes[i] = 0;
   }

   // set the node count to 0.
   m_count = 0;
}

// ----------------------------------------------------------------
//  Name:           ~Graph
//  Description:    destructor, This deletes every node
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::~Graph() {
   int index;
   for( index = 0; index < m_maxNodes; index++ ) {
        if( m_pNodes[index] != 0 ) {
            delete m_pNodes[index];
        }
   }
   // Delete the actual array
   delete m_pNodes;
}

// ----------------------------------------------------------------
//  Name:           addNode
//  Description:    This adds a node at a given index in the graph.
//  Arguments:      The first parameter is the data to store in the node.
//                  The second parameter is the index to store the node.
//  Return Value:   true if successful
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addNode( NodeType data, int index, sf::Font const &font ) {
   bool nodeNotPresent = false;
   // find out if a node does not exist at that index.
   if ( m_pNodes[index] == 0) {
      nodeNotPresent = true;
      // create a new node, put the data in it, and unmark it.
      m_pNodes[index] = new Node(font);
      m_pNodes[index]->setData(data);
      m_pNodes[index]->setMarked(false);

      // increase the count and return success.
      m_count++;
    }
        
    return nodeNotPresent;
}

// ----------------------------------------------------------------
//  Name:           removeNode
//  Description:    This removes a node from the graph
//  Arguments:      The index of the node to return.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeNode( int index ) {
     // Only proceed if node does exist.
     if( m_pNodes[index] != 0 ) {
         // now find every arc that points to the node that
         // is being removed and remove it.
         int node;
         Arc* arc;

         // loop through every node
         for( node = 0; node < m_maxNodes; node++ ) {
              // if the node is valid...
              if( m_pNodes[node] != 0 ) {
                  // see if the node has an arc pointing to the current node.
                  arc = m_pNodes[node]->getArc( m_pNodes[index] );
              }
              // if it has an arc pointing to the current node, then
              // remove the arc.
              if( arc != 0 ) {
                  removeArc( node, index );
              }
         }
        

        // now that every arc pointing to the current node has been removed,
        // the node can be deleted.
        delete m_pNodes[index];
        m_pNodes[index] = 0;
        m_count--;
    }
}

// ----------------------------------------------------------------
//  Name:           addArd
//  Description:    Adds an arc from the first index to the 
//                  second index with the specified weight.
//  Arguments:      The first argument is the originating node index
//                  The second argument is the ending node index
//                  The third argument is the weight of the arc
//  Return Value:   true on success.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addArc( int from, int to, ArcType weight ) {
     bool proceed = true; 
     // make sure both nodes exist.
     if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
         proceed = false;
     }
        
     // if an arc already exists we should not proceed
     if( m_pNodes[from]->getArc( m_pNodes[to] ) != 0 ) {
         proceed = false;
     }

     if (proceed == true) {
        // add the arc to the "from" node.
        m_pNodes[from]->addArc( m_pNodes[to], weight );
     }
        
     return proceed;
}

template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addDualArc( int from, int to, ArcType weight, int startX, int startY, int endX, int endY, sf::Font const &font ) {
     bool proceed = true; 
     // make sure both nodes exist.
     if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
         proceed = false;
     }
        
     // if an arc already exists we should not proceed
     if( m_pNodes[from]->getArc( m_pNodes[to] ) != 0 ) {
         proceed = false;
     }

     if (proceed == true) {
        // add the arc to the "from" node.
        m_pNodes[from]->addArc( m_pNodes[to], weight, startX, startY, endX, endY, font);
		m_pNodes[to]->addArc( m_pNodes[from], weight, startX, startY, endX, endY, font);
		m_pNodes[from]->setPosition(startX, startY);
		m_pNodes[to]->setPosition(endX, endY);
     }
        
     return proceed;
}

// ----------------------------------------------------------------
//  Name:           removeArc
//  Description:    This removes the arc from the first index to the second index
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeArc( int from, int to ) {
     // Make sure that the node exists before trying to remove
     // an arc from it.
     bool nodeExists = true;
     if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
         nodeExists = false;
     }

     if (nodeExists == true) {
        // remove the arc.
        m_pNodes[from]->removeArc( m_pNodes[to] );
     }
}


// ----------------------------------------------------------------
//  Name:           getArc
//  Description:    Gets a pointer to an arc from the first index
//                  to the second index.
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   pointer to the arc, or 0 if it doesn't exist.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
// Dev-CPP doesn't like Arc* as the (typedef'd) return type?
GraphArc<NodeType, ArcType>* Graph<NodeType, ArcType>::getArc( int from, int to ) {
     Arc* pArc = 0;
     // make sure the to and from nodes exist
     if( m_pNodes[from] != 0 && m_pNodes[to] != 0 ) {
         pArc = m_pNodes[from]->getArc( m_pNodes[to] );
     }
                
     return pArc;
}


// ----------------------------------------------------------------
//  Name:           clearMarks
//  Description:    This clears every mark on every node.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::clearMarks() {
     int index;
     for( index = 0; index < m_maxNodes; index++ ) {
          if( m_pNodes[index] != 0 ) {
              m_pNodes[index]->setMarked(false);
			  m_pNodes[index]->setColor(0,0,255);
          }
     }
}


// ----------------------------------------------------------------
//  Name:           depthFirst
//  Description:    Performs a depth-first traversal on the specified 
//                  node.
//  Arguments:      The first argument is the starting node
//                  The second argument is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::depthFirst( Node* pNode, void (*pProcess)(Node*) ) {
     if( pNode != 0 ) {
           // process the current node and mark it
           pProcess( pNode );
           pNode->setMarked(true);

           // go through each connecting node
           list<Arc>::iterator iter = pNode->arcList().begin();
           list<Arc>::iterator endIter = pNode->arcList().end();
        
		   for( ; iter != endIter; ++iter) {
			    // process the linked node if it isn't already marked.
                if ( (*iter).node()->marked() == false ) {
                   depthFirst( (*iter).node(), pProcess);
                }            
           }
     }
}


// ----------------------------------------------------------------
//  Name:           breadthFirst
//  Description:    Performs a depth-first traversal the starting node
//                  specified as an input parameter.
//  Arguments:      The first parameter is the starting node
//                  The second parameter is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::breadthFirst( Node* pNode, void (*pProcess)(Node*) ) {
   if( pNode != 0 ) {
	  queue<Node*> nodeQueue;        
	  // place the first node on the queue, and mark it.
      nodeQueue.push( pNode );
      pNode->setMarked(true);

      // loop through the queue while there are nodes in it.
      while( nodeQueue.size() != 0 ) {
         // process the node at the front of the queue.
         pProcess( nodeQueue.front() );

         // add all of the child nodes that have not been 
         // marked into the queue
         list<Arc>::const_iterator iter = nodeQueue.front()->arcList().begin();
         list<Arc>::const_iterator endIter = nodeQueue.front()->arcList().end();
         
		 for( ; iter != endIter; iter++ ) {
              if ( (*iter).node()->marked() == false) {
				 // mark the node and add it to the queue.
                 (*iter).node()->setMarked(true);
                 nodeQueue.push( (*iter).node() );
              }
         }

         // dequeue the current node.
         nodeQueue.pop();
      }
   }  
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::breadthFirstSearch( Node* pNode, void (*pProcess)(Node*) , Node* nodeToFind){
	bool goalReached = false;
	if( pNode != 0 ) {
	  queue<Node*> nodeQueue;        
	  // place the first node on the queue, and mark it.
      nodeQueue.push( pNode );

      pNode->setMarked(true);

      // loop through the queue while there are nodes in it.
      while((nodeQueue.size() != 0) && (goalReached == false)) {
         // process the node at the front of the queue.
         pProcess( nodeQueue.front() );

         // add all of the child nodes that have not been 
         // marked into the queue
         list<Arc>::const_iterator iter = nodeQueue.front()->arcList().begin();
         list<Arc>::const_iterator endIter = nodeQueue.front()->arcList().end();
         
		 for( ; (iter != endIter) && (goalReached == false); iter++ ) {
			  if((*iter).node() == nodeToFind){
				  cout << "found"<<endl;
				  goalReached = true;
				  (*iter).node()->setPreviousPointer(nodeQueue.front());
			  }
              if ( (*iter).node()->marked() == false) {
				  (*iter).node()->setPreviousPointer(nodeQueue.front());
				 // mark the node and add it to the queue.
                 (*iter).node()->setMarked(true);
                 nodeQueue.push( (*iter).node() );
              }

         }

         // dequeue the current node.
         nodeQueue.pop();
      }



   }  

}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::ucs( Node* pStart, Node* pDest, void (*pVisitFunc)(Node*),std::vector<Node *>& path ){
	cout << "\Commencing UCS..." << endl;

	//Let pq = a new priority queue
	priority_queue<Node*, vector<Node*>, NodeSearchCostComparer<NodeType, ArcType>> pq;

	//For each node v in graph G
	for(int i = 0; (i != m_maxNodes) ; i++ ){

	//	Initialise d[v] to infinity // don’t yet know the distances to these nodes
		m_pNodes[i]->setData(pair<string, int>(m_pNodes[i]->data().first, INT_MAX));
	}

	//Initialise d[s] to 0
	pStart->setData(pair<string, int>(pStart->data().first, 0));

	//Add s to the pq
	pq.push(pStart);

	//Mark(s)
	pq.top()->setMarked(true);

	//While the queue is not empty AND pq.top() != g
	while(!pq.empty() && (pq.top() != pDest)){
	//	For each child node c of pq.top()
		 list<Arc>::const_iterator itr = pq.top()->arcList().begin();
         list<Arc>::const_iterator endItr = pq.top()->arcList().end();

		 pVisitFunc(pq.top());

		 for( ; (itr != endItr) ; itr++ ) {
			

			//If (c != previous(pq.top()) // don’t go back to a predecessor
			if(itr->node() != pq.top()->getPrevious()){

				//Let distC = (pq.top(), c) + d[pq.top] 
				ArcType distC = pq.top()->getArc(itr->node())->weight()  + pq.top()->data().second;

				//If ( distC < d[c] )
				if(distC < itr->node()->data().second){
					//let d[c] = distC
					itr->node()->setData(pair<string, int>(itr->node()->data().first, distC));
					//Set previous pointer of c to pq.top()
					itr->node()->setPrevious(pq.top());
				}
				//If (notMarked(c))
				if(itr->node()->marked() != true){
					//Add c to the pq 
					pq.push(itr->node());
					//Mark(c)
					itr->node()->setMarked(true);
					itr->node()->setColor(100,100,100);
				}
			}
		}
	//   Remove pq.top()
		pq.pop();
	}

	//add to vector
	while (pDest->getPrevious() != NULL)
	{
		path.push_back(pDest);
		pDest = pDest->getPrevious();
	}
	path.push_back(pDest);

	cout << "\nFinished UCS." << endl;
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::aStar( Node* pStart, Node* pDest, void (*pProcess)(Node*), std::vector<Node *>& path ){
	cout << "\Commencing A*..." << endl;
	//Let s = the starting node
	//Let pq = a new priority queue
	//priority_queue<Node*, vector<Node*>, NodeHeuristicCostComparer<NodeType, ArcType>> pq;
	list<Node*> nodeList;

	//For each node v in graph G
	for(int i = 0; i != m_maxNodes; i++) {
		 m_pNodes[i]->F_Value =  INT_MAX / 2;
	}

	pStart->H_Value = INT_MAX / 2;

	//Add s to the pq
	nodeList.push_front(pStart);

	//Mark(s)
	nodeList.front()->setMarked(true);
	Node* pqTopAtStartOfWhileLoop = NULL;
	
	//While the queue is not empty AND pq.top() != g
	while(nodeList.empty() != true && nodeList.front() != pDest) {
		cout << "\n\tFrom " << nodeList.front()->data().first << ":" <<endl;
		pqTopAtStartOfWhileLoop = nodeList.front();

		//For each child node c of pq.top()
		//auto end = pq.top()->arcList().end();
		//for(auto itr = pq.top()->arclist().begin(); itr != end; itr++){
		 list<Arc>::const_iterator iter = nodeList.front()->arcList().begin();
         list<Arc>::const_iterator endIter = nodeList.front()->arcList().end();

		 for( ; (iter != endIter) ; iter++ ) {
			//If (c != previous(pq.top())
			 Node* node = iter->node();

			if(node != nodeList.front()->getPrevious()) {
				//Let distC = h(c) + g(c) // g(c) is actual path cost to child
				ArcType hC = heuristic_eval(node, pDest);
				ArcType distC = hC + node->data().second;
				
				node->H_Value = hC;
				node->setColor(100,100,100);

				//If ( distC < g(c) )
				 if(distC < node->F_Value){

					//let f[c] = distC
					 node->F_Value = distC;

					//Set previous pointer of c to pq.top()
					node->setPrevious(nodeList.front());
					//make_heap(const_cast<Node**>(&pq.top()), const_cast<Node**>(&pq.top()) + pq.size(), NodeHeuristicCostComparer<NodeType, ArcType>());
				 }//End if

				//If (notMarked(c))
				if(!node->marked()) {
					pProcess(node);
					//Add c to the pq
					nodeList.push_back(node);
					//Mark(c)
					node->setMarked(true);
				}//End if
			}//End if
		}//End for

		//Remove pq.top()
		 _ASSERT(nodeList.front() == pqTopAtStartOfWhileLoop);
		nodeList.pop_front();
		nodeList.sort(NodeHeuristicCostComparer<NodeType, ArcType>());
		//if(!pq.empty())
		//	make_heap(const_cast<Node**>(&pq.top()), const_cast<Node**>(&pq.top()) + pq.size(), NodeHeuristicCostComparer<NodeType, ArcType>());

	}//End while


	//_ASSERT(!nodeList.empty());
	if(nodeList.empty())
		cout << "\a\a\aNode List is empty. Did you remember to clear the marks?" <<endl;

		//get the best path back to the start
	for( Node* node = pDest; node != pStart; node = node->getPrevious() ) {
		path.push_back(node);
	}
	path.push_back(pStart);

	cout << "\nFinished A*." << endl;
}

template<class NodeType, class ArcType>
ArcType Graph<NodeType, ArcType>::heuristic_eval( Node* A, Node* B, float grainOfSalt) {
	sf::Vector2f aPos = A->getPosition(), bPos = B->getPosition();
	
	ArcType result = sqrt(pow(bPos.x - aPos.x, 2) + pow(bPos.y - aPos.y, 2));

	return result * grainOfSalt;
}

template<class NodeType, class ArcType>
int Graph<NodeType, ArcType>::getTotalNodes(){
	return m_count;
}


template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::resetMarked(){
	for(int i = 0; i != m_maxNodes; i++) {
		 m_pNodes[i]->setColor(0,125,0);
		 m_pNodes[i]->setMarked(false);
	}
}


#include "GraphNode.h"
#include "GraphArc.h"


#endif
