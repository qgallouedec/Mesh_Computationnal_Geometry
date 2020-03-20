#include "mesh.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <QQueue>
#include <QList>

// ------------Functions-----------
double cotan(const Vertex vertex1, const Vertex vertex2)
{
    double cosalpha = (vertex1*vertex2)/(vertex1.getNorm()*vertex2.getNorm());
    return cosalpha/sqrt(1-cosalpha*cosalpha);
}

// --------------------Point---------------------
// Constructeur
Vertex::Vertex() // Les constructeur par défaut
{
    _x = 0;
    _y = 0;
    _z = 0;
}

Vertex::Vertex(double x_, double y_, double z_) // Le constructeur avec les données explicites
{
    _x = x_;
    _y = y_;
    _z = z_;
}

// Getters
double Vertex::x() const
{
    return _x;
}

double Vertex::y() const
{
    return _y;
}

double Vertex::z() const
{
    return _z;
}

// other functions
Vertex Vertex::getNormalized() const
{
    double norm = getNorm();
    return Vertex(_x/norm, _y/norm, _z/norm);
}

bool Vertex::operator!=(const Vertex other_vertex) const
{
    return ((x() != other_vertex.x()) | (y() != other_vertex.y()) | (z() != other_vertex.z()));
}

Vertex Vertex::cross(const Vertex &other_vertex) const
{
    double result_x = _y * other_vertex.z() - _z * other_vertex.y();
    double result_y = _z * other_vertex.x() - _x * other_vertex.z();
    double result_z = _x * other_vertex.y() - _y * other_vertex.x();
    return Vertex(result_x, result_y, result_z);
}

// ----------------------Mesh--------------------
// constructor
Mesh::Mesh()
{}

// other methods
void Mesh::parseFile(const char file_name[])
{
    // Lecture du fichier, et stockage dans verticesTab et facesTab
    FILE * pFile;
    pFile = fopen (file_name,"r");
    if (pFile!=NULL)
    {
        std::cout << "file openend" << std::endl;
    }
    else
    {
        std::cout << "file " << file_name << " not opened" << std::endl;
    }

    verticesTab.clear();
    int nb_edge = 0;
    fscanf(pFile, "%d %d %d\n", &nb_vertex, &nb_faces, &nb_edge);
    std::cout << "nb points: "<< nb_vertex << ", nb faces: " << nb_faces  << std::endl;

    verticesTab.reserve(nb_vertex);
    facesTab.reserve(nb_faces);

    double x, y, z;
    for(int i_vertex = 0; i_vertex < nb_vertex; i_vertex++)
    {
        fscanf(pFile, "%lf %lf %lf\n", &x, &y, &z); // Stockage de la ligne lue
        // MODIF !!!!!!!!!
        verticesTab.push_back(Vertex(x-.5,y-.5,0)); // Ajout du point dans le vecteur verticesTab
    }

    int n_face, i_vertex0, i_vertex1, i_vertex2;
    for(int i_face = 0; i_face < nb_faces; i_face++)
    {
        fscanf(pFile, "%d %d %d %d\n", &n_face, &i_vertex0, &i_vertex1, &i_vertex2);
        facesTab.push_back(Face(i_vertex0, i_vertex1, i_vertex2));
    }
    fclose(pFile);
    std::cout << "end of reading" << std::endl;
}

void Mesh::sew()
{
    // vert_done est un liste de boolean. Si vert_done[5] == true, ça signifie que le vertex 5 à été traité
    std::cout << "begin sewing"<< std::endl;
    bool vert_done[nb_vertex];
    for(int i_vertex = 0; i_vertex < nb_vertex; i_vertex++)
    {
        vert_done[i_vertex] = false;
    }

    // Map est un dict. ici : {(int, int) : (int, int), ...}
    std::map< std::pair<int,int>, std::pair<int,int> > myMap;

    for(int i_face = 0; i_face < nb_faces; i_face++) {
        // On se balade dans toutes les faces définie, et on récupère les indices des trois points(=vertex)
        Face * face = &facesTab[i_face]; // Un pointeur qui pointe vers la i_eme face

        // On regarde si on a déja traité les vertex(=points). Si non, on dit qu'on les a traité
        // puis on ajoute l'indice de la face comme face incidente au vertex(=point)
        for(int i_vertex_in_triangle=0; i_vertex_in_triangle<3; i_vertex_in_triangle++)
        {
            int i_vertex = face->i_vertex[i_vertex_in_triangle];
            if(!vert_done[i_vertex])
                {
                    verticesTab[i_vertex].i_incident_face=i_face; // On ajoute à l'objet vertex une face inscidente
                    vert_done[i_vertex]=true; // On le considère traité

                };
        }

        // setting adjacent faces on each edge:
        std::pair <int, int> edge;      //  Arrêtes
        std::pair <int, int> i_other_face_and_i_other_vertex; //  {face id whose edge was'nt yet register in the map,  its opposed vertex id (0,1 or 2) }

        // Order vertex
        for(int i_vertex_in_triangle = 0; i_vertex_in_triangle<3; i_vertex_in_triangle++)
        {
            if (face->i_vertex[(i_vertex_in_triangle+1)%3] < face->i_vertex[(i_vertex_in_triangle+2)%3])
            {
                edge = {face->i_vertex[(i_vertex_in_triangle+1)%3] , face->i_vertex[(i_vertex_in_triangle+2)%3]};
            }
            else
            {
                edge = {face->i_vertex[(i_vertex_in_triangle+2)%3] , face->i_vertex[(i_vertex_in_triangle+1)%3]};
            }
            // edge est la pair d'indice dans l'ordre croissant de l'arrète

            if(myMap.find(edge) == myMap.end())
            {// Si l'arrète n'est pas dans la map
                // On l'ajoute
                myMap[edge] = {i_face, i_vertex_in_triangle}; // (i_vertex0, i_vertex1) : (i_face, i_vertex) (c'est le vertex opposé)
            }
            else
            { // L'arrête appartient déja à un autre triangle de la map
                i_other_face_and_i_other_vertex = myMap.at(edge); //(i_face de l'autre triangle, i_face dans l'autre triangle (0,1,2))
                int i_other_face = i_other_face_and_i_other_vertex.first;
                int i_other_vertex = i_other_face_and_i_other_vertex.second;

                face->adjacent_faces[i_vertex_in_triangle] = i_other_face; // On stocke l'indice de la face adjacente opposé au vertex(=point) n°2
                // On fait la même chose à l'autre triangle
                facesTab[i_other_face].adjacent_faces[i_other_vertex] = i_face;
            }
        }
    }
    std::cout << "end sewing" << std::endl;
}

