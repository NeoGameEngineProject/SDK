#include "HTMLView.h"
#include <cassert>

#include <FileTools.h>
#include <StringTools.h>
#include <Renderer.h>
#include <TextureLoader.h>
#include <Platform.h>

#include <iostream>
#include <cmath>

// FIXME: Ugly?
#include "../litehtml-external/src/el_text.h"

static char s_masterCSS[] = 
{
#include "master.css.inc"
    , 0
};

using namespace Neo;
using namespace HTML;

std::string HTMLElement::getValue() const 
{
	std::string str;
	m_element->get_text(str);
	return str;
}

void HTMLElement::setValue(const char* str)
{
	auto tag = std::make_shared<litehtml::el_text>(str, m_element->get_document());

	m_element->clearRecursive();
	m_element->appendChild(tag);
	
	tag->parse_styles(true);
	m_view->renderPage();
}

void HTMLElement::setStyle(const char* name, const char* value)
{
	m_element->refresh_styles();

	litehtml::style style;
	style.add_property(name, value, nullptr, true);
	m_element->add_style(style);
	m_element->parse_styles(true);
	
	m_view->renderPage();
}

void HTMLView::begin(unsigned int w, unsigned int h, float dpi)
{
	assert(!m_nv && "Already initialized!");
	m_nv = nvgCreate(1, 2);
	
	m_context.load_master_stylesheet(s_masterCSS);
	m_width = w;
	m_height = h;
	m_dpi = dpi;
}

void HTMLView::end()
{
	assert(m_nv && "Not initialized!");
	nvgDelete(m_nv);
	m_nv = nullptr;
}

bool HTMLView::loadDocument(const char* file)
{
	char* text = readTextFile(file);
	if(!text)
		return false;
	
	m_basePath = file;
	auto lastSlash = m_basePath.find_last_of('/');
	if(lastSlash != std::string::npos)
		m_basePath.erase(lastSlash + 1);
	else
		m_basePath = "./";
	
	m_document = litehtml::document::createFromUTF8(text, this, &m_context);
	delete text;
	
	if(!m_document)
		return false;
	
	renderPage();
	return true;
}

bool HTMLView::isMouseOver(Platform& p)
{
	auto& input = p.getInputContext();
	auto& mouse = input.getMouse();
	auto pos = mouse.getPosition();
	
	// FIXME Not perfect!
	auto child = m_document->root()->get_child_by_point(pos.x, pos.y, pos.x, pos.y, litehtml::draw_inlines, 0);
	return child != nullptr;
}

void HTMLView::renderPage()
{
	m_needsRender = true;
}

void HTMLView::draw(Renderer& render)
{
	nvgBeginFrame(m_nv, m_width, m_height, 1.0f);
	nvgSave(m_nv);
	
	m_document->draw(nullptr, 0, 0, nullptr);
		
	nvgRestore(m_nv);
	nvgEndFrame(m_nv);
}

void HTMLView::update(Platform& p, float dt)
{
	auto& input = p.getInputContext();
	auto& mouse = input.getMouse();
	auto pos = mouse.getPosition();
	litehtml::position::vector redraw_boxes;

	if(mouse.getDirection().getLength() > 0.0f)
	{
		m_document->on_mouse_over(pos.x, pos.y, pos.x, pos.y, redraw_boxes);
	}
	
	if(mouse.onKeyDown(MOUSE_BUTTON_LEFT))
	{
		m_document->on_lbutton_down(pos.x, pos.y, pos.x, pos.y, redraw_boxes);
	}
	else if(mouse.onKeyUp(MOUSE_BUTTON_LEFT))
	{
		m_document->on_lbutton_up(pos.x, pos.y, pos.x, pos.y, redraw_boxes);
	}
	
	if(m_needsRender)
	{
		m_document->render(m_width);
		m_needsRender = false;
	}
}

