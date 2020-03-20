#ifndef MESH_H
#define MESH_H

#include <QGLWidget>
#include <iostream>
#include <QQueue>
#include <QList>

#include<cmath>

class Vertex
{
public:
    // constructors
    Vertex();
    Vertex(double x_, double y_, double z_);

    // Les getters
    double x() const;
    double y() const;
    double z() const;

    int i_incident_face;

    double getNorm() const { return std::sqrt(_x*_x + _y*_y + _z*_z) ;}
    double operator*(const Vertex &p) const { return _x*p._x + _y*p._y + _z*p._z;}
    Vertex operator*(double a) const { return Vertex(_x*a, _y*a, _z*a);}
    Vertex operator/(double a) const { return Vertex(_x/a, _y/a, _z/a);}
    Vertex operator+(const Vertex &p) const { return Vertex(_x+p._x, _y+p._y, _z+p._z);}
    Vertex operator-(const Vertex &p) const { return Vertex(_x-p._x, _y-p._y, _z-p._z);}
    bool operator!=(const Vertex other_vertex) const;

    // produit vectoriel par *p à droite
    Vertex cross(const Vertex &p) const;
    Vertex getNormalized() const;

private:
    double _x;
    double _y;
    double _z;

};

class Face
{
public:
    Face();
    Face(int i_vertex0_, int i_vertex1_, int i_vertex2_);

    int i_vertex[3];
    int adjacent_faces[3];
};

class Circulator_on_faces;

class Circulator_on_vertices;

class Mesh
{
public:
    // constructor
    Mesh();

    //Other methods
    void parseFile(const char file[]);
    void sew();

    // public attribut
    int nb_vertex, nb_faces;
    QVector<Vertex> verticesTab;
    QVector<Face> facesTab;
    QVector<Vertex> laplacianTab;

    QVector<Vertex>::iterator it_on_vertices;
    Vertex * vertices_begin();
    Vertex * vertices_past_the_end();

    QVector<Face>::iterator it_on_faces;
    Face * faces_begin();
    Face * faces_past_the_end();

    Circulator_on_faces incident_faces(int i_vertex); // returning a Circulator_on_faces positioned on one of the incident faces at vertex v.
    Circulator_on_vertices adjacent_vertices(int i_vertex);
    void computeLaplacian();
    void test();

    double vertexInCircumscribingCircle(Face face, Vertex P);
    bool isDelaunay(int i_face1, int i_vertex_oppose_1);
    QList<std::pair<int, int>> flipEdge(int i_face1, int i_vertex_oppose_1_initial);
    double orientationTest(Vertex A, Vertex B, Vertex C);
    double inTriangleTest(Face face, Vertex P);
    void insertionTriangle(int i_P, int i_face);
    void insertionInArete(int i_face1, int i_P);
    void naiveInsertion();
    bool areteEnBordure(int i_face, int i_vertex);
    std::pair<int, int> areteSymetrique(std::pair<int, int> face_et_vertex);
    void lawsonAroundVertex(int i_P);
};

class Circulator_on_faces
{ //Special topological iterator used to iterate counterclockwise on the set of incident faces at a given vertex.
public:
    Circulator_on_faces();
    // In your implementation, the iterator will contain
    // an access to the associated triangulation
    // an access to the vertex around which it turns
    // an access to the current face which can be materialized by an index.
    Circulator_on_faces(Mesh *p_mesh_, int i_vertex_);

    Mesh * p_mesh;
    int i_vertex;
    int i_face;

    Circulator_on_faces begin() const;

    Circulator_on_faces &operator++(); // switch from the current face to the next one
    Face &operator*();// returning a reference on the current face.
    bool operator!=(const Circulator_on_faces other_circulator_on_faces);

};

class Circulator_on_vertices
{
public:
    Circulator_on_vertices(Mesh *p_mesh_, int i_vertex_);

    Mesh * p_mesh;
    int i_vertex; // Le vertex autour duquel on tourne

    Circulator_on_faces circulator_on_faces; // Un circulateur sur les faces
    int current_i_vertex; // L'id du vertex courant
    int next_i_vertex; // Le suivant

    Circulator_on_vertices begin() const;

    Circulator_on_vertices &operator++(); // switch from the current vertex to the next one
    Vertex &operator*();// returning a reference on the current vertex.
    bool operator!=(const Circulator_on_vertices other_circulator_on_vertices);


//    Circulator_on_vertices: Iterator used to iterate, counter-clokwise, on all the
//    vertices that are adjacent to a vertex. The class Mesh should contain some
//    functions adjacent_vertices(Sommet & v) returning a
//    Circulator_on_vertices positioned on one of the vertices adjacent to vertex
//    v.
};

#endif // MESH_H
