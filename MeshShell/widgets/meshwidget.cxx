#include "meshwidget.h"
#include "ui_meshwidget.h"
#include "view_tools/vtk_wrapper.h"
#include <qmessagebox.h>
MeshWidget::MeshWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::MeshWidget) {
  ui->setupUi(this);

  _viewer = new viewtools::VtkWrapper(ui->viewerWidget);

  _shell = std::shared_ptr<MeshShell>(new MeshShell(_viewer));

  addSlot();
}

MeshWidget::~MeshWidget() {
  delete _viewer;
  delete ui;
}

void MeshWidget::addSlot() {
  connect(this->ui->pushButton_read, &QPushButton::clicked, this,
          &MeshWidget::readMesh);
  connect(this->ui->checkBox_edge, &QCheckBox::toggled, this,
          &MeshWidget::updateMeshRenderStyle);
  connect(this->ui->checkBox_face, &QCheckBox::toggled, this,
          &MeshWidget::updateMeshRenderStyle);
  connect(this->ui->radioButton_normal, &QRadioButton::toggled, this,
          &MeshWidget::geometryChange);
  connect(this->ui->pushButton_test, &QPushButton::clicked, this,
          &MeshWidget::test);
  connect(this->ui->pushButton_readStressFile, &QPushButton::clicked, this,
          &MeshWidget::readStressField);
  connect(this->ui->pushButton_readCombination, &QPushButton::clicked, this,
          &MeshWidget::readCombination);
  /* draw stress field */
  connect(this->ui->checkBox_render_stress, &QCheckBox::toggled, this,
          &MeshWidget::drawStressField);
  connect(this->ui->checkBox_major, &QCheckBox::toggled, this,
          &MeshWidget::drawStressField);
  connect(this->ui->checkBox_mid, &QCheckBox::toggled, this,
          &MeshWidget::drawStressField);
  connect(this->ui->checkBox_minor, &QCheckBox::toggled, this,
          &MeshWidget::drawStressField);

  /* sigularity */
  connect(this->ui->checkBox_stressSingularity, &QCheckBox::toggled, this,
          &MeshWidget::drawStressSingularity);
  connect(this->ui->pushButton_singularityRefresh, &QPushButton::clicked, this,
          &MeshWidget::drawStressSingularity);
  connect(this->ui->pushButton_split_refresh, &QPushButton::clicked, this,
          &MeshWidget::divideCells);
  connect(this->ui->pushButton_singular_edges_refresh, &QPushButton::clicked,
          this, &MeshWidget::extractSingularLines);
};

void MeshWidget::updateMeshInfo() {
  std::string info = _shell->mesh_wrapper->meshInfo();
  ui->textBrowser_mesh->append(info.c_str());
  // std::cout << info << std::endl;
}

int MeshWidget::getRenderStyle() {
  int nRenderStyle = 0;
  if (ui->checkBox_edge->isChecked() == true) {
    nRenderStyle += 1;
  }
  if (ui->checkBox_face->isChecked() == true) {
    nRenderStyle += 2;
  }
  return nRenderStyle;
}

std::string MeshWidget::filenameFromDialog(const char *dialog_name,
                                           const char *filter) {
  QString qfname =
      QFileDialog::getOpenFileName(0, dialog_name, _directory_path, filter);

  if (qfname == "") {
    return "";
  }

  /********** save last opened path *************/

  int i = qfname.lastIndexOf('/');
  _directory_path = qfname.left(i);

  return qfname.toStdString();
}

void MeshWidget::messageBox(const char *info) {
  QMessageBox::information(NULL, "Warning", info);
}

void MeshWidget::readMesh() {
  std::string filename = filenameFromDialog(
      "Open Mesh File", "OVM files(*.ovm);;Abaqus inp files(*.inp)");

  ui->pushButton_read->setDisabled(true);

  /********** Mesh *************/

  _shell->readMesh(filename);
  _shell->drawMesh(getRenderStyle());
  updateMeshOpacity();
  updateMeshInfo();
}

void MeshWidget::readStressField() {
  std::string filename =
      filenameFromDialog("Open Stress File", "csv files(*.csv)");

  ui->pushButton_readStressFile->setDisabled(true);

  _shell->readStressField(filename);

  std::cout << "read Stress Field down" << std::endl;
}