// Override stuff
litehtml::uint_ptr HTMLView::create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) 
{
	std::string path = m_basePath + faceName + ".ttf";
	
	unsigned int byteSize = 0;
	void* data = readBinaryFile(path.c_str(), &byteSize);
	if(!data)
	{
		std::cerr << "Could not load font and using fallback for " << path << std::endl;
		return create_font(get_default_font_name(), size, weight, italic, decoration, fm);
	}
	
	nvgSave(m_nv);
	nvgFontSize(m_nv, size);
	int value = nvgCreateFontMem(m_nv, faceName, (unsigned char*) data, byteSize, true);
	
	nvgFontFaceId(m_nv, value);
	
	float ascent, descent, height;
	nvgTextMetrics(m_nv, &ascent, &descent, &height);	
	nvgRestore(m_nv);
	
	Font* font = new Font;
	font->size = size;
	font->handle = value;
	
	fm->height = height;
	fm->ascent = ascent;
	fm->descent = font->descent = descent;
	
	return (litehtml::uint_ptr) font;
}

void HTMLView::delete_font(litehtml::uint_ptr hFont) 
{
	delete reinterpret_cast<Font*>(hFont);
}

int HTMLView::text_width(const litehtml::tchar_t* text, litehtml::uint_ptr hFont) 
{
	Font* font = reinterpret_cast<Font*>(hFont);
	
	nvgSave(m_nv);
	nvgFontFaceId(m_nv, font->handle);
	nvgFontSize(m_nv, font->size);

	int size = std::ceil(nvgTextBounds(m_nv, 0, 0, text, nullptr, nullptr));
	nvgRestore(m_nv);
	
	return size;
}

void HTMLView::draw_text(litehtml::uint_ptr hdc, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) 
{
	Font* font = reinterpret_cast<Font*>(hFont);

	nvgSave(m_nv);
	
	nvgTextAlign(m_nv, NVG_ALIGN_LEFT|NVG_ALIGN_BOTTOM);
	nvgFontFaceId(m_nv, font->handle);
	nvgFontSize(m_nv, font->size);
	nvgFillColor(m_nv, nvgRGBA(color.red, color.green, color.blue, color.alpha));
	
	int x = pos.left();
	int y = pos.bottom() + font->descent;

	nvgText(m_nv, x, y, text, nullptr);
	nvgRestore(m_nv);
}

int HTMLView::pt_to_px(int pt) 
{
	return (float) pt * m_dpi / 72.0;
}

int HTMLView::get_default_font_size() const 
{
	return 16;
}

const litehtml::tchar_t* HTMLView::get_default_font_name() const 
{
	return "Roboto-Regular";
}

void HTMLView::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) {}

void HTMLView::load_image(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready) 
{
	auto image = m_images.find(src);
	if(image == m_images.end())
	{
		std::string file = m_basePath + src;
		
		Image& img = m_images[src];
		if(!TextureLoader::load(img.texture, file.c_str()) || img.texture.getComponents() != 4)
		{
			m_images.erase(src);
			std::cerr << "Could not load texture " << file << std::endl;
			return;
		}
		
		img.handle = nvgCreateImageRGBA(m_nv, 
						img.texture.getWidth(), 
						img.texture.getHeight(), 
						NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY, 
						(const unsigned char*) img.texture.getData());
		
		assert(img.handle != -1);
	}
}

void HTMLView::get_image_size(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz) 
{
	auto image = m_images.find(src);
	if(image == m_images.end())
		return;
	
	sz.width = image->second.texture.getWidth();
	sz.height = image->second.texture.getHeight();
}

