# Readme
## Դ�ļ�˵��
### mesh_tools
���ļ�����һ������������Ŀ.  
��Ŀ¼�е�mesh_wrapper.h��Ϊ�ⲿ��Ŀ���õ�ͷ�ļ�,ֻ�ṩ�����ӿ�.Ϊ�˼���������ͱ��ڰ�װΪpython��,����Eigen��û�����������Ŀ�.�����ʵ����MeshImpl��PrincipalStressField���������ṩ.  
mesh_wrapper.cxx��Ҳ���ṩ���־����ʵ��,����MeshImpl��PrincipalStressField�Ľ���,��.
### OpenVolumeMesh
���Ŀ¼������OpenVolumeMesh��Դ����,��˲���Ҫ�ٰ���OVM����.

### MeshShell
#### src
��Ҫ�ļ�Ϊmesh_shell.h/cxx,���ڽ��mesh_tools����Ⱦ����.  
view_manager_fwd.h��Ϊ������ViewManager������,����MeshShell��Ҫ����Ⱦ�ӿ�.
#### view_tools
���Ŀ¼�е��ļ��ṩ��Ⱦ�ľ��巽��.  
VtkWrapper�̳���ViewManager��ʹ��VTK��ʵ����ӿ�.  
#### widgets
���Ŀ¼�е�Դ�ļ��ṩ��ͼ�λ�����.

## ������
1. ��������  
1.1 VTK  
�汾��>=8.90  
1.2 boost  
1.3 Eigen3

2. �½���������:  
Ϊ����cmake�ҵ���,windows�¿������û�������.

������|����
-|-
EIGEN_DIR | Eigen3��װĿ¼|
Qt5_DIR | Qt5��cmake�ļ���Ŀ¼[^footnote]

[^footnote]: �������ҵ�windows�������µ�Ŀ¼D:\Qt\Qt5.9.8\5.9.8\msvc2017_64\lib\cmake\Qt5

### vtk
VTK��һ��ͼ����Ⱦ��.  
[Github](https://github.com/Kitware/VTK.git)  
����Դ����,Ȼ��ͨ��cmake���밲װ.ע�����µ�ѡ��Ҫ����ΪYES  
![](./doc/vtk01.jpg)
![](./doc/vtk02.jpg)
![](./doc/vtk03.jpg)

��release��debug�汾�����밲װ.  
�ڰ�װĿ¼/VTK/bin/qt5/plugins/designerĿ¼���ҵ�QVTKWidgetPlugin.dll,����ŵ�qt5��Ŀ¼/5.9.8/msvc2017_64/plugins/designer��

## ��װ
### windows
ʹ��cmake����visual studio����  
Where is the source code: Դ�ļ��ĸ�Ŀ¼  
Where to build the binaries: VS��������Ŀ¼
![](./doc/cmake001.jpg)

### Linux
ֱ��ʹ��cmake����


