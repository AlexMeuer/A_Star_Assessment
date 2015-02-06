////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#ifdef _DEBUG
#pragma comment(lib,"sfml-graphics-d.lib")
#pragma comment(lib,"sfml-audio-d.lib")
#pragma comment(lib,"sfml-system-d.lib")
#pragma comment(lib,"sfml-window-d.lib")
#pragma comment(lib,"sfml-network-d.lib")
#else
#pragma comment(lib,"sfml-graphics.lib")
#pragma comment(lib,"sfml-audio.lib")
#pragma comment(lib,"sfml-system.lib")
#pragma comment(lib,"sfml-window.lib")
#pragma comment(lib,"sfml-network.lib")
#endif
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#include "SFML/Graphics.hpp"
#include <iostream>
#include <fstream>

#include "Graph.h"
#include "Button.h"

#include <string>

using namespace std;
using std::pair;

typedef Graph<char, int> GraphType;

// Create the main window
sf::RenderWindow window(sf::VideoMode(800, 600, 32), "SFML First Program");
//typedef GraphArc<char, int> Arc;
//typedef GraphNode<char, int> Node;
typedef GraphArc<string, int> Arc;
typedef GraphNode<pair<string, int>, int> Node;
typedef vector<Node*> Path;

void visitFunc(Node * pNode) {
	cout << "Visiting | " << pNode->data().first << endl;
	pNode->setColor(0,100,0);
}

//outputs path to console and clears the container if param clear == true
void outputPath(vector<Node*> &path, bool clear = true) {
	cout << "PATH: " << endl;

	for(int i = 0; i < path.size(); i++) {
		path[i]->setColor(200,0,0);
		cout << path[i]->data().first << endl;
	}

	if(clear)
		path.clear();
}


int main(int argc, char *argv[]) {
	int destNode = 5, startNode = 0; 

	sf::Font mainFont;
	if(!mainFont.loadFromFile("kenvector_future.ttf"))
		return EXIT_FAILURE;

	Button runASTAR_Button("A*", mainFont, Vector2f(650, 100), Vector2f(50,50)),
		runUCS_Button("UCS", mainFont, Vector2f(650, 200), Vector2f(50,50)),
		reset_Button("RESET", mainFont, Vector2f(650, 300), Vector2f(100,50));


	//create graph
    Graph<pair<string, int>, int> graph( 30 );
	
	//read nodes
	pair<string, int> c("", 0);

	int i = 0;
	ifstream myfile;
	//cout << "before read";
	myfile.open("nodes.txt");

	while (myfile >> c.first) {
		graph.addNode(c, i++, mainFont);
	}

	myfile.close();

	//read arcs
	myfile.open("arcs.txt");

	int from, to, weight, startX, startY, endX, endY;
	while ( myfile >> from >> to >> weight >> startX >> startY >> endX >> endY) {
		graph.addDualArc(from, to, weight, startX, startY, endX, endY, mainFont);
	}

    myfile.close();

	cout << "\aLeft Click sets starting node!\nRight Click sets destination node!"<<endl;
	cout << "-----------------------------\n[1]Run UCS first.\n[2]Hit reset to clear the colours.\n[3]Run A*.\n[4]Give marks\n-----------------------------"<<endl;
	cout << "\tColour Key\nBlue\t|\tUntouched - algorithm has not touched this node at all.\nRed\t|\tPath - node is part of the path found"<<endl;
	cout << "Gray\t|\tProbed - node's heuristic was checked.\nGreen\t|\tChecked - node was travelled to and all its arcs were checked."<<endl;
	cout << "All green nodes were once gray nodes. Gray may overwrite green and vice versa\n\a"<<endl;


	vector<Node*> path;
	graph.clearMarks();

	
	// Start game loop
	while (window.isOpen())
	{
		// Process events
		sf::Event Event;
		while (window.pollEvent(Event))
		{
			// Close window : exit
			if (Event.type == sf::Event::Closed)
				window.close();

			// Escape key : exit
			else if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape))
				window.close();

			//Clear marks
			else if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::R))
				graph.clearMarks();

			//Run A*
			else if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::A)){
				_ASSERT(path.empty());
				graph.aStar(graph.nodeArray()[startNode], graph.nodeArray()[destNode], visitFunc, path);
				outputPath(path);
			}
			
			else if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::U)){
				//_ASSERT(path.empty());
				graph.ucs(graph.nodeArray()[startNode], graph.nodeArray()[destNode], visitFunc, path);
				outputPath(path);
			}

			else if (Event.type == sf::Event::MouseButtonPressed) {

			   sf::Vector2i mousePos = sf::Mouse::getPosition(window);

#pragma region Button Click Checks
			   //check mouse click on buttons
			   if(runUCS_Button.containsPoint(mousePos.x, mousePos.y)) {
				   graph.ucs(graph.nodeArray()[startNode], graph.nodeArray()[destNode], visitFunc, path);
				   outputPath(path);
			   }
			   else if(runASTAR_Button.containsPoint(mousePos.x, mousePos.y)) {
				   graph.aStar(graph.nodeArray()[startNode], graph.nodeArray()[destNode], visitFunc, path);
				   outputPath(path);
			   }
			   else if(reset_Button.containsPoint(mousePos.x, mousePos.y)) {
				   graph.clearMarks();
			   }  
#pragma endregion


#pragma region Node Click Checks
			   for (int i = 0; i < graph.getTotalNodes(); ++i){

				   if(graph.nodeArray()[i]->containsPoint(mousePos.x, mousePos.y)){

					   if (Event.mouseButton.button == sf::Mouse::Left) {
						   startNode = i;
						   cout << "\aStarting node set | " << graph.nodeArray()[i]->data().first << endl;
						   break;
					   }
					   else if(Event.mouseButton.button == sf::Mouse::Right) {
						   destNode = i;
						   cout << "\aDestination node set | " << graph.nodeArray()[i]->data().first << endl;
						   break;
					   }
				   }
			   }
			}  
#pragma endregion
				
		}
		//prepare frame
		window.clear();

		//draw nodes and arcs (TODO: don't draw reverse arcs)
		for (int i = 0; i < graph.getTotalNodes(); ++i) {
			graph.nodeArray()[i]->drawArcs(window);
		}
		for (int i = 0; i < graph.getTotalNodes(); ++i) {
			graph.nodeArray()[i]->drawSelf(window);
		}

		runASTAR_Button.Draw(window);
		runUCS_Button.Draw(window);
		reset_Button.Draw(window);

		window.display();
	} //loop back for next frame
	return EXIT_SUCCESS;
}