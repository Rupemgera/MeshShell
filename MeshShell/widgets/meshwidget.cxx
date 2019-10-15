#include "meshwidget.h"
#include "ui_meshwidget.h"
#include <qcolordialog.h>
#include <qmessagebox.h>
MeshWidget::MeshWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::MeshWidget) {
  ui->setupUi(this);

  _viewer = new viewtools::VtkWrapper(ui->viewerWidget);

  _shell = std::shared_ptr<MeshShell>(new MeshShell(_viewer));

  addSlot();
}

MeshWidget::MeshWidget(QWidget *parent, std::string mesh_file,
                       std::string stress_file)
    : QWidget(parent), ui(new Ui::MeshWidget) {
  ui->setupUi(this);
  _viewer = new viewtools::VtkWrapper(ui->viewerWidget);

  _shell = std::shared_ptr<MeshShell>(new MeshShell(_viewer));

  addSlot();

  /********** Mesh *************/

  ui->pushButton_read->setDisabled(true);
  ui->pushButton_readStressFile->setDisabled(true);
  ui->pushButton_readCombination->setDisabled(true);

  _shell->readMesh(mesh_file);
  _shell->drawMesh(getRenderStyle());
  updateMeshOpacity();
  updateMeshInfo();

  /******** stress ************/

  _shell->readStressField(stress_file);
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
  /*connect(this->ui->pushButton_refresh_listWidget, &QPushButton::clicked,
     this, &MeshWidget::on_pushButton_refresh_listWidget_clicked);*/
  /* connect(this->ui->listWidget,&QListWidget::itemChanged,this,&MeshWidget::on_listWidget_item_changed);*/

  /* draw stress field */
  connect(this->ui->checkBox_render_stress, &QCheckBox::toggled, this,
          &MeshWidget::drawStressField);
  connect(this->ui->checkBox_major, &QCheckBox::toggled, this,
          &MeshWidget::drawStressField);
  connect(this->ui->checkBox_mid, &QCheckBox::toggled, this,
          &MeshWidget::drawStressField);
  connect(this->ui->checkBox_minor, &QCheckBox::toggled, this,
          &MeshWidget::drawStressField);
  /*connect(this->ui->checkBox_smoothed_major, &QCheckBox::toggled, this,
          &MeshWidget::drawStressField);*/

  /* sigularity */
  connect(this->ui->checkBox_stressSingularity, &QCheckBox::toggled, this,
          &MeshWidget::toggleStressSingularity);
  connect(this->ui->pushButton_singularityRefresh, &QPushButton::clicked, this,
          &MeshWidget::updateStressSingularity);
  connect(this->ui->pushButton_split_refresh, &QPushButton::clicked, this,
          &MeshWidget::splitFaces);
  connect(this->ui->pushButton_singular_edges_refresh, &QPushButton::clicked,
          this, &MeshWidget::extractSingularLines);
  // connect(this->ui->checkBox_render_singular_edges, &QCheckBox::toggled,
  // this,
  //       &MeshWidget::on_checkBox_render_singular_edges_toggled);
  /*connect(this->ui->checkBox_render_splited_face, &QCheckBox::toggled, this,
          &MeshWidget::on_checkBox_render_splited_face_toggled);*/
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
void MeshWidget::toggleStressSingularity() {
  if (ui->checkBox_stressSingularity->isChecked()) {
    _viewer->setVisibility("Singularity", true);
    ui->doubleSpinBox_pointSize->setDisabled(false);
    ui->doubleSpinBox_stressSingularityTolerance->setDisabled(false);
  } else {
    _viewer->setVisibility("Singularity", false);
    ui->doubleSpinBox_pointSize->setDisabled(true);
    ui->doubleSpinBox_stressSingularityTolerance->setDisabled(true);
  }
  // std::cout<<"singularities calculation down"<<std::endl;
}
void MeshWidget::updateStressSingularity() {

  double tolerance = ui->doubleSpinBox_stressSingularityTolerance->value();
  double pointSize = ui->doubleSpinBox_pointSize->value();
  _shell->stressSingularity(tolerance, pointSize);
  toggleStressSingularity();
}

void MeshWidget::splitFaces() {
  double tolerance = ui->doubleSpinBox_split_tolerance->value();
  /*if (ui->checkBox_render_split_faces->isChecked()) {
    _shell->splitFaces(tolerance);
  } else{
  _shell->setVisibility("split_faces",false);}*/
  std::string name = _shell->splitFaces(tolerance);

  active_actors["splited_faces"] = name;

  ui->checkBox_render_splited_faces->setChecked(true);
}

void MeshWidget::on_checkBox_render_splited_faces_toggled() {
  if (ui->checkBox_render_splited_faces->isChecked()) {
    _viewer->setVisibility(active_actors["splited_faces"], true);
  } else {
    _viewer->setVisibility(active_actors["splited_faces"], false);
  }
  _viewer->refresh();
}

void MeshWidget::extractSingularLines() {
  std::string name = _shell->extractSingularLines();

  active_actors["singular_edges"] = name;

  ui->checkBox_render_singular_edges->setChecked(true);
}

void MeshWidget::on_checkBox_render_singular_edges_toggled() {
  if (ui->checkBox_render_singular_edges->isChecked()) {
    _viewer->setVisibility(active_actors["singular_edges"], true);
  } else {
    _viewer->setVisibility(active_actors["singular_edges"], false);
  }
  _viewer->refresh();
}

void MeshWidget::test() { _shell->test(); }

void MeshWidget::on_pushButton_refresh_listWidget_clicked() {
  auto additem = [&](viewtools::ActorControler *ac) {
    QListWidgetItem *new_item = new QListWidgetItem(ac->name_.c_str());
    /*new_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable |
                       Qt::ItemIsEnabled);*/
    new_item->setFlags(new_item->flags() | Qt::ItemIsUserCheckable);

    if (ac->getVisibility())
      new_item->setCheckState(Qt::Checked);
    else
      new_item->setCheckState(Qt::Unchecked);
    ui->listWidget->addItem(new_item);
  };

  ui->listWidget->clear();

  for (auto &ac : _viewer->getTable()) {
    // mesh render is not added
    if (ac.second->name_.find("mesh") == ac.second->name_.npos)
      additem(ac.second);
  }
}

void MeshWidget::on_listWidget_itemChanged(QListWidgetItem *item) {
  std::string name = item->text().toStdString();
  if (item->checkState() == Qt::Checked) {
    _viewer->setVisibility(name, true);
  } else {
    _viewer->setVisibility(name, false);
  }
  _viewer->refresh();
}

// void MeshWidget::on_listWidget_itemSelectionChanged() {
//  /* auto current_item = ui->listWidget->currentItem();
//   std::string name = current_item->text().toStdString();
//   auto status = _viewer->getStatus(name);
//   ui->actor_opacity->setValue(std::get<0>(status));
//   ui->actor_size->setValue(std::get<1>(status));*/
//  //std::cout << "item selection changed" << std::endl;
//}

void MeshWidget::on_listWidget_itemClicked(QListWidgetItem *item) {
  std::string name = item->text().toStdString();
  auto status = _viewer->getStatus(name);
  ui->actor_opacity->setValue(std::get<0>(status));
  ui->actor_size->setValue(std::get<1>(status));

  auto c = _viewer->getColor(name);
  QColor color(std::get<0>(c), std::get<1>(c), std::get<2>(c));
  ui->color_frame->setPalette(QPalette(color));
  // std::cout << name << " clicked" << std::endl;
}

void MeshWidget::on_pushButton_actor_color_clicked() {
  QColor color = QColorDialog::getColor();
  if (color.isValid()) {
    // set actor's color
    double red = color.red();
    double green = color.green();
    double blue = color.blue();
    double c[] = {red / 255.0, green / 255.0, blue / 255.0};
    auto item = ui->listWidget->currentItem();
    std::string name = item->text().toStdString();
    _viewer->setColor(name, c);

    // gui changes
    ui->color_frame->setPalette(QPalette(color));
  }
}

void MeshWidget::on_actor_refresh_clicked() {
  double size = ui->actor_size->value();
  double opacity = ui->actor_opacity->value();
  auto item = ui->listWidget->currentItem();
  std::string name = item->text().toStdString();
  _viewer->setSize(name, size);
  _viewer->setOpacity(name, opacity);
}

void MeshWidget::on_checkBox_smoothed_major_toggled() {
  _viewer->setVisibility("smoothed_field",
                         ui->checkBox_smoothed_major->isChecked());
}

void MeshWidget::on_pushButton_smooth_clicked() {
  _shell->smoothStressField(0);
  ui->checkBox_smoothed_major->setDisabled(false);
}
