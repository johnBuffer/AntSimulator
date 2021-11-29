#include "container.hpp"
#include "text_label.hpp"
#include "rounded_rectangle.hpp"


namespace GUI
{

struct NamedContainer : public Container
{
    SPtr<TextLabel> label;
    SPtr<Container> root;
    
    NamedContainer(const std::string& name, Container::Orientation orientation = Container::Orientation::Vertical)
        : Container(Container::Orientation::Vertical)
    {
        padding = 7.0f;
        spacing = 0.0f;
        size_type.y = Size::FitContent;
        
        label = create<TextLabel>(name, 14);
        label->setColor(sf::Color(100, 100, 100));
        label->setHeight(20.0f);
        label->setAlignement(Alignement::Left);
        
        root = create<Container>(orientation);
        root->size_type.y = Size::FitContent;
        root->padding = 0.0f;
        
        Container::addItem(label);
        Container::addItem(root);
    }
    
    void addItem(ItemPtr item, const std::string& name = "")
    {
        root->addItem(item, name);
    }
    
    void render(sf::RenderTarget& target) override
    {
        const float angle_radius = 10.0f;
        RoundedRectangle background(size, position, angle_radius);
        const uint8_t background_intensity = 220;
        background.setFillColor(sf::Color(background_intensity, background_intensity, background_intensity));
        GUI::Item::draw(target, background);
    }
};

}