void Mesh::test()
{
    // Vertex A = Vertex(0.0, 0.0, 0.0);
    // Vertex B = Vertex(5.0, 0.0, 0.0);
    // Vertex C = Vertex(10.0, 0.0, 0.0);

    // double value = orientationTest(A, B, C);
    // insersionTriangle(8, 2);

    // std::cout << "adjacent_faces de 2 : " << facesTab[2].adjacent_faces[0]<< ", " << facesTab[2].adjacent_faces[1]<< ", " << facesTab[2].adjacent_faces[2] << std::endl;
    // std::cout << "vertices de 2 : " << facesTab[2].i_vertex[0]<< ", " << facesTab[2].i_vertex[1]<< ", " << facesTab[2].i_vertex[2] << std::endl;
    // std::cout << "adjacent_faces de 12 : " << facesTab[12].adjacent_faces[0]<< ", " << facesTab[12].adjacent_faces[1]<< ", " << facesTab[12].adjacent_faces[2] << std::endl;
    // std::cout << "vertices de 12 : " << facesTab[12].i_vertex[0]<< ", " << facesTab[12].i_vertex[1]<< ", " << facesTab[12].i_vertex[2] << std::endl;
    // std::cout << "adjacent_faces de 13 : " << facesTab[13].adjacent_faces[0]<< ", " << facesTab[13].adjacent_faces[1]<< ", " << facesTab[13].adjacent_faces[2] << std::endl;
    // std::cout << "vertices de 13 : " << facesTab[13].i_vertex[0]<< ", " << facesTab[13].i_vertex[1]<< ", " << facesTab[13].i_vertex[2] << std::endl;

    // std::cout << "i_incident_face du point 1 : " << verticesTab[1].i_incident_face << std::endl;
    // std::cout << "i_incident_face du point 2 : " << verticesTab[2].i_incident_face << std::endl;
    // std::cout << "i_incident_face du point 6 : " << verticesTab[6].i_incident_face << std::endl;
    // std::cout << "i_incident_face du point 8 : " << verticesTab[8].i_incident_face << std::endl;

    // std::cout << "adjacent_faces de 3 : " << facesTab[3].adjacent_faces[0]<< ", " << facesTab[3].adjacent_faces[1]<< ", " << facesTab[3].adjacent_faces[2] << std::endl;
    // std::cout << "adjacent_faces de 4 : " << facesTab[4].adjacent_faces[0]<< ", " << facesTab[4].adjacent_faces[1]<< ", " << facesTab[4].adjacent_faces[2] << std::endl;
    // std::cout << "adjacent_faces de 10 : " << facesTab[10].adjacent_faces[0]<< ", " << facesTab[10].adjacent_faces[1]<< ", " << facesTab[10].adjacent_faces[2] << std::endl;

    // Vertex P = Vertex(0.1, 0, 0);
    // Face face = facesTab[2];
    // double value = inTriangleTest(face, P);

    // Face face = facesTab[2];
    // flipEdge(2, 2);
    // std::cout << "===== Test isDelaunay =====" << std::endl;
    // std::cout << "x " << (verticesTab[face.i_vertex[0]]).x() << std::endl;
    // std::cout << "y " << (verticesTab[face.i_vertex[0]]).y() << std::endl;
    // std::cout << "z " << (verticesTab[face.i_vertex[0]]).z() << std::endl;
    // std::cout << result << std::endl;
}

Vertex * Mesh::vertices_begin()
{
    return verticesTab.begin();
}

Vertex * Mesh::vertices_past_the_end()
{
    return verticesTab.end();
}

Face * Mesh::faces_begin()
{
    return facesTab.begin();
}

Face * Mesh::faces_past_the_end()
{
    return facesTab.end();
}

Circulator_on_faces Mesh::incident_faces(int i_vertex)
{
    return Circulator_on_faces(this, i_vertex);
}

