#pragma once

class QLabel;
class QMovie;
class QSlider;
class QScrollArea;

#include <QWidget>

#include <vector>
#include <string>
#include <filesystem>

class main_widget final : public QWidget
{
   Q_OBJECT

private:
   QLabel *canvas{nullptr};
   QMovie *movie{nullptr};
   QScrollArea* scroll_area{nullptr};
   // QSlider* frame_slider{nullptr};

   QAction* play_pause{nullptr};
   QAction* fit_to_window{nullptr};
   QAction* previous_image{nullptr};
   QAction* next_image{nullptr};

   using container_t = std::vector<std::string>;
   using value_t = container_t::value_type;

   container_t image_list;

private slots:
   void fit_image_to_widget(bool value);

private:
   void show_image(value_t const& image_filename);

public:
   explicit main_widget(container_t&& img_list);

protected:
   virtual void showEvent(QShowEvent *event) override;
   virtual void wheelEvent(QWheelEvent *event) override;
};
