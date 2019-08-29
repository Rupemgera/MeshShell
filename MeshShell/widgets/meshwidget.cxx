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
	connect(this->ui->checkBox_edge, &QCheckBox::toggled, this,
		&MeshWidget::updateMeshRenderStyle);
	connect(this->ui->checkBox_face, &QCheckBox::toggled, this,
		&MeshWidget::updateMeshRenderStyle);
	connect(this->ui->radioButton_normal, &QRadioButton::toggled, this,
		&MeshWidget::geometryChange);
	connect(this->ui->pushButton_test, &QPushButton::clicked, this,
		&MeshWidget::test);
}

void MeshWidget::updateMeshInfo() {
  ui->label_vertices_count->setNum((int)_shell->ovm_mesh->n_vertices());
  ui->label_edges_count->setNum((int)_shell->ovm_mesh->n_edges());
  ui->label_faces_count->setNum((int)_shell->ovm_mesh->n_faces());
  ui->label_cells_count->setNum((int)_shell->ovm_mesh->n_cells());
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

void MeshWidget::updateMeshRenderStyle() {
  _shell->updateMeshRenderStyle(getRenderStyle());
}
void MeshWidget::geometryChange()
{
	if(ui->radioButton_normal->isChecked()){
		_shell->drawMesh();
	}
	else if(ui->radioButton_shrink->isChecked()){
		_shell->drawShrink();
	}
}
void MeshWidget::test()
{
	std::vector<double> s;
	s.push_back(1);
	s.push_back(2);
	s.push_back(3);
	s.push_back(4);
	_shell->setVertexScalars(s, 0, 4);
}
void MeshWidget::readMesh() {
  

  QString qfname =
      QFileDialog::getOpenFileName(0, "Open mesh file", _directory_path,
                                   "OVM files(*.ovm);;Abaqus inp files(*.inp)");

  if (qfname == "") {
    return;
  }

	ui->pushButton_read->setDisabled(true);

  /********** save last opened path *************/

  int i = qfname.lastIndexOf('/');
  _directory_path = qfname.left(i);

  // meshtools::MeshProcessingTools::separateFilename(qfname.toStdString());
  std::ifstream fin(qfname.toStdString());

  /********** Mesh *************/

  _shell = std::shared_ptr<MeshShell>(new MeshShell(_viewer));
  _shell->readMesh(qfname.toStdString());
  _shell->drawMesh(getRenderStyle());
  updateMeshInfo();
}