Circulator_on_vertices Mesh::adjacent_vertices(int i_vertex)
{
    return Circulator_on_vertices(this, i_vertex);
}

void Mesh::computeLaplacian()
{
    std::cout << "begin compute laplacian" <<std::endl;
    for(int i_vertex = 0; i_vertex < nb_vertex; i_vertex ++)
    {
        double area = 0;
        Vertex laplacian(0, 0, 0);

        Circulator_on_vertices circulator_on_vertices(this, i_vertex);

        Vertex vertexO = verticesTab[i_vertex];
        Vertex vertexA;
        Vertex vertexB;
        Vertex vertexC;

        Vertex vectorAtoO;
        Vertex vectorAtoB;
        Vertex vectorCtoO;
        Vertex vectorCtoB;
        Vertex vectorOtoB;

        vertexA = *circulator_on_vertices;
        ++circulator_on_vertices;
        vertexB = *circulator_on_vertices;
        ++circulator_on_vertices;
        vertexC = *circulator_on_vertices;

        do{
            vectorAtoO = vertexO - vertexA;
            vectorAtoB = vertexB - vertexA;
            vectorCtoO = vertexO - vertexC;
            vectorCtoB = vertexB - vertexC;
            vectorOtoB = vertexB - vertexO;

            laplacian = laplacian + vectorOtoB * (cotan(vectorAtoO, vectorAtoB) + cotan(vectorCtoO, vectorCtoB));
            area += vectorAtoO.cross(vectorAtoB).getNorm()/2 /3;

            vertexA = vertexB;
            vertexB = vertexC;
            ++circulator_on_vertices;
            vertexC = *circulator_on_vertices;

        }while(vertexA != *circulator_on_vertices.begin());

        laplacianTab.push_back(laplacian/(2*area));
    }
    std::cout << "laplacian computed" <<std::endl;
}

double Mesh::vertexInCircumscribingCircle(Face face, Vertex P)
{
    // On analyse seulement les dimensions x, y
    // On regarde en projetant sur le paraboloïde z = x*x + y*y


    Vertex A = verticesTab[face.i_vertex[0]];
    Vertex B = verticesTab[face.i_vertex[1]];
    Vertex C = verticesTab[face.i_vertex[2]];

    Vertex A_hyper = Vertex(A.x(), A.y(), A.x()*A.x() + A.y()*A.y());
    Vertex B_hyper = Vertex(B.x(), B.y(), B.x()*B.x() + B.y()*B.y());
    Vertex C_hyper = Vertex(C.x(), C.y(), C.x()*C.x() + C.y()*C.y());
    Vertex P_hyper = Vertex(P.x(), P.y(), P.x()*P.x() + P.y()*P.y());

    Vertex AB_hyper = B_hyper - A_hyper;
    Vertex AC_hyper = C_hyper - A_hyper;
    Vertex AP_hyper = P_hyper - A_hyper;

    Vertex pdtVect = AB_hyper.cross(AC_hyper);
    double pdtScal = (-1)* (pdtVect * AP_hyper);

    // si pdtScal < 0, alors P en dehors du cercle circonscrit
    // si pdtScal > 0, alors P en dans le cercle circonscrit
    return pdtScal;
}

bool Mesh::isDelaunay(int i_face1, int i_vertex_oppose_1)
{
    // On test la qualité "être de Delaunay" seulement sur le plan x, y
    Face face1 = facesTab[i_face1];
    Face face2;
    if (face1.i_vertex[0] == i_vertex_oppose_1){
        face2 = facesTab[face1.adjacent_faces[0]];
    } else if (face1.i_vertex[1] == i_vertex_oppose_1){
        face2 = facesTab[face1.adjacent_faces[1]];
    } else {
        face2 = facesTab[face1.adjacent_faces[2]];
    };
    int i_vertex_oppose_2;
    if (face2.adjacent_faces[0] == i_face1){
        i_vertex_oppose_2 = face2.i_vertex[0];
    } else if (face2.adjacent_faces[1] == i_face1){
        i_vertex_oppose_2 = face2.i_vertex[1];
    } else {
        i_vertex_oppose_2 = face2.i_vertex[2];
    };

    double test1 = vertexInCircumscribingCircle(face1, verticesTab[i_vertex_oppose_2]);
    double test2 = vertexInCircumscribingCircle(face2, verticesTab[i_vertex_oppose_1]);

    // On demande à ce que les points soient sur la bordure ou strictement à l'extérieur du cercle
    return ((test1 <= 0) && (test2 <= 0)); // STRICTEMENT
}

QList<std::pair<int, int>> Mesh::flipEdge(int i_face1, int i_vertex_oppose_1_initial){
    // Cette fonction fait le flip de l'edge en question
    // (Il faudra verifier avant de l'utiliser qu'on ne se trouve pas sur un bord)
    // Et elle renvoi les 2 aretes encadrant l'arete flipée et opposées au vertex initial dans une queue de taille 2
    // (Pour faciliter l'algorithme de Lawson)

    // Initialisation des valeurs

    Face &face1 = facesTab[i_face1];
    int &i_vertex_a = i_vertex_oppose_1_initial;
    int i_vertex_b = -1;
    int i_vertex_c = -1;
    int i_vertex_d = -1;
    int i_vertex_oppose_2_initial = -1;
    int i_face2 = -1;
    int i_face3 = -1;
    int i_face4 = -1;
    int i_face5 = -1;
    int i_face6 = -1;

    if (i_vertex_oppose_1_initial == face1.i_vertex[0]){
        i_face2 = face1.adjacent_faces[0];
        i_vertex_d = face1.i_vertex[1];
        i_vertex_b = face1.i_vertex[2];
        i_face4 = face1.adjacent_faces[1];
        i_face3 = face1.adjacent_faces[2];

    } else if (i_vertex_oppose_1_initial == face1.i_vertex[1]){
        i_face2 = face1.adjacent_faces[1];
        i_vertex_d = face1.i_vertex[2];
        i_vertex_b = face1.i_vertex[0];
        i_face4 = face1.adjacent_faces[2];
        i_face3 = face1.adjacent_faces[0];

    } else {
        i_face2 = face1.adjacent_faces[2];
        i_vertex_d = face1.i_vertex[0];
        i_vertex_b = face1.i_vertex[1];
        i_face4 = face1.adjacent_faces[0];
        i_face3 = face1.adjacent_faces[1];
    };

    Face &face2 = facesTab[i_face2];

    if (i_face1 == face2.adjacent_faces[0]){

        i_vertex_oppose_2_initial = face2.i_vertex[0];
        i_face6 = face2.adjacent_faces[1];
        i_face5 = face1.adjacent_faces[2];

    } else if (i_face1 == face2.adjacent_faces[1]){

        i_vertex_oppose_2_initial = face2.i_vertex[1];
        i_face6 = face2.adjacent_faces[2];
        i_face5 = face1.adjacent_faces[0];

    } else {

        i_vertex_oppose_2_initial = face2.i_vertex[2];
        i_face6 = face2.adjacent_faces[0];
        i_face5 = face1.adjacent_faces[1];

    };

    i_vertex_c = i_vertex_oppose_2_initial;

    // Actualisation des donnees apres le flip

    // Faces 1 et 2

    face1.i_vertex[0] = i_vertex_a;
    face1.i_vertex[1] = i_vertex_d;
    face1.i_vertex[2] = i_vertex_c;

    face1.adjacent_faces[0] = i_face6;
    face1.adjacent_faces[1] = i_face2;
    face1.adjacent_faces[2] = i_face3;

    face2.i_vertex[0] = i_vertex_c;
    face2.i_vertex[1] = i_vertex_b;
    face2.i_vertex[2] = i_vertex_a;

    face2.adjacent_faces[0] = i_face4;
    face2.adjacent_faces[1] = i_face1;
    face2.adjacent_faces[2] = i_face5;

    // Faces 3 4 5 6
    // On actualise la face adj partant du vertex à l'oppose de notre quadrilatere

    // std::cout<<"testestest"<<std::endl;

    for (int i=0; i<3; i++){
        if(i_face4>=0 && facesTab[i_face4].adjacent_faces[i] == i_face1){
            facesTab[i_face4].adjacent_faces[i] = i_face2;
        };
        if(i_face6>=0 && facesTab[i_face6].adjacent_faces[i] == i_face2){
            facesTab[i_face6].adjacent_faces[i] = i_face1;
        };
    };

    // Vertices

    // std::cout<<"testestest"<<std::endl;

    verticesTab[i_vertex_a].i_incident_face = i_face1;
    verticesTab[i_vertex_d].i_incident_face = i_face1;
    verticesTab[i_vertex_c].i_incident_face = i_face2;
    verticesTab[i_vertex_b].i_incident_face = i_face2;

    // std::cout<<"debut aretes quadrilatere : "<< i_face1 << ", "<<i_vertex_oppose_1_initial<<" ,"<<std::endl;
    QList < std::pair <int, int> > aretes_quadrilatere;
    aretes_quadrilatere.append({i_face1, i_vertex_a}); // arete f6
    //aretes_quadrilatere.append({i_face1, i_vertex_c}); // arete f3
    aretes_quadrilatere.append({i_face2, i_vertex_a}); // arete f5
    //aretes_quadrilatere.append({i_face2, i_vertex_c}); // arete f4

    return aretes_quadrilatere;
}

double Mesh::orientationTest(Vertex A, Vertex B, Vertex C){

    // Positif si orientes dans le sens trigo
    // = 0 <=> points alignés

    // On travaille dans le plan z = 0

    Vertex AB = B - A;
    Vertex BC = C - B;

    Vertex AB_proj = Vertex(AB.x(), AB.y(), 0);
    Vertex BC_proj = Vertex(BC.x(), BC.y(), 0);

    Vertex vector = AB_proj.cross(BC_proj);
    double value = vector.z();
    return value;
}