void MeshWidget::readCombination() {
  std::string filename = filenameFromDialog(
      "Open Mesh File", "OVM files(*.ovm);;Abaqus inp files(*.inp)");

  size_t dot_position = filename.find_last_of('.');
  /********** filename without extension *************/
  std::string common_name = filename.substr(0, dot_position);

  ui->pushButton_read->setDisabled(true);

  /********** Mesh *************/

  _shell->readMesh(filename);
  _shell->drawMesh(getRenderStyle());
  updateMeshOpacity();
  updateMeshInfo();

  ui->pushButton_readStressFile->setDisabled(true);

  if (!_shell->readStressField(common_name + ".csv") &&
      !_shell->readStressField(common_name + "_cell.csv") &&
      !_shell->readStressField(common_name + "_result.csv")) {
    messageBox("No stress field found!");
    return;
  }

  std::cout << "read Stress Field down" << std::endl;

  ui->pushButton_readCombination->setDisabled(true);
}

void MeshWidget::drawStressField() {
  bool mjr, mdd, mnr;
  if (ui->checkBox_render_stress->isChecked()) {
    mjr = ui->checkBox_major->isChecked();
    mdd = ui->checkBox_mid->isChecked();
    mnr = ui->checkBox_minor->isChecked();
    ui->checkBox_major->setDisabled(false);
    ui->checkBox_mid->setDisabled(false);
    ui->checkBox_minor->setDisabled(false);
  } else {
    mjr = mdd = mnr = false;
    ui->checkBox_major->setDisabled(true);
    ui->checkBox_mid->setDisabled(true);
    ui->checkBox_minor->setDisabled(true);
  }
  _shell->drawStressField(mjr, mdd, mnr);
}

void MeshWidget::updateMeshRenderStyle() {
  _shell->updateMeshRenderStyle(getRenderStyle());
}
void MeshWidget::updateMeshOpacity() {
  if (_shell->mesh_loaded == false)
    return;
  double opacity = ui->doubleSpinBox_opacity->value();
  int geometry;
  if (ui->radioButton_shrink->isChecked()) {
    geometry = 2;
  } else {
    geometry = 1;
  }
  _shell->updateFaceOpacity(opacity, geometry);
}
void MeshWidget::geometryChange() {
  if (!_shell->mesh_loaded)
    return;
  if (ui->radioButton_normal->isChecked()) {
    _shell->drawMesh();
  } else if (ui->radioButton_shrink->isChecked()) {
    _shell->drawShrink();
    updateMeshOpacity();
  }
}
void MeshWidget::drawStressSingularity() {
  double tolerance = ui->doubleSpinBox_stressSingularityTolerance->value();
  double pointSize = ui->doubleSpinBox_pointSize->value();
  if (ui->checkBox_stressSingularity->isChecked()) {
    // if has not rendered, do first render
    _shell->setVisibility("Singularity", true);
    _shell->stressSingularity(tolerance, pointSize);
    ui->doubleSpinBox_pointSize->setDisabled(false);
    ui->doubleSpinBox_stressSingularityTolerance->setDisabled(false);
  } else {
    _shell->setVisibility("Singularity", false);
    ui->doubleSpinBox_pointSize->setDisabled(true);
    ui->doubleSpinBox_stressSingularityTolerance->setDisabled(true);
  }
  // std::cout<<"singularities calculation down"<<std::endl;
}
void MeshWidget::updateStressSingularity() {
  double pointSize = ui->doubleSpinBox_pointSize->value();
  _shell->singularitySizeChange(pointSize);
}

void MeshWidget::divideCells() {
  double tolerance = ui->doubleSpinBox_split_tolerance->value();
  /*if (ui->checkBox_render_split_faces->isChecked()) {
    _shell->divideCells(tolerance);
  } else{
  _shell->setVisibility("split_faces",false);}*/
  _shell->divideCells(tolerance);
}

void MeshWidget::extractSingularLines() { _shell->extractSingularLines(); }

void MeshWidget::test() { _shell->test(); }
