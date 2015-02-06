#ifndef GRAPHARC_H
#define GRAPHARC_H

#include "GraphNode.h"
#include <sstream>

// -------------------------------------------------------
// Name:        GraphArc
// Description: This is the arc class. The arc class
//              points to a graph node, and contains a 
//              weight.
// -------------------------------------------------------
template<class NodeType, class ArcType>
class GraphArc {
private:

// -------------------------------------------------------
// Description: pointer to the node that the arc points to
// -------------------------------------------------------
    GraphNode<NodeType, ArcType>* m_pNode;

// -------------------------------------------------------
// Description: Weight of the arc
// -------------------------------------------------------
    ArcType m_weight;

	sf::Text m_text;

	//start and end of the arc
	sf::Vertex m_line[2];

	//buffer for converting integer to string
	char buffer[4];
	
	//start and end positions of arc
	//int m_startX, m_startY, m_endX, m_endY;

public:   

	GraphArc(Font const &font) {
		m_text.setFont(font);
		m_text.setCharacterSize(8);
	}
    
    // Accessor functions
    GraphNode<NodeType, ArcType>* node() const {
        return m_pNode;
    }
                              
    ArcType weight() const {
        return m_weight;
    }
    
    // Manipulator functions
    void setNode(GraphNode<NodeType, ArcType>* pNode) {
       m_pNode = pNode;
    }
    
    void setWeight(ArcType weight) {
		
       m_weight = weight;
	   
    }

	void setStartEnd(int startX, int startY, int endX, int endY) {
		m_line[0] = sf::Vertex(sf::Vector2f( startX, startY ));
		m_line[1] = sf::Vertex(sf::Vector2f( endX, endY ));
		m_text.setPosition((startX + endX)/2.0f, (startY + endY)/2.0f);
    }




	void DrawArc(sf::RenderWindow & const w){
		//draw the arc line
		w.draw(m_line, 2, sf::Lines);

		//draw the arc text
		_itoa_s(m_weight,buffer,sizeof(buffer),10);
		m_text.setString(buffer);
		w.draw(m_text);
			
	}
    
};

#endif