double Mesh::inTriangleTest(Face face, Vertex P){

    // Positif si on est dans le triangle, 0 sur les bords, negatif si à l'exterieur
    // On travaille dans le plan z = 0

    Vertex A = verticesTab[face.i_vertex[0]];
    Vertex B = verticesTab[face.i_vertex[1]];
    Vertex C = verticesTab[face.i_vertex[2]];

    double test1 = orientationTest(A, B, P)*nb_vertex*nb_vertex;
    double test2 = orientationTest(B, C, P)*nb_vertex*nb_vertex;
    double test3 = orientationTest(C, A, P)*nb_vertex*nb_vertex;

    if ((test1 >= 0) && (test2 >= 0) && (test3 >= 0)){
        // Renvoi val strictement positive si toutes les orientation sont strictement positive
        // Ou 0 si au moins une des orientations est nulle (ie P sur un bord)
        return test1*test2*test3;
    } else {
        // On est forcément en dehors du triangle
        // On renvoi une valeur négative
        // double value = -abs(test1*test2*test3);
        return -1; //value;
    };
}

void Mesh::insertionTriangle(int i_P, int i_face){
    Face old_face = facesTab[i_face];

    // Creer nouveaux triangles

    int i_A = old_face.i_vertex[0];
    int i_B = old_face.i_vertex[1];
    int i_C = old_face.i_vertex[2];

    // On prend les faces autour du triangle si les voisins existent
    // Si l'indice  = -1, c'est qu'il n'y a pas de face voisine (cf plus tard)

    int i_face_A = old_face.adjacent_faces[0];
    int i_face_B = old_face.adjacent_faces[1];
    int i_face_C = old_face.adjacent_faces[2];

    Vertex &A = verticesTab[i_A];
    Vertex &B = verticesTab[i_B];
    Vertex &C = verticesTab[i_C];
    Vertex &P = verticesTab[i_P];

    // Insérer ces triangles dans le tableau faces et supprimer l'ancien

    facesTab[i_face] = Face(i_A, i_B, i_P); // ABP
    facesTab.append(Face(i_B, i_C, i_P)); // BCP
    facesTab.append(Face(i_C, i_A, i_P)); // CAP

    int i_ABP = i_face;
    int i_BCP = nb_faces;
    int i_CAP = nb_faces+1;

    Face &ABP = facesTab[i_ABP];
    Face &BCP = facesTab[i_BCP];
    Face &CAP = facesTab[i_CAP];

    // Actualiser paramètres du Mesh

    nb_faces +=2;

    // Ajouter les adjacences des nouveaux triangles

    ABP.adjacent_faces[0] = i_BCP;
    ABP.adjacent_faces[1] = i_CAP; 
    ABP.adjacent_faces[2] = i_face_C;

    BCP.adjacent_faces[0] = i_CAP;
    BCP.adjacent_faces[1] = i_ABP;
    BCP.adjacent_faces[2] = i_face_A;

    CAP.adjacent_faces[0] = i_ABP;
    CAP.adjacent_faces[1] = i_BCP;
    CAP.adjacent_faces[2] = i_face_B;

    // Adapter les adjacences des triangles autour

    for(int i=0;i<3;i++){
        if (i_face_A >=0 && facesTab[i_face_A].adjacent_faces[i] == i_face) {
            facesTab[i_face_A].adjacent_faces[i] = i_BCP;
        };
        if (i_face_B >=0 && facesTab[i_face_B].adjacent_faces[i] == i_face) {
            facesTab[i_face_B].adjacent_faces[i] = i_CAP;
        };
        if (i_face_C >=0 && facesTab[i_face_C].adjacent_faces[i] == i_face) {
            facesTab[i_face_C].adjacent_faces[i] = i_ABP;
        };
    }

    // Modifier les i_incident_face des Vertices

    A.i_incident_face = i_ABP;
    B.i_incident_face = i_BCP;
    C.i_incident_face = i_CAP;
    P.i_incident_face = i_ABP;
}

