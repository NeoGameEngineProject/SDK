#ifndef NEO_LUABEHAVIOR_H
#define NEO_LUABEHAVIOR_H

#include <Behavior.h>
#include <LuaScript.h>

namespace Neo 
{

class NEO_SCRIPT_EXPORT LuaBehavior : public Neo::Behavior
{
	FixedString<64> fileName;
	FixedString<32> name;
	
	bool m_hasUpdate = false;
	bool m_hasDraw = false;
	
	LuaScript m_script;
	
public:
	LuaBehavior(const char* file);
	LuaBehavior() = default;
	~LuaBehavior() = default;

	LuaBehavior(const LuaBehavior& b)
	{
		fileName = b.fileName;
		name = b.name;
	}
	
	bool isEditorVisible() const override { return false; }
	const char* getName() const override;
	Neo::Behavior* getNew() const override;
	void copyTo(Behavior&) const override {}
	
	std::unique_ptr<Neo::Behavior> clone() const override;
	void begin(Neo::Platform& p, Neo::Renderer& render, Neo::Level& level) override;
	void update(Neo::Platform& p, float dt) override;
	void draw(Neo::Renderer& render) override;
	void end() override;
	void serialize(std::ostream& out) override;
	void deserialize(Neo::Level&, std::istream& in) override;
	
	LuaScript& getScript() { return m_script; }
};

}

#endif // NEO_LUABEHAVIOR_H
