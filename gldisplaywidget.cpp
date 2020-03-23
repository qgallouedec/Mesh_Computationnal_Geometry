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
void glVertexDraw(const Vertex &p)
{
    glVertex3f(p.x(), p.y(), p.z());
}

GLDisplayWidget::GLDisplayWidget(QWidget *parent) : QGLWidget(parent)
{
    // Initial point of view is set by _X, _Y, _Z
    _X = 0;
    _Y = 0;
    _Z = 0;

    // The angle of the point of view.
    // You can rotate the shape by clicking and dragging on it.
    _angle = 0;

    // When show_vertices is false, the vertices are not displayed
    // When it becomes true, the vertices are displayed.
    // You can switch from one value to another with the checkboxes on the GUI.
    show_vertecis = false;
    show_edges = false;
    show_faces = false;
    show_laplacian = false;

    // The higher the value of this variable,
    // the longer the representation of the Laplacian is extended
    coef_laplacian = 0;

    // Update the scene
    connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    _timer.start(16);
}

void GLDisplayWidget::initializeGL()
{
    // Background color
    glClearColor(0.2, 0.2, 0.2, 1);

    // Shader
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // Construction of the mesh before it is displayed
    // Replace the values of the following two variables with
    // the path to the off-file of your choice
    // --------------------------------------------------------------------------------------
    char path_to_off_files[512] = "/path/to/directory/Mesh_Computationnal_Geometry/off_files/";
    char off_filename[64] = "your_off_file.off";
    // --------------------------------------------------------------------------------------

    // Building the full path to the off file.
    char path_to_off_file[512] = "";
    strcat(path_to_off_file, path_to_off_files);
    strcat(path_to_off_file, off_filename);

    // Construction of the mesh before it is displayed
    _mesh.parseFile(path_to_off_file);
    _mesh.sew();

    //Choose which action to apply to the selected file.
    // --------------------------------------------------------------------------------------
    _mesh.computeLaplacian();
    //_mesh.naiveInsertion();
    //_mesh.naiveInsertionAndLawson();
    // --------------------------------------------------------------------------------------
}

void GLDisplayWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Center the camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
    //GLKMatrix4MakeLookAt(0,0,5,  0,0,0,   0,1,0);

    // Translation
    glTranslated(_X, _Y, _Z);

    // Rotation
    glRotatef(_angle, 1.0f, 1.0f, 0.0f);

    // Color for your mesh
    glColor3f(0, 1, 0);

    // For each refresh of the window, vertices are displayed
    // according to the value of draw_vertices.
    if (show_vertecis)
    {
        drawVertices();
    }
    // It works the same for edges, faces and laplacian
    if (show_edges)
    {
        drawEdges();
    }
    if (show_faces)
    {
        drawFaces();
    }
    if (show_laplacian)
    {
        drawLaplacian();
    }
}

void GLDisplayWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    //GLKMatrix4MakePerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
    updateGL();
}

// Draw functions
void GLDisplayWidget::drawVertices()
{
    // Draws all the vertices of _mesh
    glColor3f(1, 1, 1); // Choose the color white
    for (int i_vertex = 0; i_vertex < _mesh.nb_vertex; i_vertex++)
    {
        glBegin(GL_POINTS);
        glVertexDraw(_mesh.verticesTab[i_vertex]);
        glEnd();
    }
}

void GLDisplayWidget::drawEdges()
{
    // Draws all the edges of _mesh
    glColor3f(1, 0, 0); // Choose the color red
    for (int i_face = 0; i_face < _mesh.nb_faces; i_face++)
    {
        Face face = _mesh.facesTab[i_face];
        for (int i_vertex_in_triangle = 0; i_vertex_in_triangle < 3; i_vertex_in_triangle++)
        { // For every face of _mesh.facesTab
            // It draws a line between one vertex and the next.
            glBegin(GL_LINE_STRIP);
            glVertexDraw(_mesh.verticesTab[face.i_vertex[i_vertex_in_triangle % 3]]);
            glVertexDraw(_mesh.verticesTab[face.i_vertex[(i_vertex_in_triangle + 1) % 3]]);
            glEnd();
        }
    }
}

void GLDisplayWidget::drawFaces()
{
    // Draws all the faces of _mesh
    glColor3f(0, 1, 0); // Choose the color green
    for (int i_face = 0; i_face < _mesh.nb_faces; i_face++)
    { // For every face of _mesh.facesTab,
        // It draws the correspondiang triangle
        Face face = _mesh.facesTab[i_face];
        glBegin(GL_TRIANGLES);
        for (int i_vertex_in_triangle = 0; i_vertex_in_triangle < 3; i_vertex_in_triangle++)
        {
            glVertexDraw(_mesh.verticesTab[face.i_vertex[i_vertex_in_triangle]]);
        }
        glEnd();
    }
}

void GLDisplayWidget::drawLaplacian()
{
    // Draws a representation of the Laplacian for all vertices of _mesh.
    glColor3f(0, 0, 1); // Choose the color blue
    for (int i_vertex = 0; i_vertex < _mesh.nb_vertex; i_vertex++)
    { // For every face of _mesh.facesTab,
        Vertex &vertex = _mesh.verticesTab[i_vertex]; // the vertex
        Vertex &dir = _mesh.laplacianTab[i_vertex]; // the laplacian

        // Construct a line between the vertex and another distant
        // point in the direction of the Laplacian.
        glBegin(GL_LINE_STRIP);
        glVertexDraw(vertex);
        glVertexDraw(vertex + dir * coef_laplacian);
        glEnd();
    }
}

// - - - - - - - - - - - - Mouse Management  - - - - - - - - - - - - - - - -
// When you click, the position of your mouse is saved
void GLDisplayWidget::mousePressEvent(QMouseEvent *event)
{
    if (event != NULL)
        _lastPosMouse = event->pos();
}

// Mouse movement management
void GLDisplayWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - _lastPosMouse.x();
    // int dy = event->y() - lastPosMouse.y();

    if (event != NULL)
    {
        _angle += dx;
        _lastPosMouse = event->pos();

        updateGL();
    }
}

// Mouse Management for the zoom
void GLDisplayWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta();
    double stepZoom = 0.1;
    if (!numDegrees.isNull())
    {
        _Z = (numDegrees.x() > 0 || numDegrees.y() > 0) ? _Z + stepZoom : _Z - stepZoom;
    }
}