void Mesh::insertionInArete(int i_face1, int i_P){
    // quand on veut inserer un point qui se trouve sur une arete

    Face face1 = facesTab[i_face1];

    int i_A = -1;
    int i_B = -1;
    int i_C = -1;
    int i_D = -1;
    int i_face2 = -1;
    int i_face3 = -1;
    int i_face4 = -1;
    int i_face5 = -1;
    int i_face6 = -1;

    if(orientationTest(verticesTab[face1.i_vertex[0]], verticesTab[face1.i_vertex[1]], verticesTab[i_P])==0){
        i_A = face1.i_vertex[2];
        i_B = face1.i_vertex[0];
        i_D = face1.i_vertex[1];
        i_face2 = face1.adjacent_faces[2];
        i_face6 = face1.adjacent_faces[0];
        i_face3 = face1.adjacent_faces[1];
    } else if (orientationTest(verticesTab[face1.i_vertex[1]], verticesTab[face1.i_vertex[2]], verticesTab[i_P])==0){
        i_A = face1.i_vertex[0];
        i_B = face1.i_vertex[1];
        i_D = face1.i_vertex[2];
        i_face2 = face1.adjacent_faces[0];
        i_face6 = face1.adjacent_faces[1];
        i_face3 = face1.adjacent_faces[2];
    } else if (orientationTest(verticesTab[face1.i_vertex[2]], verticesTab[face1.i_vertex[0]], verticesTab[i_P])==0){
        i_A = face1.i_vertex[1];
        i_B = face1.i_vertex[2];
        i_D = face1.i_vertex[0];
        i_face2 = face1.adjacent_faces[1];
        i_face6 = face1.adjacent_faces[2];
        i_face3 = face1.adjacent_faces[0];
    };

    // Tester le cas si i_face2 = -1 ?? Pas la peine pour l'instant, on gardera un cadre plus grand
    for (int i = 0; i<3; i++){
        if(facesTab[i_face2].adjacent_faces[i] == i_face1){
            i_C = facesTab[i_face2].i_vertex[i];
            i_face4 = facesTab[i_face2].adjacent_faces[(i+1)%3];
            i_face5 = facesTab[i_face2].adjacent_faces[(i+2)%3];
        };
    };


    Vertex &A = verticesTab[i_A];
    Vertex &B = verticesTab[i_B];
    Vertex &C = verticesTab[i_C];
    Vertex &D = verticesTab[i_D];
    Vertex &P = verticesTab[i_P];

    // Insérer ces triangles dans le tableau faces et supprimer l'ancien

    facesTab[i_face1] = Face(i_D, i_A, i_P); // DAP
    facesTab[i_face2] = Face(i_C, i_D, i_P); // CDP

    facesTab.append(Face(i_A, i_B, i_P)); // ABP
    facesTab.append(Face(i_B, i_C, i_P)); // BCP

    int i_DAP = i_face1;
    int i_CDP = i_face2;
    int i_ABP = nb_faces;
    int i_BCP = nb_faces+1;

    Face &DAP = facesTab[i_DAP];
    Face &CDP = facesTab[i_CDP];
    Face &ABP = facesTab[i_ABP];
    Face &BCP = facesTab[i_BCP];

    // Actualiser paramètres du Mesh

    nb_faces +=2;

    // Ajouter les adjacences des nouveaux triangles



    DAP.adjacent_faces[0] = i_ABP;
    DAP.adjacent_faces[1] = i_CDP;
    DAP.adjacent_faces[2] = i_face6;

    CDP.adjacent_faces[0] = i_DAP;
    CDP.adjacent_faces[1] = i_BCP;
    CDP.adjacent_faces[2] = i_face5;

    ABP.adjacent_faces[0] = i_BCP;
    ABP.adjacent_faces[1] = i_DAP;
    ABP.adjacent_faces[2] = i_face3;

    BCP.adjacent_faces[0] = i_CDP;
    BCP.adjacent_faces[1] = i_ABP;
    BCP.adjacent_faces[2] = i_face4;

    // Adapter les adjacences des triangles autour

    for(int i=0;i<3;i++){
        if (i_face3 >=0 && facesTab[i_face3].adjacent_faces[i] == i_face1) {
            facesTab[i_face3].adjacent_faces[i] = i_ABP;
        };
        if (i_face4 >=0 && facesTab[i_face4].adjacent_faces[i] == i_face2) {
            facesTab[i_face4].adjacent_faces[i] = i_BCP;
        };
        if (i_face5 >=0 && facesTab[i_face5].adjacent_faces[i] == i_face2) {
            facesTab[i_face5].adjacent_faces[i] = i_CDP;
        };
        if (i_face6 >=0 && facesTab[i_face6].adjacent_faces[i] == i_face1) {
            facesTab[i_face6].adjacent_faces[i] = i_DAP;
        };
    }

    // Modifier les i_incident_face des Vertices

    A.i_incident_face = i_ABP;
    B.i_incident_face = i_BCP;
    C.i_incident_face = i_CDP;
    D.i_incident_face = i_DAP;
    P.i_incident_face = i_ABP;
}

