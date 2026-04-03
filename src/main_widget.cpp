#include "main_widget.h"

#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QMovie>
#include <QScreen>
#include <QScrollArea>
#include <QSlider>
#include <QStyle>
#include <QVBoxLayout>
#include <QWheelEvent>

#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>

namespace imgv
{
   main_widget::main_widget(container_t img_list) : m_image_list{std::move(img_list)}
   {
      //! \todo 2019-04-04 action: play/pause / state / space
      //! \todo 2019-08-29 reload image list in a second thread if key_o is pressed
      //! -> enable action if thread is done

      m_movie = new QMovie(this);
      m_movie->setCacheMode(QMovie::CacheAll);

      m_canvas = new QLabel{tr("no picture loaded"), this};
      m_canvas->setAlignment(Qt::AlignCenter);
      // canvas->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
      m_canvas->setBackgroundRole(QPalette::Dark);
      m_canvas->setAutoFillBackground(true);
      m_canvas->setScaledContents(true);

      m_scroll_area = new QScrollArea{this};
      m_scroll_area->setBackgroundRole(QPalette::Dark);
      m_scroll_area->setWidget(m_canvas);

      // frame_slider = new QSlider(Qt::Horizontal, this);
      // frame_slider->setVisible(false);
      // frame_slider->setTickPosition(QSlider::TicksBelow);
      // frame_slider->setTickInterval(10);

      auto layout = new QVBoxLayout;
      layout->setContentsMargins(0, 0, 0, 0);
      layout->addWidget(m_scroll_area);
      setLayout(layout);

      // actions
      auto action_close = new QAction{this};
      action_close->setShortcut(QKeySequence{Qt::Key::Key_Escape});
      QObject::connect(action_close, &QAction::triggered, this, &QWidget::close);

      auto fit_to_window = new QAction{this};
      fit_to_window->setObjectName("fit_to_window");
      fit_to_window->setCheckable(true);
      fit_to_window->setChecked(true);
      fit_to_window->setShortcut(QKeySequence{Qt::Key::Key_R});

      auto switch_fullscreen = new QAction{this};
      switch_fullscreen->setShortcut(QKeySequence{Qt::Key::Key_F10});

      // auto switch_speed = new QAction{this};
      // switch_speed->setShortcuts({ QKeySequence{Qt::Key::Key_Minus},
      // QKeySequence{Qt::Key::Key_Plus} });

      auto action_play_pause = new QAction{this};
      action_play_pause->setShortcut(QKeySequence{Qt::Key::Key_Space});

      auto action_previous_image = new QAction{this};
      action_previous_image->setShortcut(QKeySequence{Qt::Key::Key_Left});
      QObject::connect(action_previous_image, &QAction::triggered, [this] { previous_image(); });

      auto action_next_image = new QAction{this};
      action_next_image->setShortcut(QKeySequence{Qt::Key::Key_Right});
      QObject::connect(action_next_image, &QAction::triggered, [this] { next_image(); });

      m_canvas->addAction(fit_to_window);
      m_canvas->addAction(switch_fullscreen);
      m_canvas->addAction(action_play_pause);
      m_canvas->addAction(action_previous_image);
      m_canvas->addAction(action_next_image);
      m_canvas->addAction(action_close);

      // auto state_animation = new QState(state_main);
      // auto state_speed = new QState(state_main);

      // state_animation
      //{
      //   auto state_animation_play = new QState(state_animation);
      //   state_animation_play->assignProperty(frame_slider, "visible", false);
      //   QObject::connect(state_animation_play, &QState::entered, [this] {
      //
      //            std::cout << "animation play" << std::endl;
      //      movie->setPaused(false); });
      //   auto state_animation_pause = new QState(state_animation);

      //   state_animation_pause->assignProperty(frame_slider, "visible", true);
      //   //! \todo 2019-04-05 show frameslider (overlay)
      //   QObject::connect(state_animation_play, &QState::entered, [this] {
      //
      //
      //   std::cout << "animation paused" << std::endl;
      //
      //      movie->setPaused(true); });
      //
      //   state_animation->setInitialState(state_animation_play);
      //
      //   state_animation_play->addTransition(play_pause, &QAction::triggered,
      //   state_animation_pause); state_animation_pause->addTransition(play_pause,
      //   &QAction::triggered, state_animation_play);
      //}

      // QObject::connect(movie, &QMovie::frameChanged, this,
      // &main_widget::update_frame_slider); QObject::connect(frame_slider,
      // &QSlider::valueChanged, [this](auto frame){ movie->jumpToFrame(frame); });

      QObject::connect(fit_to_window, &QAction::triggered, this, &main_widget::fit_image_to_widget);
      QObject::connect(switch_fullscreen, &QAction::triggered, [this] {
         spdlog::debug("switch fullscreen");
         setWindowState(windowState() ^ Qt::WindowFullScreen);
      });

      setContentsMargins(0, 0, 0, 0);
      resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
   }

   void main_widget::fit_image_to_widget(bool value)
   {
      spdlog::debug("fit image to widget: {}", value);
      m_scroll_area->setWidgetResizable(value);

      if (not value) {
         m_canvas->adjustSize();
      }
   }

   void main_widget::showEvent(QShowEvent* event) { show_image(*std::begin(m_image_list)); }

   void main_widget::wheelEvent(QWheelEvent* event)
   {
      const auto delta = event->angleDelta();

      if (std::min(delta.y(), 0)) {
         previous_image();
      } else if (std::max(delta.y(), 0)) {
         next_image();
      }
   }

   void main_widget::mousePressEvent(QMouseEvent* event)
   {
      if (event->button() == Qt::LeftButton) {
         next_image();
      }
      if (event->button() == Qt::RightButton) {
         previous_image();
      }
   }

   void main_widget::show_image(value_t const& image_filename)
   {
      m_movie->stop();
      m_canvas->setMovie(m_movie);
      m_movie->setFileName(QString::fromStdString(image_filename));
      m_movie->start();

      // auto fit_to_window = findChild<QAction*>("fit_to_window");
      // fit_image_to_widget(fit_to_window->isChecked());
      fit_image_to_widget(true);

      setWindowTitle(QString::fromStdString(image_filename.string()));
   }

   void main_widget::next_image()
   {
      std::rotate(std::rbegin(m_image_list), std::rbegin(m_image_list) + 1, std::rend(m_image_list));
      show_image(*std::begin(m_image_list));
   }
   void main_widget::previous_image()
   {
      std::rotate(std::begin(m_image_list), std::begin(m_image_list) + 1, std::end(m_image_list));
      show_image(*std::begin(m_image_list));
   }
} // namespace imgv