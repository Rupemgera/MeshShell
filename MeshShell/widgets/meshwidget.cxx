#include "meshwidget.h"
#include "ui_meshwidget.h"
MeshWidget::MeshWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::MeshWidget) {
  ui->setupUi(this);

  _viewer = new VtkWrapper(ui->viewerWidget);

  addSlot();
}

MeshWidget::~MeshWidget() {
  delete ui;
  delete _viewer;
}

void MeshWidget::addSlot() {
  connect(this->ui->pushButton_read, &QPushButton::clicked, this,
          &MeshWidget::readMesh);
}

void MeshWidget::readMesh() {
  QString qfname =
      QFileDialog::getOpenFileName(0, "Open mesh file", _directory_path,
                                   "OVM files(*.ovm);;Abaqus inp files(*.inp)");

  if (qfname == "") {
    return;
  }

  /********** save last opened path *************/

  int i = qfname.lastIndexOf('/');
  _directory_path = qfname.left(i);

  // meshtools::MeshProcessingTools::separateFilename(qfname.toStdString());
  std::ifstream fin(qfname.toStdString());

  /********** Mesh *************/

  _mesh = std::shared_ptr<MeshShell>(new MeshShell(_viewer));
  _mesh->readMesh(qfname.toStdString());
  _mesh->drawMesh();
}