void Mesh::naiveInsertion(){
    // On ne prend pas en compte la dimension z
    // On part d'un maillage sans triangle, seulement des points

    // Tout d'abord, on crée le rectangle qui va contenir tous les points

    double x_min = verticesTab[0].x();
    double x_max = verticesTab[0].x();
    double y_min = verticesTab[0].y();
    double y_max = verticesTab[0].y();

    for(int i_vertex=1;i_vertex<nb_vertex;i_vertex++){
        if(verticesTab[i_vertex].x() < x_min){x_min = verticesTab[i_vertex].x();};
        if(verticesTab[i_vertex].x() > x_max){x_max = verticesTab[i_vertex].x();};
        if(verticesTab[i_vertex].y() < y_min){y_min = verticesTab[i_vertex].y();};
        if(verticesTab[i_vertex].y() > y_max){y_max = verticesTab[i_vertex].y();};
    };

    x_min -= (x_max-x_min)/10;
    x_max += (x_max-x_min)/10;
    y_min -= (y_max-y_min)/10;
    y_max += (y_max-y_min)/10;

    verticesTab.append(Vertex(x_min, y_min, 0)); // bas gauche
    verticesTab.append(Vertex(x_min, y_max, 0)); // haut gauche
    verticesTab.append(Vertex(x_max, y_min, 0)); // bas droit
    verticesTab.append(Vertex(x_max, y_max, 0)); // haut droit

    nb_vertex+=4;

    Vertex &bg = verticesTab[nb_vertex-4];
    Vertex &hg = verticesTab[nb_vertex-3];
    Vertex &bd = verticesTab[nb_vertex-2];
    Vertex &hd = verticesTab[nb_vertex-1];

    facesTab.append(Face(nb_vertex-4, nb_vertex-2, nb_vertex-3)); // Triangle bas gauche
    facesTab.append(Face(nb_vertex-1, nb_vertex-3, nb_vertex-2)); // Triangle haut droit

    nb_faces+=2;

    Face &triangle_bg = facesTab[nb_faces-2];
    Face &triangle_hd = facesTab[nb_faces-1];

    bg.i_incident_face = nb_faces-2;
    hg.i_incident_face = nb_faces-2;
    bd.i_incident_face = nb_faces-1;
    hd.i_incident_face = nb_faces-1;

    triangle_bg.adjacent_faces[0] = nb_faces-1;
    triangle_bg.adjacent_faces[1] = -1;
    triangle_bg.adjacent_faces[2] = -1;

    triangle_hd.adjacent_faces[0] = nb_faces-2;
    triangle_hd.adjacent_faces[1] = -1;
    triangle_hd.adjacent_faces[2] = -1;

    std::cout<<"------ok------"<<std::endl;

    for(int i_vertex=0;i_vertex<(nb_vertex-4);i_vertex++){
        for(int i_face=0;i_face<nb_faces;i_face++){
            //std::cout<<inTriangleTest(facesTab[i_face], verticesTab[i_vertex])<<std::endl;
            //std::cout<<"inTriangleTest : "<<inTriangleTest(facesTab[i_face], verticesTab[i_vertex])<<std::endl;
            if (inTriangleTest(facesTab[i_face], verticesTab[i_vertex])==0){
                std::cout<<"!!!!!!!!!!!!!    inTriangleTest = 0   !!!!!!!!!!!!!!!! "<<std::endl;
                std::cout<<"i_vertex :  "<<i_vertex<<std::endl;
                std::cout<<"i_face :  "<<i_face<<std::endl;
                insertionInArete(i_face, i_vertex);
                //lawsonAroundVertex(i_vertex);
            }else if (inTriangleTest(facesTab[i_face], verticesTab[i_vertex])>0){
                //std::cout<<"inTriangleTest : "<<inTriangleTest(facesTab[i_face], verticesTab[i_vertex])<<std::endl;
                std::cout<<"!!!!!!!!!!!!!    inTriangleTest = 0   !!!!!!!!!!!!!!!! "<<std::endl;
                std::cout<<"i_vertex :  "<<i_vertex<<std::endl;
                std::cout<<"i_face :  "<<i_face<<std::endl;
                insertionTriangle(i_vertex, i_face);
                //lawsonAroundVertex(i_vertex);
                //break;
            };
        };
    };
}

bool Mesh::areteEnBordure(int i_face, int i_vertex){
    // Renvoie True si, de l'autre coté de l'arete, il n'y a rien (indice de face negatif)
    if(facesTab[i_face].i_vertex[0] == i_vertex){
        return facesTab[i_face].adjacent_faces[0] <0;
    }else if(facesTab[i_face].i_vertex[1] == i_vertex){
        return facesTab[i_face].adjacent_faces[1] <0;
    }else{
        return facesTab[i_face].adjacent_faces[2] <0;
    };
}

std::pair<int, int> Mesh::areteSymetrique(std::pair<int, int> face_et_vertex){
    // En partant d'une arete décrite par i_face, i_vertex,
    // La fonction renvoi l'autre couple de i_face, i_vertex
    // Qui décrit la même arete mais opposée

    int i_face = face_et_vertex.first;
    int i_vertex = face_et_vertex.second;

    if(!areteEnBordure(i_face, i_vertex)){
        int i_face_sym, i_vertex_sym;
        for(int i=0;i<3;i++){
            if(facesTab[i_face].i_vertex[i] == i_vertex){
                i_face_sym = facesTab[i_face].adjacent_faces[i];
            };
        };
        for(int i=0;i<3;i++){
            if(facesTab[i_face_sym].adjacent_faces[i] == i_face){
                i_vertex_sym = i;
            };
        };
        return {i_face_sym, i_vertex_sym};
    }else{
        return {i_face, i_vertex};
    };
}

void Mesh::lawsonAroundVertex(int i_P){
    // On recupere les trois ou quatre aretes autour du P dans une file
    QList<std::pair<int, int>> atraiter;
    for(int i_face = 0;i_face<nb_faces;i_face++){
        for(int i = 0;i<3;i++){
            if(facesTab[i_face].i_vertex[i] == i_P){
                atraiter.append({i_face, i_P});
            }
        }
    };
    std::cout<<"a_traiter_size_ini :"<<atraiter.size()<<std::endl;
    // On lance la boucle while et on remplie et traite la file
    while(!atraiter.isEmpty()){
        std::cout<<"a_traiter : :"<<atraiter.size()<<std::endl;
        std::pair<int, int> face_et_vertex = atraiter.takeFirst();
        int i_face = face_et_vertex.first;
        int i_vertex = face_et_vertex.second;

        if(!areteEnBordure(i_face, i_vertex)){
            if(!isDelaunay(i_face, i_vertex)){
                QList<std::pair<int, int>> nouvelle_queue = flipEdge(i_face, i_vertex); // On fait le flip et récupère les arete à retester
                while(!nouvelle_queue.isEmpty()){

                    std::pair<int, int> face_et_vertex_quadrilatere = nouvelle_queue.takeFirst();
                    int i_face_quadrilatere = face_et_vertex.first;
                    int i_vertex_quadrilatere = face_et_vertex.second;

                    if(!areteEnBordure(i_face_quadrilatere, i_vertex_quadrilatere)){
                        if(!isDelaunay(i_face_quadrilatere, i_vertex_quadrilatere)){
                            atraiter.append(face_et_vertex_quadrilatere);
                        };
                    };
                };
            };
        };
    }
}


