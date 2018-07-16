#ifndef NEO_HTML_HTMLVIEW_H
#define NEO_HTML_HTMLVIEW_H

#include <nanovg.h>
#include <litehtml.h>

#include <vector>
#include <unordered_map>
#include <functional>

#include <Texture.h>

namespace Neo 
{
class Renderer;
class Platform;
namespace HTML 
{

class HTMLView;
class HTMLElement
{
	HTMLView* m_view = nullptr;
	std::shared_ptr<litehtml::element> m_element;
public:
	HTMLElement(HTMLView* view, const std::shared_ptr<litehtml::element>& element): 
		m_element(element),
		m_view(view) {}
	
	HTMLElement() {}
	
	std::string getValue() const;
	void setValue(const char* str);
	
	void setStyle(const char* name, const char* value);
	bool empty() const { return m_view == nullptr || m_element == nullptr; }
};

class HTMLView : public litehtml::document_container
{
	std::string m_basePath = "./";
	
	NVGcontext* m_nv = nullptr;
	std::shared_ptr<litehtml::document> m_document = nullptr;
	litehtml::context m_context;
	
	struct Font
	{
		int handle = -1;
		int size = 16;
		int descent = 0;
	};
	
	float m_dpi = 96;
	unsigned int m_width = 0, m_height = 0;
	bool m_needsRender = true;
	
	struct Image
	{
		Image() = default;
		Image(Image&& img) : texture(std::move(img.texture)),
			handle(img.handle) {}
			
		Texture texture;
		int handle;
	};
	
	std::unordered_map<std::string, Image> m_images;
	std::unordered_map<std::string, std::function<void(const char* url)>> m_callbacks;
	
public:
	void begin(unsigned int w, unsigned int h, float dpi = 96.0f);
	void end();
	
	void addCallback(const char* id, const std::function<void(const char* url)> cb) { m_callbacks[id] = cb; }
	void removeCallback(const char* id) { m_callbacks.erase(id); }
	
	float getDPI() const { return m_dpi; }
	void setDPI(float f) { m_dpi = f; }
	
	HTMLElement getById(const char* id)
	{
		auto ptr = m_document->root()->select_one(id);
		return HTMLElement(this, ptr);
	}
	
	bool loadDocument(const char* file);
	void draw(Renderer& render);
	void update(Platform& p, float dt);
	void renderPage();
	
	bool isMouseOver(Platform& p);
	
	// Overridden stuff
	litehtml::uint_ptr create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) override;
	void delete_font(litehtml::uint_ptr hFont) override;
	int text_width(const litehtml::tchar_t* text, litehtml::uint_ptr hFont) override;
	void draw_text(litehtml::uint_ptr hdc, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) override;
	int pt_to_px(int pt) override;
	int get_default_font_size() const override;
	const litehtml::tchar_t* get_default_font_name() const override;
	void draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) override;
	void load_image(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready) override;
	void get_image_size(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz) override;
	void draw_background(litehtml::uint_ptr hdc, const litehtml::background_paint& bg) override;
	void draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) override;

	void set_caption(const litehtml::tchar_t* caption) override;
	void set_base_url(const litehtml::tchar_t* base_url) override;
	void link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) override;
	void on_anchor_click(const litehtml::tchar_t* url, const litehtml::element::ptr& el) override;
	void set_cursor(const litehtml::tchar_t* cursor) override;
	void transform_text(litehtml::tstring& text, litehtml::text_transform tt) override;
	void import_css(litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl) override;
	
	void set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y) override;
	void del_clip() override;
	
	void get_client_rect(litehtml::position& client) const override;
	std::shared_ptr<litehtml::element> create_element(const litehtml::tchar_t *tag_name,
								const litehtml::string_map &attributes,
								const std::shared_ptr<litehtml::document> &doc) override;

	void get_media_features(litehtml::media_features& media) const override;
	void get_language(litehtml::tstring& language, litehtml::tstring & culture) const override;
};

}
}

#endif // NEO_HTML_HTMLVIEW_H
