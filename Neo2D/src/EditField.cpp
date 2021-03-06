#include <EditField.h>
#include <Neo2DLevel.h>
#include <utf8.h>

using namespace Neo2D;
using namespace Gui;
using namespace Neo;

// Can't test rendering
// LCOV_EXCL_START
class EditFieldTheme : public Neo2D::Gui::Theme
{
	OText* text;
public:

	EditFieldTheme()
	{
		text = Neo2D::Neo2DLevel::getInstance()->createText("assets/default.ttf", 12);
		text->setColor(Vector4(0,0,0,1));
	}

	virtual void draw(Neo2D::Gui::Widget* widget, const Neo::Vector2& offset)
	{
		NeoEngine* engine = NeoEngine::getInstance();
		Renderer* renderer = engine->getRenderer();
		EditField* edit = static_cast<EditField*>(widget);

		const Neo::Vector2 position = widget->getPosition() + offset + Vector2(2,0);
		Box3d* box = text->getBoundingBox();

		text->setText(widget->getLabel());
		Vector2 cursorPos = calculateCursorPos(text, edit->getCaret()); // + offset + edit->getPosition();
		float textOffset = cursorPos.x;

		// Calculate overflow offset
		if (textOffset <= edit->getSize().x) textOffset = 0.0f; // If we do not overflow, simply ignore
		else textOffset -= edit->getSize().x; // Calculate the new position relative to the right edge

		switch(widget->getState())
		{
			case Neo2D::Gui::WIDGET_HOVER:
			case Neo2D::Gui::WIDGET_NORMAL:
				/*renderer->drawColoredQuad(position - Vector2(1,1), widget->getSize(), Vector4(0.1, 0.1, 0.1, 1), 0);
				renderer->drawColoredQuad(position, widget->getSize() - Vector2(1,2), Vector4(1, 1, 1, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(1, 1, 1, 1), 0);*/

				renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.0, 0.0, 0.0, 1), 0);
				renderer->drawColoredQuad(position, widget->getSize() - Vector2(1, 1), Vector4(0.8, 0.8, 0.8, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1, 1),  widget->getSize() - Vector2(2, 2),
										  Vector4(0.3, 0.3, 0.3, 1),
										  0);
				renderer->drawColoredQuad(position + Vector2(1, 1),
										  widget->getSize() - Vector2(3, 3),
										  Vector4(1, 1, 1, 1),
										  0);
				break;

			case Neo2D::Gui::WIDGET_SELECTED:
			{
				renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.0, 0.0, 0.0, 1), 0);
				renderer->drawColoredQuad(position, widget->getSize() - Vector2(1, 1), Vector4(0.4, 0.4, 0.4, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1, 1),  widget->getSize() - Vector2(2, 2),
										  Vector4(0.3, 0.3, 0.3, 1),
										  0);
				renderer->drawColoredQuad(position + Vector2(1, 1),
										  widget->getSize() - Vector2(3, 3),
										  Vector4(1, 1, 1, 1),
										  0);

				Vector2 pos = cursorPos + offset + edit->getPosition();
				renderer->drawColoredQuad(Vector2(pos.x - textOffset + 3, pos.y + 0.25 * text->getSize()),
										  Vector2(2.0, 0.75*text->getSize()),
										  text->getColor() + Vector4(0.1,0.1,0.1,0.0), 0.0f);

			}
				break;

			default: break;
		}

		Vector2 screen = engine->getSystemContext()->getScreenSize();
		renderer->enableScissors(position.x, screen.y - (position.y + edit->getSize().y), edit->getSize().x, edit->getSize().y);

		renderer->drawText2D(text, position.x - textOffset,
							 position.y + 0.5 * edit->getSize().y,
							 0.0f);

		renderer->disableScissors();
	}

	Vector2 calculateCursorPos(OText* text, int pos) const
	{
		Vector2 length;
		Font* font = text->getFontRef()->getFont();
		map<unsigned int, Character>* chars = font->getCharacters();

		std::string s = text->getText();
		auto iter = s.begin();
		int i = 0;

		while(iter != s.end() && (i < pos || pos == -1))
		{
			const unsigned int character = utf8::next(iter, s.end());
			length.x += ((*chars)[character]).getXAdvance();

			if(character == '\n')
			{
				length.x = 0;
				length.y++;
			}
			i += EditField::characterSize(character);
		}

		return length * text->getSize();
	}
};
// LCOV_EXCL_STOP

/**
 * @brief Gets triggered each time a key gets pressed and repeats with the keyboard delay.
 */
class KeyRepeatEvent: public Event
{
	unsigned int m_key;
public:
	KeyRepeatEvent(std::weak_ptr<Neo2D::Gui::Widget> w, const function<void(Widget&, const Event &, void *)>& cb, void* d) :
		Event(w, cb, d),
		m_key(-1)
	{}

