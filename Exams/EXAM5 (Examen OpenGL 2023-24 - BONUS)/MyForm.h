#include "ui_MyForm.h"

/**
 * @author Arturo Aragón Hidalgo - https://github.com/artHub-j/indi-opengl-full-course
 */
class MyForm : public QWidget
{
  Q_OBJECT

  public:
    MyForm (QWidget *parent=0);
  
  private:
    Ui::MyForm ui;
};