// ---------------Face------------------
Face::Face()
{
    i_vertex[0] = 0;
    i_vertex[1] = 0;
    i_vertex[2] = 0;
}

Face::Face(int i_vertex0_, int i_vertex1_, int i_vertex2_)
{
    i_vertex[0] = i_vertex0_;
    i_vertex[1] = i_vertex1_;
    i_vertex[2] = i_vertex2_;
}


// ---------------Useful function for circulator---------------
int find_i_vertex_in_face(Face *face, int i_vertex)
{
    int i_vertex_in_face = 0;
    while(face->i_vertex[i_vertex_in_face] != i_vertex)
    {
        i_vertex_in_face++;
    }
    return i_vertex_in_face;
}

// ------------Circulator on faces--------------------
Circulator_on_faces::Circulator_on_faces(){}

Circulator_on_faces::Circulator_on_faces(Mesh *p_mesh_, int i_vertex_)
{
    p_mesh = p_mesh_;
    i_vertex = i_vertex_; // Un indice de face
    i_face = p_mesh->verticesTab[i_vertex].i_incident_face; // Et l'indice de sa face adjacente
}

Circulator_on_faces Circulator_on_faces::begin() const
{
    return Circulator_on_faces(p_mesh, i_vertex);
}

Circulator_on_faces & Circulator_on_faces::operator++()
{
    Face &incident_face = p_mesh->facesTab[i_face]; // On récupère l'objet face

    // On trouve l'indice (0, 1 ou 2) de notre vecteur dans cette face

    int i_vertex_in_face = find_i_vertex_in_face(&incident_face, i_vertex);

    int next_i_vertex_in_face = (i_vertex_in_face+2)%3; // On prend l'indice du vertex suivant (2 car sens trigo)
    i_face = incident_face.adjacent_faces[next_i_vertex_in_face]; // On récupère l'indice de la face qui est en face
    return *this;
}

Face & Circulator_on_faces::operator*()
{
    // returning a reference on the current face.
    return p_mesh->facesTab[i_face];
}

bool Circulator_on_faces::operator!=(const Circulator_on_faces other_circulator_on_faces)
{
    return i_face != other_circulator_on_faces.i_face;
}

// -------------Circulator on vertices------------
Circulator_on_vertices::Circulator_on_vertices(Mesh *p_mesh_, int i_vertex_)
{
    p_mesh = p_mesh_;
    i_vertex = i_vertex_; // Un indice de face

    circulator_on_faces = Circulator_on_faces(p_mesh, i_vertex); // Initilisation du circulateur

    Face face = *circulator_on_faces;


    int i_vertex_in_face = find_i_vertex_in_face(&face, i_vertex);
    int i_vertex1 = face.i_vertex[(i_vertex_in_face+1)%3]; // On récupère les deux autres sommets
    int i_vertex2 = face.i_vertex[(i_vertex_in_face+2)%3];

    ++circulator_on_faces; // On fait circuler
    face = *circulator_on_faces;

    i_vertex_in_face = find_i_vertex_in_face(&face, i_vertex);
    int i_new_vertex1 = face.i_vertex[(i_vertex_in_face+1)%3]; // On récupère les deux autres sommets
    int i_new_vertex2 = face.i_vertex[(i_vertex_in_face+2)%3];

    // Ici il y forcement un idice de vertex en double. Celui en double sera le 1er
    if((i_new_vertex1 == i_vertex1) | (i_new_vertex1 == i_vertex2))
    {
        current_i_vertex = i_new_vertex1;
        next_i_vertex = i_new_vertex2;
    }
    else
    {
        current_i_vertex = i_new_vertex2;
        next_i_vertex = i_new_vertex1;
    }
}

Circulator_on_vertices & Circulator_on_vertices::operator++()
{// To end
    current_i_vertex = next_i_vertex;
    ++circulator_on_faces; // On passe à la prochaine face
    Face face = *circulator_on_faces; // On récupère l'objet face

    int i_vertex_in_face = find_i_vertex_in_face(&face, i_vertex); // On récupère l'id du vertex principal dans la face
    int current_i_vertex_in_face = find_i_vertex_in_face(&face, current_i_vertex); // On récupère l'id du vertex courrant dans la face
    int next_i_vertex_in_face = 3 - (i_vertex_in_face + current_i_vertex_in_face); // Donne le chiffre manquant parmis (0, 1, 2)

    next_i_vertex = face.i_vertex[next_i_vertex_in_face];
    return *this;
}

Circulator_on_vertices Circulator_on_vertices::begin() const
{
    return Circulator_on_vertices(p_mesh, i_vertex);
}

Vertex & Circulator_on_vertices::operator*()
{
    // returning a reference on the current vertex.
    return p_mesh->verticesTab[current_i_vertex];

}

bool Circulator_on_vertices::operator!=(const Circulator_on_vertices other_circulator_on_vertices)
{
    return current_i_vertex != other_circulator_on_vertices.current_i_vertex;
}


