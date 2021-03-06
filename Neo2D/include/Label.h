#ifndef NEO_LABEL_H
#define NEO_LABEL_H

#include "Widget.h"

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Implements a 2D text label.
 */
class NEO2D_EXPORT Label : public Widget
{
	Neo::Vector4 m_color;
	Neo::TEXT_ALIGN_MODES m_alignment;
	unsigned int m_fontsize;
	Neo::OText* m_text;
	std::string m_font;

public:
	Label(int x, int y, unsigned int w, unsigned int h, const char* label, const shared_ptr<Object2D>& parent);
	~Label() { SAFE_DELETE(m_text); }

	virtual void draw(const Neo::Vector2& offset) override;
	virtual void initText();

	const Neo::Vector4& getColor() const { return m_color; }
	void setColor(const Neo::Vector4& c) { m_color = c; }

	void setAlignment(Neo::TEXT_ALIGN_MODES modes) { m_alignment = modes; }
	Neo::TEXT_ALIGN_MODES getAlignment() const { return m_alignment; }

	unsigned int getFontsize() const { return m_fontsize; }
	void setFontsize(unsigned int fontsize);

	void setFont(const char* m_font);
	const char* getFont() const
	{
		return m_font.c_str();
	}
};

}
}

#endif //NEO_LABEL_H