	virtual void update(float dt)
	{
		reject();

		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Keyboard& kbd = input->getKeyboard();

		for(unsigned int i = 0; i < kbd.getKeys().size(); i++)
		{
			// Only detect real keys
			if(kbd.onKeyDown(i))
			{
				m_key = i;
				handle();

				if(handled())
					kbd.keyUp(i);
			}
		}
	}

	/**
	 * @brief Retrieves the key ID.
	 * @return The key ID.
	 */
	unsigned int getKey() const { return m_key; }
	void setKey(unsigned int k) { m_key = k; }
	virtual unsigned int getType() const { return KEY_PRESSED; }
};

Neo2D::Gui::EditField::EditField(int x,
								 int y,
								 unsigned int w,
								 unsigned int h,
								 const char* label,
								 const shared_ptr<Object2D>& parent,
								 const shared_ptr<Theme>& theme)
	: Widget(x, y, w, h, label, parent, (theme == nullptr) ? make_shared<EditFieldTheme>() : theme),
	  m_caret(0)
{}

void Neo2D::Gui::EditField::init()
{
	registerEvent(make_shared<MouseOverEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<MouseLeaveEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<MouseLeftClickEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<CharacterInputEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<KeyRepeatEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<MouseDeselectEvent>(shared_from_this(), nullptr, nullptr));
}

unsigned int Neo2D::Gui::EditField::characterSize(unsigned int c)
{
	if (c <= 0x7F)
		return 1;
	else if (c <= 0x7FF)
		return 2;
	else if (c <= 0xFFFF)
		return 3;
	else if (c <= 0x10FFFF)
		return 4;

	return 0;
}

bool Neo2D::Gui::EditField::handle(const Event& e)
{
	switch(e.getType())
	{
		case CHARACTER_INPUT: {

			if(getState() != WIDGET_SELECTED)
				return false;

			auto keypress = static_cast<const CharacterInputEvent&>(e);
			std::string str(getLabel());
			unsigned int character = keypress.getCharacter();

			string postfix = str.substr(getCaret());
			str.erase(getCaret());

			str = str + (const char*) &character + postfix;

			m_caret += strlen((const char*) &character);
			setLabel(str.c_str());
		}
		return true;

		case KEY_PRESSED:
		{
			bool handled = false;
			auto keypress = static_cast<const KeyPressEvent&>(e);
			std::string str(getLabel());

			if(getState() != WIDGET_SELECTED)
				return false;

			switch (keypress.getKey())
			{
				case Neo::KEY_BACKSPACE:
				{
					// Don't go back if we don't have to
					if(m_caret == 0) break;

					auto i = str.begin() + m_caret;
					int codepoint = utf8::prior(i, str.begin());

					unsigned int size = characterSize(codepoint);

					if (str.length() >= size && m_caret >= size)
					{
						auto endDelete = i + size;
						str.erase(i, endDelete);
						m_caret -= size;
					}
					handled = true;
				}
					break;

				case Neo::KEY_DELETE:
				{
					auto i = str.begin() + m_caret;
					if(i == str.end())
						break;

					size_t size = characterSize(utf8::peek_next(i, str.end()));
					if (str.length() >= size && getCaret() < str.length())
					{
						auto endDelete = i + size;
						str.erase(i, endDelete);
					}
					handled = true;
				}
					break;

				case Neo::KEY_LEFT_ARROW:
				{
					// Don't go back if we don't have to
					if(m_caret == 0) break;

					auto i = str.begin() + m_caret;
					int codepoint = utf8::prior(i, str.begin());

					unsigned int size = characterSize(codepoint);
					if (m_caret >= size)
						m_caret -= size;

					handled = true;
				}
					break;

				case Neo::KEY_RIGHT_ARROW:
				{
					auto i = str.begin() + m_caret;
					if(i == str.end())
						break;

					size_t size = characterSize(utf8::peek_next(i, str.end()));

					if (m_caret <= str.length() - size)
						m_caret += size;

					handled = true;
				}
					break;

				case Neo::KEY_RETURN:
					doCallback();
					return true;
			}
			setLabel(str.c_str());
			return handled;
		}

		case MOUSE_OVER:
			if(getState() != WIDGET_SELECTED)
				setState(WIDGET_HOVER);
			return true;

		case MOUSE_LEAVE:
			if(getState() == WIDGET_HOVER)
				setState(WIDGET_NORMAL);
			return true;

		case MOUSE_LEFT_CLICK:
			setState(WIDGET_SELECTED);
			// Reset last typed character so we don't pick anything up that already happened
			NeoEngine::getInstance()->getInputContext()->getKeyboard().setCharacter(EOF);
			return true;

		case MOUSE_DESELECT:
			setState(WIDGET_NORMAL);
			return true;
	}

	return false;
}
