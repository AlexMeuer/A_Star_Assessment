#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <list>

using namespace sf;

// Forward references
template <typename NodeType, typename ArcType> class GraphArc;

// -------------------------------------------------------
// Name:        GraphNode
// Description: This is the node class. The node class 
//              contains data, and has a linked list of 
//              arcs.
// -------------------------------------------------------
template<class NodeType, class ArcType>
class GraphNode {
private:    
// typedef the classes to make our lives easier.
    typedef GraphArc<NodeType, ArcType> Arc;
    typedef GraphNode<NodeType, ArcType> Node;
// -------------------------------------------------------
// Description: data inside the node
// -------------------------------------------------------
	NodeType m_data;

// -------------------------------------------------------
// Description: list of arcs that the node has.
// -------------------------------------------------------
    list<Arc> m_arcList;

// -------------------------------------------------------
// Description: This remembers if the node is marked.
// -------------------------------------------------------
    bool m_marked;

	//Pointer to previous node
	Node* m_previousNode;


	//Graphics variables
	Text m_text;
	CircleShape m_circle;

public:
	//constructor
	GraphNode<NodeType, ArcType>::GraphNode( Font const &font, unsigned int radius = 25U ) {
		m_previousNode = NULL;

		//setup circle
		m_circle.setOrigin(radius, radius);
		m_circle.setRadius(radius);
		m_circle.setFillColor(Color::Blue);

		//setup text
		m_text.setFont(font);
		m_text.setCharacterSize(20U);
	}

    // Accessor functions
    list<Arc> const & arcList() const {
        return m_arcList;              
    }

	Vector2f getPosition() const {
		return m_circle.getPosition();
	}

    bool marked() const {
        return m_marked;
    }

	NodeType const & data() const {
        return m_data;
    }

    // Manipulator functions
	void setData(NodeType data) {
        m_data = data;
		m_text.setString(m_data.first);
    }
    
	NodeType data() {
        return m_data;
    }

    void setMarked(bool mark) {
        m_marked = mark;
    }

	void setPrevious(Node* value) {
        m_previousNode = value;
    }

	Node* getPrevious() {
        return m_previousNode;
    }

    Arc* getArc( Node* pNode );    
    void addArc( Node* pNode, ArcType pWeight, int startX, int startY, int endX, int endY, Font const &font);
    void removeArc( Node* pNode );

	void drawSelf(sf::RenderWindow &w);

	void drawArcs(sf::RenderWindow &w);

	void setPosition(int x, int y);

	ArcType H_Value;
	ArcType F_Value;

	void setColor(float r, float g, float b);

	bool containsPoint(int x, int y);

};

// ----------------------------------------------------------------
//  Name:           getArc
//  Description:    This finds the arc in the current node that
//                  points to the node in the parameter.
//  Arguments:      The node that the arc connects to.
//  Return Value:   A pointer to the arc, or 0 if an arc doesn't
//                  exist from this to the specified input node.
// ----------------------------------------------------------------
template<typename NodeType, typename ArcType>
GraphArc<NodeType, ArcType>* GraphNode<NodeType, ArcType>::getArc( Node* pNode ) {

     list<Arc>::iterator iter = m_arcList.begin();
     list<Arc>::iterator endIter = m_arcList.end();
     Arc* pArc = 0;
     
     // find the arc that matches the node
     for( ; iter != endIter && pArc == NULL; ++iter ) {         
          if ( iter->node() == pNode) {
               pArc = &( (*iter) );
          }
     }

     // returns null if not found
     return pArc;
}


// ----------------------------------------------------------------
//  Name:           addArc
//  Description:    This adds an arc from the current node pointing
//                  to the first parameter, with the second parameter 
//                  as the weight.
//  Arguments:      First argument is the node to connect the arc to.
//                  Second argument is the weight of the arc.
//  Return Value:   None.
// ----------------------------------------------------------------
template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::addArc( Node* pNode, ArcType weight, int startx, int starty, int endx, int endy, sf::Font const &font) {
   
	// Create a new arc.
   Arc a(font);
   a.setNode(pNode);
   a.setWeight(weight);   
   a.setStartEnd(startx, starty, endx, endy);
   
   // Add it to the arc list.
   m_arcList.push_back( a );
}


// ----------------------------------------------------------------
//  Name:           removeArc
//  Description:    This finds an arc from this node to input node 
//                  and removes it.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::removeArc( Node* pNode ) {
	list<Arc>::iterator iter = m_arcList.begin();
	list<Arc>::iterator endIter = m_arcList.end();

     int size = m_arcList.size();
     // find the arc that matches the node
     for( ; iter != endIter && m_arcList.size() == size; ++iter ) {
          if ( iter->node() == pNode) {
             m_arcList.remove( (*iter) );
          }                           
     }
}



template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::drawArcs(sf::RenderWindow & const w){
	list<Arc>::iterator iter = m_arcList.begin();
    list<Arc>::iterator endIter = m_arcList.end();
    for( ; iter != endIter; ++iter ) {
		iter->DrawArc(w);                     
    }
}


template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::drawSelf(sf::RenderWindow & const w){
	w.draw(m_circle);
	w.draw(m_text);
}

template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::setPosition(int x, int y){
	m_circle.setPosition(x, y);
	m_text.setPosition(x - m_circle.getRadius()/2.0f, y - m_circle.getRadius()/2.0f);
}

template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::setColor(float r, float g, float b){
	m_circle.setFillColor(sf::Color(r, g, b, 255));
}

template<typename NodeType, typename ArcType>
bool GraphNode<NodeType, ArcType>::containsPoint(int x, int y) {
	Vector2f pos = m_circle.getPosition();

	//return true if the point is closer (euclidean space) than the radius of the circle
	return sqrtf(powf(x - pos.x, 2) + powf(y - pos.y, 2)) < m_circle.getRadius();
}

#include "GraphArc.h"

#endif