void HTMLView::draw_background(litehtml::uint_ptr hdc, const litehtml::background_paint& bg) 
{
	if(bg.is_root)
		return;
	
	auto pair = m_images.find(bg.image);
	if(pair == m_images.end())
	{
		nvgSave(m_nv);
		nvgBeginPath(m_nv);
		
		nvgRoundedRectVarying(m_nv, 
					bg.border_box.x,
					bg.border_box.y, 
					bg.border_box.width, 
					bg.border_box.height, 
					bg.border_radius.top_left_x,
					bg.border_radius.top_right_x,
					bg.border_radius.bottom_right_x,
					bg.border_radius.bottom_left_x
     				);
		
		nvgFillColor(m_nv, nvgRGBA(bg.color.red, bg.color.green, bg.color.blue, bg.color.alpha));
		nvgFill(m_nv);
		nvgRestore(m_nv);
		
		return;
	}
	
	auto& img = pair->second;
	
	nvgSave(m_nv);
	
	auto paint = nvgImagePattern(m_nv, 0, 0, img.texture.getWidth(), img.texture.getHeight(), 0, img.handle, 1.0f);
	
	nvgBeginPath(m_nv);
	nvgTranslate(m_nv, bg.position_x, bg.position_y);
	nvgRoundedRectVarying(m_nv, 0, 0, bg.image_size.width, bg.image_size.height,
					bg.border_radius.top_left_x,
					bg.border_radius.top_right_x,
					bg.border_radius.bottom_right_x,
					bg.border_radius.bottom_left_x
	);

	nvgFillPaint(m_nv, paint);
	nvgFill(m_nv);
	
	nvgRestore(m_nv);
}

void HTMLView::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root)
{
	int bdr_top = 0;
	int bdr_bottom = 0;
	int bdr_left = 0;
	int bdr_right = 0;

	if(borders.top.width != 0 && borders.top.style != litehtml::border_style_hidden)
	{
		bdr_top = borders.top.width;
	}
	
	if(borders.bottom.width != 0 && borders.bottom.style != litehtml::border_style_hidden)
	{
		bdr_bottom = borders.bottom.width;
	}
	
	if(borders.left.width != 0 && borders.left.style != litehtml::border_style_hidden)
	{
		bdr_left = borders.left.width;
	}
	
	if(borders.right.width != 0 && borders.right.style != litehtml::border_style_hidden)
	{
		bdr_right = borders.right.width;
	}
	

	if(!bdr_top && !bdr_bottom && !bdr_left && !bdr_right)
		return;
	else if(bdr_top && bdr_bottom && bdr_left && bdr_right
		&& borders.top.style == litehtml::border_style_outset)
	{	
		int xoff = 2;
		int yoff = 2;
		
		auto paint = nvgBoxGradient(m_nv, 
				draw_pos.left() + xoff,
				draw_pos.top() + yoff,
				draw_pos.width,
				draw_pos.height,
				borders.radius.top_left_x,
				borders.top.width,
				nvgRGBA(borders.top.color.red, borders.top.color.green, borders.top.color.blue, 128),
				nvgRGBA(borders.bottom.color.red, borders.bottom.color.green, borders.bottom.color.blue, 0));
				
		nvgSave(m_nv);
		nvgBeginPath(m_nv);
		
		nvgFillColor(m_nv, nvgRGBA(0, 0, 0, 255));
		nvgRect(m_nv, draw_pos.left() - borders.top.width + xoff, draw_pos.top() - borders.top.width + yoff,
			draw_pos.width + borders.top.width * 2 + xoff, draw_pos.height + borders.top.width * 2 + yoff);
		
		nvgGlobalCompositeOperation(m_nv, NVG_DESTINATION_OUT);
		
		nvgFillPaint(m_nv, paint);
		nvgFill(m_nv);
		nvgRestore(m_nv);
		return;
	}
	
	nvgSave(m_nv);
	nvgBeginPath(m_nv);
	
	if(bdr_top)
	{
		if(borders.radius.top_left_x)
			nvgLineCap(m_nv, NVG_ROUND);
		else
			nvgLineCap(m_nv, NVG_BUTT);
		
		nvgStrokeColor(m_nv, nvgRGBA(borders.top.color.red, borders.top.color.green, borders.top.color.blue, borders.top.color.alpha));
		nvgStrokeWidth(m_nv, borders.top.width);
		
		nvgMoveTo(m_nv, draw_pos.x, draw_pos.y);
		nvgLineTo(m_nv, draw_pos.x + draw_pos.width, draw_pos.y);
	}
	
	if(bdr_bottom)
	{
		if(borders.radius.bottom_left_x)
			nvgLineCap(m_nv, NVG_ROUND);
		else
			nvgLineCap(m_nv, NVG_BUTT);
		
		nvgStrokeColor(m_nv, nvgRGBA(borders.bottom.color.red, borders.bottom.color.green, borders.bottom.color.blue, borders.bottom.color.alpha));
		nvgStrokeWidth(m_nv, borders.bottom.width);
		
		nvgMoveTo(m_nv, draw_pos.left(), draw_pos.bottom());
		nvgLineTo(m_nv, draw_pos.left() + draw_pos.width, draw_pos.bottom());
	}
	
	if(bdr_left)
	{
		if(borders.radius.top_left_y)
			nvgLineCap(m_nv, NVG_ROUND);
		else
			nvgLineCap(m_nv, NVG_BUTT);
		
		nvgStrokeColor(m_nv, nvgRGBA(borders.left.color.red, borders.left.color.green, borders.left.color.blue, borders.left.color.alpha));
		nvgStrokeWidth(m_nv, borders.left.width);
		
		nvgMoveTo(m_nv, draw_pos.left(), draw_pos.top());
		nvgLineTo(m_nv, draw_pos.left(), draw_pos.top() + draw_pos.height);
	}
	
	if(bdr_right)
	{
		if(borders.radius.top_right_y)
			nvgLineCap(m_nv, NVG_ROUND);
		else
			nvgLineCap(m_nv, NVG_BUTT);
		
		nvgStrokeColor(m_nv, nvgRGBA(borders.right.color.red, borders.right.color.green, borders.right.color.blue, borders.right.color.alpha));
		nvgStrokeWidth(m_nv, borders.right.width);
		
		nvgMoveTo(m_nv, draw_pos.right(), draw_pos.top());
		nvgLineTo(m_nv, draw_pos.right(), draw_pos.top() + draw_pos.height);
	}
	
	nvgStroke(m_nv);
	nvgRestore(m_nv);
}

