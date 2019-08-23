#pragma once

#include "src/mesh_shell.h"

#include <QWidget>
#include <QFile>
#include <QFileDialog>

#include <memory>

namespace Ui
{
class MeshWidget;
}

class MeshWidget : public QWidget
{
	Q_OBJECT
public:
	MeshWidget(QWidget *parent);
	//QVtkMeshWidget(VMeshPtr mesh, QVTKOpenGLWidget *widget, QWidget *parent);
	~MeshWidget();

private:
	Ui::MeshWidget *ui;

	void addSlot();

	/*VTK */

	VtkWrapper *_viewer;
	std::shared_ptr<MeshShell> _mesh;

	bool _mesh_loaded = false;

	/* functions */

	void renderMesh();

	void message(QString news);

	/* properties */
#ifdef __linux
	QString _directory_path = "~/";
#endif
#ifdef WIN32
	QString _directory_path = "c:\\";
#endif

private slots:
	void readMesh();
};