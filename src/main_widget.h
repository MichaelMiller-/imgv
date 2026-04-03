#pragma once

class QLabel;
class QMovie;
class QSlider;
class QScrollArea;

#include <QWidget>

#include <filesystem>
#include <string>
#include <vector>

namespace imgv
{
   class main_widget final : public QWidget
   {
      Q_OBJECT

   private:
      QLabel* m_canvas{nullptr};
      QMovie* m_movie{nullptr};
      QScrollArea* m_scroll_area{nullptr};
      // QSlider* frame_slider{nullptr};

      using container_t = std::vector<std::filesystem::path>;
      using value_t = container_t::value_type;

      container_t m_image_list;

   private slots:
      void fit_image_to_widget(bool value);

   private:
      void show_image(value_t const& image_filename);
      void next_image();
      void previous_image();

   public:
      explicit main_widget(container_t img_list);

   protected:
      void showEvent(QShowEvent* event) override;
      void wheelEvent(QWheelEvent* event) override;
      void mousePressEvent(QMouseEvent* event) override;
   };
} // namespace imgv