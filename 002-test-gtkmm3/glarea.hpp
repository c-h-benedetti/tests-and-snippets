#ifndef GL_AREA_HPP_INCLUDED
#define GL_AREA_HPP_INCLUDED

#include <gtkmm.h>
#include <vector>
#include <giomm/resource.h>
#include <epoxy/gl.h>

int useglarea(int argc, char *argv[]);
int launchexample(int argc, char *argv[]);

enum {
  X_AXIS,
  Y_AXIS,
  Z_AXIS,

  N_AXIS
};


class Example_GLArea : public Gtk::Window
{
public:
  Example_GLArea();
  ~Example_GLArea() override;

protected:
  Gtk::Box m_VBox {Gtk::Orientation::ORIENTATION_VERTICAL, false};
  Gtk::GLArea m_GLArea;
  Gtk::Box m_Controls {Gtk::Orientation::ORIENTATION_VERTICAL, false};
  Gtk::Button m_Button {"Quit"};

  GLuint m_Vao {0};
  GLuint m_Buffer {0};
  GLuint m_Program {0};
  GLuint m_Mvp {0};

  std::vector<float> m_RotationAngles;

  void on_axis_value_change(int axis, const Glib::RefPtr<Gtk::Adjustment>& adj);

  void realize();
  void unrealize();
  bool render(const Glib::RefPtr<Gdk::GLContext>& context);

  Gtk::Box* create_axis_slider_box(int axis);
  void init_buffers();
  void init_shaders(const std::string& vertex_path, const std::string& fragment_path);
  void draw_triangle();
};

Gtk::Window* do_glarea();

#endif // GL_AREA_HPP_INCLUDED