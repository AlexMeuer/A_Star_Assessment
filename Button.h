#ifndef _BUTTON_H
#define _BUTTON_H
#include "SFML\Graphics.hpp"

using namespace sf;

class Button {
private:
	RectangleShape m_rect;
	Text m_text;
	//void(*pFunction)(void);

public:
	Button(String const &label, Font const &font, Vector2f const &position, Vector2f const &size, Color const &outline = Color::Blue, Color const &fill = Color(0, 100, 0, 255), Color const &labelColor = Color::Yellow) {
		//pFunction = pTargetFunction;

		m_text = Text(label, font, 20U);
		m_text.setPosition(position);
		m_text.setColor(labelColor);

		m_rect = RectangleShape(size);
		m_rect.setPosition(position);
		m_rect.setOutlineColor(outline);
		m_rect.setOutlineThickness(5);
		m_rect.setFillColor(fill);
	}

	void Draw(RenderWindow &w) {
		w.draw(m_rect);
		w.draw(m_text);
	}

	//checks if a point is withing the button bounds.
	//if param callFunction is true (default) the buttons function pointer will be invoked.
	bool containsPoint(int x, int y, bool callFunction = true) {
		Vector2f position = m_rect.getPosition();
		Vector2f size = m_rect.getSize();

		bool containsX = (x > position.x) && (x < position.x + size.x);
		bool containsY = (y > position.y) && (y < position.y + size.y);

		return containsX && containsY;
	}
};
#endif