set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(HEADERS
	include/Neo2D.h
	include/Object2D.h
	include/Container.h
	include/Widget.h
	include/LayoutStrategy.h
	include/FilterStrategy.h
	include/Event.h
	include/CommonEvents.h
	include/Canvas.h
	include/Button.h
	include/Neo2DLevel.h
	include/Label.h
	include/CheckButton.h
	include/RadioButton.h
	include/EditField.h
	include/Menu.h
	include/ImageButton.h
	include/Sprite.h
	include/Toolbar.h
	include/Scrollbar.h
	include/ScrollPanel.h
	include/TreeView.h
	include/Translator.h
	include/csv.h
	include/HorizontalLayout.h
	include/VerticalLayout.h
	include/KeyboardShortcuts.h
	include/Theme.h)

set(SOURCES
	src/Object2D.cpp
	src/Widget.cpp
	src/Event.cpp
	src/Neo2D.cpp
	src/Canvas.cpp
	src/Button.cpp
	src/Neo2DLevel.cpp
	src/Label.cpp
	src/CheckButton.cpp
	src/RadioButton.cpp
	src/EditField.cpp
	src/Menu.cpp
	src/ImageButton.cpp
	src/Sprite.cpp
	src/Toolbar.cpp
	src/Scrollbar.cpp
	src/ScrollPanel.cpp
	src/Container.cpp
	src/TreeView.cpp
	src/Translator.cpp
	src/HorizontalLayout.cpp
	src/VerticalLayout.cpp
	src/KeyboardShortcuts.cpp)

add_library(Neo2D ${SOURCES} ${HEADERS})

if(NOT NO_TESTS)
	add_executable(Neo2DTest 
			tests/main.cpp 
			tests/Object2DTest.cpp 
			tests/ContainerTest.cpp 
			tests/EventTest.cpp 
			tests/ButtonTest.cpp 
			tests/CheckButtonTest.cpp 
			tests/RadioButtonTest.cpp 
			tests/EditFieldTest.cpp 
			tests/MenuTest.cpp 
			tests/TestScrollbar.cpp 
			tests/TreeViewTest.cpp 
			tests/VerticalLayoutTest.cpp 
			tests/KeyboardShortcutsTest.cpp 
			tests/TranslatorTest.cpp 
			tests/SpriteTest.cpp 
			tests/HorizontalLayoutTest.cpp 
			tests/InputContextFixture.h 
			tests/ScrollPanelTest.cpp)
		
	target_link_libraries(Neo2DTest Neo2D NeoEngine ${GTEST_LIBRARIES})
endif()

# Needed for windows
target_link_libraries(Neo2D NeoEngine NeoCore)
target_compile_definitions(Neo2D PRIVATE NEO2D_DLL=1)
target_include_directories(Neo2D PUBLIC include PRIVATE utfcpp/source)

if(SWIG_FOUND)
	set_source_files_properties(Neo2D.i PROPERTIES CPLUSPLUS ON)

	swig_add_module(Neo2DLua lua Neo2D.i)
	target_link_libraries(Neo2DLua
			${NEO_CORE_LIBRARIES}
			${NEO_ENGINE_LIBRARIES}
			Neo2D
			${LUA_LIBRARIES})
endif()

