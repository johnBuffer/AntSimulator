#pragma once
#include "container.hpp"
#include "text_label.hpp"
#include "rounded_rectangle.hpp"
#include "empty_item.hpp"


namespace GUI
{

struct NamedContainer : public Container
{
    SPtr<Container> header;
    SPtr<TextLabel> label;
    SPtr<Container> root;

    uint8_t background_intensity = 220;

    bool show_root = true;
    
    NamedContainer(const std::string& name, Container::Orientation orientation = Container::Orientation::Vertical)
        : Container(Container::Orientation::Vertical)
    {
        padding = 7.0f;
        spacing = 5.0f;
        size_type.y = Size::FitContent;

        header = create<Container>(Container::Orientation::Horizontal);
        header->padding = 0.0f;
        header->spacing = 0.0f;
        header->size_type.y = Size::FitContent;

        label = create<TextLabel>(name, 14);
        label->setColor(sf::Color(100, 100, 100));
        label->setHeight(20.0f);
        label->setAlignment(Alignment::Left);

        header->addItem(label);

        root = create<Container>(orientation);
        root->padding = 0.0f;
        root->size_type.y = GUI::Size::FitContent;
        
        Container::addItem(header);
    }

    void hideRoot()
    {
        show_root = false;
        GUI::Container::removeItem(root);
    }

    void showRoot()
    {
        if (!show_root) {
            show_root = true;
            GUI::Container::addItem(root);
        }
    }

    void fitLabel()
    {
        this->header->setWidth(label->size.x);
        watchSize(label, [this](){
            this->header->setWidth(label->size.x);
        });
    }
    
    void addItem(ItemPtr item, const std::string& item_name = "")
    {
        if (root->isEmpty() && show_root) {
            GUI::Container::addItem(root);
        }
        root->addItem(item, item_name);
    }

    template<typename T>
    void removeItem(SPtr<T> item)
    {
        root->removeItem(item);
        if (root->isEmpty()) {
            GUI::Container::removeItem(root);
        }
    }

    void render(sf::RenderTarget& target) override
    {
        const float angle_radius = 10.0f;
        RoundedRectangle background(size, position, angle_radius);
        background.setFillColor({background_intensity, background_intensity, background_intensity});
        GUI::Item::draw(target, background);
    }

    void fitContent()
    {
        GUI::Container::fitContent();
        root->fitContent();
    }
};

}
