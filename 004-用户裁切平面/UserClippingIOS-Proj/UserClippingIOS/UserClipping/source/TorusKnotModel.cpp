//--------------------------------------------------------------------------------
// TorusKnotModel.cpp
// Render a plane
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "TorusKnotModel.h"

#include "matrix4.h"
#include "Geometry.h"

#include "TorusKnotGeometry.h"

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

extern float UserClipDistance;

float TorusKnotModel::timer=0.f;

TorusKnotModel::TorusKnotModel() {
    vao_ = 0;
    vbo_ = 0;
    ibo_ = 0;
    
    // Load shader
    phongShaderState_.reset(new PhongShaderState());
    
    TorusKnotGeometry tkg = TorusKnotGeometry();
    TorusKnotGeometry::TKGVertexData tkgVdata = makeTKGVertexData(tkg);
    
    // Create VBO
    num_vertices_ = (int)tkgVdata.vData.size();
    num_indices_ = (int)tkgVdata.iData.size();
    
    vector<VertexPNX> vertices = tkgVdata.vData;
    vector<unsigned short> indices = tkgVdata.iData;
    geometry_.reset(new Geometry(&vertices[0],&indices[0],num_vertices_,num_indices_));
    
    
    if(!vao_)
        glGenVertexArrays(1, &vao_);
    
    glBindVertexArray(vao_);
    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, geometry_->vbo);
    
    int32_t iStride = sizeof(VertexPNX);
    // Pass the vertex data
    glVertexAttribPointer(TEXTURE_ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(0));
    glEnableVertexAttribArray(TEXTURE_ATTRIB_VERTEX);
    
    glVertexAttribPointer(TEXTURE_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(TEXTURE_ATTRIB_NORMAL);
    
    glVertexAttribPointer(TEXTURE_ATTRIB_UV, 2, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(2* 3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(TEXTURE_ATTRIB_UV);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry_->ibo);
    
    glBindVertexArray(0);
    
    std::vector<std::string> textures = {std::string(GetBundleFileName("cloud.png"))};
    texObj_ = Texture::Create(GL_TEXTURE_2D, textures);
    assert(texObj_);
    
    //    texObj_->Activate(GL_TEXTURE2);
    //    glUniform1i(phongShaderState_->tex1_, 2);
    
    std::vector<std::string> textures1 = {std::string(GetBundleFileName("lavatile.jpg"))};
    texObj1_ = Texture::Create(GL_TEXTURE_2D, textures1);
    assert(texObj1_);
}

TorusKnotModel::~TorusKnotModel(){
    Unload();
}

void TorusKnotModel::Init() {
    
    
    mat_model_ = Matrix4::makeTranslation(Cvec3(0.f, 0.f, 0.f));
    
    //mat_model_ =  mat_model_ * Matrix4::makeXRotation(45);
    
    mat_view_ = Matrix4::makeTranslation(Cvec3(0,0,4.0f));
    mat_view_ = inv(mat_view_);

}

void TorusKnotModel::Unload() {
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    
    if (ibo_) {
        glDeleteBuffers(1, &ibo_);
        ibo_ = 0;
    }
    
}

void TorusKnotModel::Update(double time) {
    mat_projection_ = perspectiveCamera_->projMat;
    
}

void TorusKnotModel::setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera){
    this->perspectiveCamera_ = camera;
    
    Update(0);
}

void TorusKnotModel::Render(){
    glDisable(GL_CULL_FACE);
    
    glUseProgram(phongShaderState_->program);

    
    TORUS_MATERIALS material = {
        {1.0, 0.055, 0.027}, {1.0f, 1.0f, 1.0f, 90.f}, {0.1f, 0.1f, 0.1f}, };
    
    // Update uniforms
    glUniform4f(phongShaderState_->material_diffuse_, material.diffuse_color[0],
                material.diffuse_color[1], material.diffuse_color[2], 1.f);
    
    glUniform4f(phongShaderState_->material_specular_, material.specular_color[0],
                material.specular_color[1], material.specular_color[2],
                material.specular_color[3]);
    
    glUniform3f(phongShaderState_->material_ambient_, material.ambient_color[0],
                material.ambient_color[1], material.ambient_color[2]);
    
    glUniform3f(phongShaderState_->light0_, -2.f, 5.f, 50.f);
    
    glUniform4f(phongShaderState_->userClipPlane_, 0, -1, 0, UserClipDistance);
    
    mat_model_ = mat_model_ * Matrix4::makeYRotation(0.0625);
    mat_model_ = mat_model_ * Matrix4::makeXRotation(0.25);
    // Feed Projection and Model View matrices to the shaders
    //Matrix4 mat_vp = mat_projection_ * mat_view_ * mat_model_;
    Matrix4 mat_mv = mat_view_ * mat_model_;
    
    
    GLfloat glmatrix[16];
    mat_mv.writeToColumnMajorMatrix(glmatrix);
    glUniformMatrix4fv(phongShaderState_->matrix_mv_, 1, GL_FALSE,
                       glmatrix);
    
    GLfloat glmatrix2[16];
    mat_projection_.writeToColumnMajorMatrix(glmatrix2);
    glUniformMatrix4fv(phongShaderState_->matrix_p_, 1, GL_FALSE,
                       glmatrix2);
    
    
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, num_indices_,GL_UNSIGNED_SHORT,NULL);
    
    glBindVertexArray(0);
}

