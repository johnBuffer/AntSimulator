#pragma once
#include "GUI/scene.hpp"
#include "toolbox.hpp"
#include "color_picker.hpp"
#include "color_saver.hpp"
#include "GUI/utils.hpp"
#include "set_color_button.hpp"
#include "GUI/empty_item.hpp"
#include "GUI/named_container.hpp"
#include "tool_selector.hpp"


namespace edtr
{

struct EditorScene : public GUI::Scene
{
    using Ptr = std::shared_ptr<EditorScene>;


    
    SPtr<Toolbox> toolbox;
    SPtr<Renderer> renderer;
    SPtr<ColorPicker> color_picker;
    SPtr<ColorSaver> color_saver;
    SPtr<SetColorButton> set_color_button;
    SPtr<ToolSelector> tool_selector;
    
    EditorScene(sf::RenderWindow& window)
        : GUI::Scene(window)
    {
        initialize();
    }
    
    void initialize()
    {
        const sf::Vector2u window_size = window.getSize();
        toolbox = create<Toolbox>(sf::Vector2f(450.0f, to<float>(window_size.y)));
        color_picker = create<ColorPicker>();
        color_saver = create<ColorSaver>();
        tool_selector = create<ToolSelector>();
        
        // Atoms editing group
        SPtr<GUI::Container> atom_editing = create<GUI::Container>(GUI::Container::Orientation::Horizontal);
        atom_editing->size_type.y = GUI::Size::FitContent;
        atom_editing->padding = 0.0f;
        
        // Brush radius
        SPtr<GUI::NamedContainer> brush_radius_group = create<GUI::NamedContainer>("Brush radius");
        // Slider
        SPtr<SliderLabel> slider = create<SliderLabel>(10.0f, 1.0f);
        root.watch(slider, [this, slider](){

        });
        brush_radius_group->addItem(slider);
        
        // Brush selector
        SPtr<GUI::NamedContainer> brush_type_group = create<GUI::NamedContainer>("Brush mode");
        brush_type_group->addItem(tool_selector);
        root.watch(tool_selector, [this](){

        });
        
        // Colors
        SPtr<GUI::NamedContainer> colors = create<GUI::NamedContainer>("Color picker", GUI::Container::Orientation::Vertical);
        colors->name = "Colors Group";
        SPtr<GUI::Container> atom_color = create<GUI::Container>(GUI::Container::Orientation::Horizontal);
        set_color_button = create<SetColorButton>("Set Color", [&](){});
        const float color_button_height = 80.0f;
        set_color_button->setHeight(color_button_height);
        atom_color->size_type.y = GUI::Size::FitContent;
        atom_color->padding = 0.0f;
        // Add buttons to atom_color
        atom_color->addItem(set_color_button);
        atom_color->addItem(color_saver->current_color_add);
        // Add items to scene
        colors->addItem(atom_color);
        colors->addItem(color_picker);
        colors->addItem(color_saver);
        // Create GUI elements
        // Force toolbox to fit window's height
        toolbox->size_type.y = GUI::Size::Fixed;
        
        const float edit_button_height = 100.0f;
        SPtr<GUI::Button> create_atoms = create<GUI::Button>("Add", [&](){
        });
        create_atoms->setHeight(edit_button_height);
        
        SPtr<GUI::Button> delete_atoms = create<GUI::Button>("Remove", [&](){
        });
        delete_atoms->setHeight(edit_button_height);
        
        root.watch(color_saver, [&](){
            set_color_button->color = color_saver->loaded_color;
        });
        // Add buttons to atom_editing
        atom_editing->addItem(create_atoms);
        atom_editing->addItem(delete_atoms);
        toolbox->addItem(atom_editing);
        toolbox->addItem(brush_radius_group);
        toolbox->addItem(brush_type_group);
        toolbox->addItem(create<GUI::EmptyItem>());
        toolbox->addItem(colors, "Colors Group");
        
        toolbox->watch(color_picker, [&](){
            updateInterfaceColor();
        });
        
        addItem(toolbox, "Toolbox");
        
        updateInterfaceColor();
    }
    
    void updateInterfaceColor()
    {
        color_saver->current_color_add->setColor(color_picker->getColor());
        set_color_button->color = color_picker->getColor();
    }
};

}
