#include "gldisplaywidget.h"
#ifdef __APPLE__
    #include <glu.h>
#else
    #include <GL/glu.h>
#endif
#include <iostream>
#include "QDebug"


// The following functions could be displaced into a module OpenGLDisplayMesh that would include Mesh
// Draw a vertex
void glVertexDraw(const Vertex & p) {
    glVertex3f(p.x(), p.y(), p.z());
}

GLDisplayWidget::GLDisplayWidget(QWidget *parent) : QGLWidget(parent)
{
    _X = 0;
    _Y = 0;
    _Z = 0;

    show_vertecis = false;
    show_edges = false;
    show_faces = false;
    show_laplacian = false;
    coef_laplacian = 0;
    _angle = 0;

    // Update the scene
    connect( &_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    _timer.start(16);
}

void GLDisplayWidget::initializeGL()
{
    // background color
    glClearColor(0.2, 0.2, 0.2, 1);

    // Shader
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // Construction of the mesh before it is displayed
    _mesh.parseFile("/Users/William/Desktop/ECL/SCOLAIRE/3A/OPTION ET MOS/CMGIG/Fichiers terrains OFF/franketest.off");
    //_mesh.parseFile("/Users/quentingallouedec/GitHub/Mesh_Computationnal_Geometry/off_files/cube.off");
    _mesh.sew();

    //_mesh.computeLaplacian();
    _mesh.naiveInsertion();
}

void GLDisplayWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Center the camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,5,  0,0,0,   0,1,0);
    //GLKMatrix4MakeLookAt(0,0,5,  0,0,0,   0,1,0);

    // Translation
    glTranslated(_X, _Y, _Z);

    // Rotation
    glRotatef(_angle, 1.0f, 1.0f, 0.0f);

    // Color for your mesh
    glColor3f(0, 1 ,0);

    if(show_vertecis){drawVertices();}
    if(show_edges){drawEdges();}
    if(show_faces){drawFaces();}
    if(show_laplacian){drawLaplacian();}
}

void GLDisplayWidget::resizeGL(int width, int height){

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
    //GLKMatrix4MakePerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
    updateGL();
}

// Draw functions
void GLDisplayWidget::drawVertices() {
    for(int i_vertex = 0; i_vertex < _mesh.nb_vertex; i_vertex++) {
        glBegin(GL_POINTS);
        glVertexDraw(_mesh.verticesTab[i_vertex]);
        glEnd();
    }
}

void GLDisplayWidget::drawEdges() {
    for(int i_face = 0; i_face < _mesh.nb_faces; i_face++) {
        Face face = _mesh.facesTab[i_face];
        for(int i_vertex_in_triangle = 0; i_vertex_in_triangle < 3; i_vertex_in_triangle++)
        {
            glBegin(GL_LINE_STRIP);
                glVertexDraw(_mesh.verticesTab[face.i_vertex[i_vertex_in_triangle%3]]);
                glVertexDraw(_mesh.verticesTab[face.i_vertex[(i_vertex_in_triangle+1)%3]]);
            glEnd();
        }
    }
}

void GLDisplayWidget::drawFaces()
{
    for(int i_face = 0; i_face < _mesh.nb_faces; i_face++) {
        Face face = _mesh.facesTab[i_face];
        glColor3f(0, 1 ,0);
        glBegin(GL_TRIANGLES);
        for(int i_vertex_in_triangle=0; i_vertex_in_triangle<3; i_vertex_in_triangle++)
        {
            glVertexDraw(_mesh.verticesTab[face.i_vertex[i_vertex_in_triangle]]);
        }
        glEnd();
    }
}

void GLDisplayWidget::drawLaplacian() {
    glColor3f(0, 0 ,1);
    for(int i_vertex = 0; i_vertex < _mesh.nb_vertex; i_vertex++) {
        Vertex &vertex = _mesh.verticesTab[i_vertex];
        Vertex &dir = _mesh.laplacianTab[i_vertex];

        glBegin(GL_LINE_STRIP);
        glVertexDraw(vertex);
        glVertexDraw(vertex + dir*coef_laplacian);
        glEnd();
    }
}

// - - - - - - - - - - - - Mouse Management  - - - - - - - - - - - - - - - -
// When you click, the position of your mouse is saved
void GLDisplayWidget::mousePressEvent(QMouseEvent *event)
{
    if( event != NULL )
        _lastPosMouse = event->pos();
}

// Mouse movement management
void GLDisplayWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - _lastPosMouse.x();
    // int dy = event->y() - lastPosMouse.y();

    if( event != NULL )
    {
        _angle += dx;
        _lastPosMouse = event->pos();

        updateGL();
    }
}

// Mouse Management for the zoom
void GLDisplayWidget::wheelEvent(QWheelEvent *event) {
    QPoint numDegrees = event->angleDelta();
    double stepZoom = 0.25;
    if (!numDegrees.isNull())
    {
      _Z = (numDegrees.x() > 0 || numDegrees.y() > 0) ? _Z + stepZoom : _Z - stepZoom;
    }
}