void HTMLView::set_caption(const litehtml::tchar_t* caption) {}
void HTMLView::set_base_url(const litehtml::tchar_t* base_url) {}
void HTMLView::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) {}

void HTMLView::on_anchor_click(const litehtml::tchar_t* url, const litehtml::element::ptr& el) 
{
	auto id = el->get_attr("id");
	if(!id)
		return;
	
	auto cb = m_callbacks.find(id);
	if(cb == m_callbacks.end())
		return;
	
	cb->second(url);
}

void HTMLView::set_cursor(const litehtml::tchar_t* cursor) {}
void HTMLView::transform_text(litehtml::tstring& text, litehtml::text_transform tt) {}

void HTMLView::import_css(litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl) 
{
	char* file = readTextFile((m_basePath + url).c_str());
	
	if(!file)
	{
		std::cerr << "Could not load CSS: " << m_basePath << url << std::endl;
		return;
	}
	
	text = file;
	delete file;
}

void HTMLView::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y) 
{
	
}

void HTMLView::del_clip() 
{
	
}

void HTMLView::get_client_rect(litehtml::position& client) const 
{
	client.x = 0;
	client.y = 0;
	client.width = m_width;
	client.height = m_height;
}

std::shared_ptr<litehtml::element> HTMLView::create_element(const litehtml::tchar_t *tag_name,
								const litehtml::string_map &attributes,
								const std::shared_ptr<litehtml::document> &doc) 
{
	return nullptr;
}

void HTMLView::get_media_features(litehtml::media_features& media) const 
{
	media.type = litehtml::media_type_screen;
	media.width = m_width;
	media.height = m_height;
	
	media.device_width = m_width;
	media.device_height = m_height;
	
	media.color = 8;
	media.monochrome = 0;
	media.color_index = 256;
	media.resolution = m_dpi;
}

void HTMLView::get_language(litehtml::tstring& language, litehtml::tstring & culture) const {}
	
	
