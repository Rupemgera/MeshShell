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
	connect(this->ui->pushButton_readStressFile, &QPushButton::clicked, this,
		&MeshWidget::readStressField);
	connect(this->ui->checkBox_stressSingularity,&QCheckBox::toggled,this,
		&MeshWidget::stressSingularity);
	connect(this->ui->checkBox_render_stress,&QCheckBox::toggled,this,
		&MeshWidget::drawStressField);
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

std::string MeshWidget::filenameFromDialog(const char* dialog_name, const char *filter) {
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

void MeshWidget::readStressField() {
	std::string filename = filenameFromDialog("Open Stress File", "csv files(*.csv)");

	ui->pushButton_readStressFile->setDisabled(true);

	_shell->readStressField(filename);

	std::cout<<"read Stress Field down"<<std::endl;
}

void MeshWidget::drawStressField() {
	bool mjr,mdd,mnr;
	mjr = ui->checkBox_major->isChecked();
	mdd = ui->checkBox_mid->isChecked();
	mnr = ui->checkBox_minor->isChecked();
	_shell->drawStressField(mjr,mdd,mnr);
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
void MeshWidget::stressSingularity() {
	double tolerance = ui->doubleSpinBox_stressSingularityTolerance->value();
	_shell->stressSingularity(tolerance);
	std::cout<<"singularities calculation down"<<std::endl;
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
	std::string filename = filenameFromDialog("Open Mesh File", "OVM files(*.ovm);;Abaqus inp files(*.inp)"); 

	ui->pushButton_read->setDisabled(true);

  /********** Mesh *************/

  _shell = std::shared_ptr<MeshShell>(new MeshShell(_viewer));
  _shell->readMesh(filename);
  _shell->drawMesh(getRenderStyle());
  updateMeshInfo();
}