#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <chrono>

#include <fstream>
#include <sstream>

// ---------------- STRUCTS -----------------------
struct Mesh {
    GLuint VAO, VBO, EBO;
    int indexCount;
    int materialID;
};

// ---------------- UTILS -----------------------
GLuint CompileShader(GLenum type, const char* src){
    GLuint s = glCreateShader(type);
    glShaderSource(s,1,&src,NULL);
    glCompileShader(s);

    int ok;
    glGetShaderiv(s,GL_COMPILE_STATUS,&ok);
    if(!ok){
        char info[512];
        glGetShaderInfoLog(s,512,NULL,info);
        std::cerr << "Shader Error: " << info << std::endl;
    }
    return s;
}

std::string LoadTextFile(const char* path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// MAIN 
int main(){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800,600,"Cornell Box",nullptr,nullptr);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    // SHADER PROGRAM
    std::string vertCode = LoadTextFile("shaders/basic.vert");
    std::string fragCode = LoadTextFile("shaders/basic.frag");

    const char* vertexShaderSrc = vertCode.c_str();
    const char* fragmentShaderSrc = fragCode.c_str();

    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

    GLuint shader = glCreateProgram();
    glAttachShader(shader, vs);
    glAttachShader(shader, fs);
    glLinkProgram(shader);

    glDeleteShader(vs);
    glDeleteShader(fs);


    // LOAD OBJ
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn,err;

    tinyobj::LoadObj(
        &attrib,&shapes,&materials,&warn,&err,
        "scene/CornellBox-Sphere.obj","scene/"
    );

    std::vector<Mesh> meshes;

    for(auto& shape : shapes){
        std::vector<float> verts;
        std::vector<unsigned int> inds;

        for(size_t i=0;i<shape.mesh.indices.size();i++){
            auto idx = shape.mesh.indices[i];

            verts.push_back(attrib.vertices[3*idx.vertex_index+0]);
            verts.push_back(attrib.vertices[3*idx.vertex_index+1]);
            verts.push_back(attrib.vertices[3*idx.vertex_index+2]);

            verts.push_back(attrib.normals[3*idx.normal_index+0]);
            verts.push_back(attrib.normals[3*idx.normal_index+1]);
            verts.push_back(attrib.normals[3*idx.normal_index+2]);

            inds.push_back(i);
        }

        Mesh m;
        m.materialID = shape.mesh.material_ids[0];
        m.indexCount = inds.size();

        glGenVertexArrays(1,&m.VAO);
        glGenBuffers(1,&m.VBO);
        glGenBuffers(1,&m.EBO);

        glBindVertexArray(m.VAO);

        glBindBuffer(GL_ARRAY_BUFFER,m.VBO);
        glBufferData(GL_ARRAY_BUFFER,verts.size()*sizeof(float),verts.data(),GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,inds.size()*sizeof(unsigned int),inds.data(),GL_STATIC_DRAW);

        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        meshes.push_back(m);
    }

    // ---- iluminación ----
    int lightingModel = 0;    //empieza en Phon
    float angle = 0.0f;
    const int numLights = 2;
    glm::vec3 lightPos[numLights];
    glm::vec3 lightColor[numLights];

    // ---- FPS ----
    auto lastTime = std::chrono::high_resolution_clock::now();
    int frames = 0;

    // ---- Controlar duración ----
    //float duration = 10.0f;
    //auto startTime = std::chrono::high_resolution_clock::now();

    // ---- Buffer para almacenar imagen ----
    std::vector<unsigned char> pixels(800*600*3);

    // MAIN LOOP
    while(!glfwWindowShouldClose(window)){

        // FPS
        frames++;
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now-lastTime).count();
        if(dt >= 1.0f){
            std::cout << "FPS: " << frames << std::endl;
            frames = 0;
            lastTime = now;
        }

        // Clear
        glClearColor(0.1,0.1,0.1,1.0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);

        // Camera
        glm::mat4 model(1.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0,0.8,2.2), glm::vec3(0,0,0), glm::vec3(0,1,0));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.f/600.f, 0.1f, 100.f);

        glUniformMatrix4fv(glGetUniformLocation(shader,"model"),1,false,glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader,"view"),1,false,glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader,"projection"),1,false,glm::value_ptr(proj));

        glUniform3f(glGetUniformLocation(shader,"viewPos"),0,1,5);
        glUniform1i(glGetUniformLocation(shader,"lightingModel"),lightingModel);
        glUniform1i(glGetUniformLocation(shader,"numLights"),numLights);

        // Lights
        angle += 0.005f;
        glm::vec3 center(0,0.5,0);

        for(int i=0;i<numLights;i++){
            float radius = 3.0f + i*0.2f;
            float height = 0.8f + i*0.1f;
            float speed = 0.5f + i*0.2f;
            float phase = i*1.2f;

            lightPos[i] = center + glm::vec3(
                radius * sin(angle*speed+phase),
                height,
                radius * cos(angle*speed+phase)
            );
            lightColor[i] = glm::vec3(1.0f);

            glUniform3fv(
                glGetUniformLocation(shader,("lightPos["+std::to_string(i)+"]").c_str()),
                1, glm::value_ptr(lightPos[i])
            );
            glUniform3fv(
                glGetUniformLocation(shader,("lightColor["+std::to_string(i)+"]").c_str()),
                1, glm::value_ptr(lightColor[i])
            );
        }

        // Draw meshes
        for(auto& m : meshes){
            tinyobj::material_t mat = materials[m.materialID];

            glUniform3fv(glGetUniformLocation(shader,"Ka"),1,mat.ambient);
            glUniform3fv(glGetUniformLocation(shader,"Kd"),1,mat.diffuse);
            glUniform3fv(glGetUniformLocation(shader,"Ks"),1,mat.specular);
            glUniform3fv(glGetUniformLocation(shader,"Ke"),1,mat.emission);
            glUniform1f(glGetUniformLocation(shader,"Ns"),mat.shininess);

            glBindVertexArray(m.VAO);
            glDrawElements(GL_TRIANGLES, m.indexCount, GL_UNSIGNED_INT, 0);
        }

        // Cambiar modelo de iluminación
        if(glfwGetKey(window,GLFW_KEY_1)==GLFW_PRESS) lightingModel=0;
        if(glfwGetKey(window,GLFW_KEY_2)==GLFW_PRESS) lightingModel=1;
        if(glfwGetKey(window,GLFW_KEY_3)==GLFW_PRESS) lightingModel=2;

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Terminar después de 10s
        //float elapsed = std::chrono::duration<float>(now - startTime).count();
        //if(elapsed >= duration) break;
    }

    // CAPTURAR IMAGEN FINAL 
    glReadPixels(0,0,800,600,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    stbi_flip_vertically_on_write(1);
    stbi_write_png("final_render.png",800,600,3,pixels.data(),800*3);

    glfwTerminate();
}


