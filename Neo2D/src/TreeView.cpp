#include <TreeView.h>
#include <Neo2DLevel.h>
#include <CommonEvents.h>

using namespace Neo2D;
using namespace Gui;
using namespace Neo;

#define LINE_HEIGHT 20

// Can't test rendering
// LCOV_EXCL_START
class TreeItemTheme : public Neo2D::Gui::Theme
{
	OText* text;
public:

	TreeItemTheme()
	{
		text = Neo2D::Neo2DLevel::getInstance()->createText("assets/default.ttf", 12);
		text->setAlign(TEXT_ALIGN_LEFT);
	}

	virtual void draw(Neo2D::Gui::Widget* widget, const Neo::Vector2& offset)
	{
		NeoEngine* engine = NeoEngine::getInstance();
		Renderer* renderer = engine->getRenderer();

		text->setText(widget->getLabel());
		const Neo::Vector2 position = widget->getPosition() + offset;

		switch(widget->getState())
		{
			case Neo2D::Gui::WIDGET_NORMAL:
				renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.8, 0.8, 0.8, 1), 0);

				text->setColor(Vector4(0.0,0.0,0.0,1.0));
				renderer->drawText2D(text, position.x + LINE_HEIGHT + 3,
									 position.y + text->getSize() * 1.25, 0.0f);

				break;

			case Neo2D::Gui::WIDGET_SELECTED:
				renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.0, 0.0, 0.8, 1), 0);

				text->setColor(Vector4(1.0,1.0,1.0,1.0));
				renderer->drawText2D(text, position.x + LINE_HEIGHT + 3,
									 position.y + text->getSize() * 1.25, 0.0f);

				break;

			default: break;
		}
	}
};


class TreeBackgroundTheme : public Neo2D::Gui::Theme
{
public:

	TreeBackgroundTheme()
	{

	}

	virtual void draw(Neo2D::Gui::Widget* widget, const Neo::Vector2& offset)
	{
		NeoEngine* engine = NeoEngine::getInstance();
		Renderer* renderer = engine->getRenderer();
		renderer->drawColoredQuad(widget->getPosition() + offset, widget->getSize(), Vector4(0.8, 0.8, 0.8, 1), 0);
	}
};
// LCOV_EXCL_STOP

Neo2D::Gui::TreeView::TreeView(int x,
							   int y,
							   unsigned int w,
							   unsigned int h,
							   const shared_ptr<Object2D>& parent,
							   const shared_ptr<Theme>& itemtheme,
							   const shared_ptr<Theme>& background)
	: Widget(x, y, w, h, nullptr, parent, (background == nullptr) ? make_shared<TreeBackgroundTheme>() : background),
	  m_itemTheme(itemtheme), m_rootNode(nullptr), m_selected(nullptr)
{ }

void TreeView::update(float dt)
{
	Widget::update(dt);
	if(m_rootNode)
		m_rootNode->update(dt);
}

void TreeView::draw(const Neo::Vector2& offset)
{
	Widget::draw(offset);
	Vector2 level(-1, -1);

	if (m_rootNode)
	{
		m_rootNode->setOpen(true);
		drawTree(*m_rootNode, level, offset);
		setSize(Neo::Vector2(getSize().x, level.y * LINE_HEIGHT));
	}
}

void TreeView::drawTree(TreeNode& root, Vector2& level, const Vector2& offset)
{
	Vector2 pos = getPosition() + level * LINE_HEIGHT;
	if(level.x >= 0)
	{
		root.setSize(Vector2(getSize().x - level.x * LINE_HEIGHT, LINE_HEIGHT));
		root.setPosition(pos);

		if(&root == m_selected)
			root.setState(WIDGET_SELECTED);
		else
			root.setState(WIDGET_NORMAL);

		root.draw(offset);
	}

	level.y++;

	if(!root.isOpen())
		return;

	level.x++;
	for(auto c : root.getChildren())
	{
		drawTree(*c, level, offset);
	}

	level.x--;
}

TreeView::TreeNode::TreeNode(int x,
							 int y,
							 unsigned int w,
							 unsigned int h,
							 const char* label,
							 const shared_ptr<Object2D>& parent,
							 const shared_ptr<Theme>& theme)
	: Widget(x, y, w, h, label, parent, (theme == nullptr) ? make_shared<TreeItemTheme>() : theme),
	  m_button(make_shared<Button>(x,y,LINE_HEIGHT, LINE_HEIGHT, "+", parent)), m_open(false)
{
	m_button->setCallback([this](Widget& w, void* d) {
		setOpen(!isOpen());
		w.setLabel((isOpen() ? "-" : "+"));
	}, nullptr);
}

void TreeView::TreeNode::init()
{
	registerEvent(make_shared<MouseLeftClickEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<MouseDeselectEvent>(shared_from_this(), nullptr, nullptr));
}

bool TreeView::TreeNode::handle(const Event& e)
{
	switch(e.getType())
	{
		case MOUSE_LEFT_CLICK:
			setState(WIDGET_SELECTED);
			doCallback();
			return true;

		case MOUSE_DESELECT:
			setState(WIDGET_NORMAL);
			return true;
	}

	return false;
}

shared_ptr<TreeView::TreeNode> TreeView::findNode(const char* name)
{
	if(!m_rootNode)
		return nullptr;

	return m_rootNode->findNode(name);
}

void TreeView::setSelected(const char* label)
{
	m_selected = findNode(label).get();

	// Ensure the selection is visible
	if (m_selected)
	{
		auto parent = dynamic_pointer_cast<TreeNode>(m_selected->getParent().lock());
		while(parent)
		{
			parent->setOpen(true);
			parent = dynamic_pointer_cast<TreeNode>(parent->getParent().lock());
		}
	}
}
