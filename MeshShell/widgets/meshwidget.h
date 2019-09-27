#pragma once

#include "src/mesh_shell.h"
#include "view_tools/vtk_wrapper.h"

#include "QtWidgets/qlistwidget.h"
#include <QFile>
#include <QFileDialog>
#include <QWidget>

#include <map>
#include <memory>

using ActorMap = std::map<std::string, std::string>;

namespace Ui {
class MeshWidget;
}

class MeshWidget : public QWidget {
  Q_OBJECT
public:
  MeshWidget(QWidget *parent);
  MeshWidget(QWidget *parent,std::string mesh_file,std::string stress_file);
  // QVtkMeshWidget(VMeshPtr mesh, QVTKOpenGLWidget *widget, QWidget *parent);
  ~MeshWidget();

private:
  Ui::MeshWidget *ui;

  void addSlot();

  viewtools::VtkWrapper *_viewer;
  std::shared_ptr<MeshShell> _shell;
  ActorMap active_actors;

  bool _mesh_loaded = false;

  /* functions */

  void updateMeshInfo();

  int getRenderStyle();

  std::string filenameFromDialog(const char *dialog_name, const char *filter);

  void messageBox(const char *info);

  /* properties */
#ifdef __linux
  QString _directory_path = "~/";
#endif
#ifdef WIN32
  QString _directory_path = "D:\\Users\\A\\Documents\\Models";
#endif

private slots:
  void readMesh();

  void readStressField();

  void readCombination();

  void drawStressField();

  void updateMeshRenderStyle();

  void updateMeshOpacity();

  void geometryChange();

  /* stress singularity related */

  void updateStressSingularity();

  void toggleStressSingularity();

  void splitFaces();

  void on_checkBox_render_splited_faces_toggled();

  void extractSingularLines();

  void on_checkBox_render_singular_edges_toggled();

  void test();

  void on_pushButton_refresh_listWidget_clicked();

  void on_listWidget_itemChanged(QListWidgetItem *item);

  // void on_listWidget_itemSelectionChanged();

  void on_listWidget_itemClicked(QListWidgetItem *item);

  void on_pushButton_actor_color_clicked();

  void on_actor_refresh_clicked();
};