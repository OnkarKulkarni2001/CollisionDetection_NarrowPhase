#include "GLCommon.h"
#include "sharedThings.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include "sMesh.h"
#include "cVAOManager/cVAOManager.h"

extern cVAOManager* g_pMeshManager;

//RenderCall

sMesh* pDebugSphere = NULL;


void DrawMesh(sMesh* pCurMesh, glm::mat4 matModel, GLuint program)
{
    // Is it visible? 
    if (!pCurMesh->bIsVisible)
    {
        // Continue in loops jumps to the end of this loop
        // (for, while, do)
        return;
    }

    glCullFace(GL_BACK);
    //glEnable(GL_CULL_FACE);

    // Use lighting or not
    // uniform bool bDoNotLight;	
    GLint bDoNotLight_UL = glGetUniformLocation(program, "bDoNotLight");
    if (pCurMesh->bDoNotLight)
    {
        //glUniform1f(bDoNotLight_UL, 1.0f);  // True
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);  // True
    }
    else
    {
        //                glUniform1f(bDoNotLight_UL, 0.0f);  // False
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);  // False
    }


    //// Could be called the "model" or "world" matrix
    //glm::mat4 matModel = glm::mat4(1.0f);

    //// Translation (movement, position, placement...)
    //glm::mat4 matTranslate
    //    = glm::translate(glm::mat4(1.0f),
    //        glm::vec3(pCurMesh->positionXYZ.x,
    //            pCurMesh->positionXYZ.y,
    //            pCurMesh->positionXYZ.z));

    //// Rotation...
    //// Caculate 3 Euler acix matrices...
    //glm::mat4 matRotateX =
    //    glm::rotate(glm::mat4(1.0f),
    //        glm::radians(pCurMesh->rotationEulerXYZ.x), // Angle in radians
    //        glm::vec3(1.0f, 0.0, 0.0f));

    //glm::mat4 matRotateY =
    //    glm::rotate(glm::mat4(1.0f),
    //        glm::radians(pCurMesh->rotationEulerXYZ.y), // Angle in radians
    //        glm::vec3(0.0f, 1.0, 0.0f));

    //glm::mat4 matRotateZ =
    //    glm::rotate(glm::mat4(1.0f),
    //        glm::radians(pCurMesh->rotationEulerXYZ.z), // Angle in radians
    //        glm::vec3(0.0f, 0.0, 1.0f));


    //// Scale
    //glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
    //    glm::vec3(pCurMesh->uniformScale,
    //        pCurMesh->uniformScale,
    //        pCurMesh->uniformScale));


    //// Calculate the final model/world matrix
    //matModel *= matTranslate;     // matModel = matModel * matTranslate;
    //matModel *= matRotateX;
    //matModel *= matRotateY;
    //matModel *= matRotateZ;
    //matModel *= matScale;

    //           matRoationOnly = matModel * matRotateX * matRotateY * matRotateZ;


               //mat4x4_mul(mvp, p, m);
               //mvp = p * v * m;
   //            glm::mat4 matMVP = matProjection * matView * matModel;

               //const GLint mvp_location = glGetUniformLocation(program, "MVP");
               //glUniformMatrix4fv(mvp_location, 
               //    1,
               //    GL_FALSE,
               //    (const GLfloat*)&matMVP);

    const GLint mvp_location = glGetUniformLocation(program, "matModel");
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&matModel);

    //const GLint mvp_location = glGetUniformLocation(program, "matView");
    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&matMVP);

    //const GLint mvp_location = glGetUniformLocation(program, "matProjection");
    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&matMVP);

    // uniform bool bUseObjectColour;
    GLint bUseObjectColour = glGetUniformLocation(program, "bUseObjectColour");

    if (pCurMesh->bOverrideObjectColour)
    {
        // bool doesn't really exist, it's a float...
        glUniform1f(bUseObjectColour, (GLfloat)GL_TRUE);    // or 1.0f
    }
    else
    {
        glUniform1f(bUseObjectColour, (GLfloat)GL_FALSE);   // or 0.0f
    }

    // Set the object colour
    // uniform vec4 objectColour;
    GLint objectColour_UL = glGetUniformLocation(program, "objectColour");
    glUniform4f(objectColour_UL,
        pCurMesh->objectColourRGBA.r,
        pCurMesh->objectColourRGBA.g,
        pCurMesh->objectColourRGBA.b,
        1.0f);


    // solid or wireframe, etc.
//        glPointSize(10.0f);
    if (pCurMesh->bIsWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    //        glDrawArrays(GL_TRIANGLES, 0, 3);
//            glDrawArrays(GL_TRIANGLES, 0, numberOfVertices_TO_DRAW);

    sModelDrawInfo meshToDrawInfo;
    if (::g_pMeshManager->FindDrawInfoByModelName(pCurMesh->modelFileName, meshToDrawInfo))
    {
        // Found the model
        glBindVertexArray(meshToDrawInfo.VAO_ID); 		// enable VAO(and everything else)
        //https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.xhtml
        glDrawElements(GL_TRIANGLES,
            meshToDrawInfo.numberOfIndices,
            GL_UNSIGNED_INT,
            (void*)0);

        glBindVertexArray(0); 			//disable VAO(and everything else)
    }


    //    // Check if there are child meshes
    //    for (unsigned int childIndex = 0; childIndex != pCurMesh->vec_pChildMeshes.size(); childIndex++)
    //    {
    //        DrawMesh(pCurMesh->vec_pChildMeshes[childIndex], program);
    //    }

    return;
}

void DrawMesh(sMesh* pCurMesh, GLuint program)
{
    // Is it visible? 
    if (!pCurMesh->bIsVisible)
    {
        // Continue in loops jumps to the end of this loop
        // (for, while, do)
        return;
    }

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // Use lighting or not
    // uniform bool bDoNotLight;	
    GLint bDoNotLight_UL = glGetUniformLocation(program, "bDoNotLight");
    if (pCurMesh->bDoNotLight)
    {
        //glUniform1f(bDoNotLight_UL, 1.0f);  // True
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);  // True
    }
    else
    {
        //                glUniform1f(bDoNotLight_UL, 0.0f);  // False
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);  // False
    }


    // Could be called the "model" or "world" matrix
    glm::mat4 matModel = glm::mat4(1.0f);

    // Translation (movement, position, placement...)
    glm::mat4 matTranslate
        = glm::translate(glm::mat4(1.0f),
            glm::vec3(pCurMesh->positionXYZ.x,
                pCurMesh->positionXYZ.y,
                pCurMesh->positionXYZ.z));

    // Rotation...
    // Caculate 3 Euler acix matrices...
    glm::mat4 matRotateX =
        glm::rotate(glm::mat4(1.0f),
            glm::radians(pCurMesh->rotationEulerXYZ.x), // Angle in radians
            glm::vec3(1.0f, 0.0, 0.0f));

    glm::mat4 matRotateY =
        glm::rotate(glm::mat4(1.0f),
            glm::radians(pCurMesh->rotationEulerXYZ.y), // Angle in radians
            glm::vec3(0.0f, 1.0, 0.0f));

    glm::mat4 matRotateZ =
        glm::rotate(glm::mat4(1.0f),
            glm::radians(pCurMesh->rotationEulerXYZ.z), // Angle in radians
            glm::vec3(0.0f, 0.0, 1.0f));


    // Scale
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
        glm::vec3(pCurMesh->uniformScale,
            pCurMesh->uniformScale,
            pCurMesh->uniformScale));


    // Calculate the final model/world matrix
    matModel *= matTranslate;     // matModel = matModel * matTranslate;
    matModel *= matRotateX;
    matModel *= matRotateY;
    matModel *= matRotateZ;
    matModel *= matScale;

    //           matRoationOnly = matModel * matRotateX * matRotateY * matRotateZ;


               //mat4x4_mul(mvp, p, m);
               //mvp = p * v * m;
   //            glm::mat4 matMVP = matProjection * matView * matModel;

               //const GLint mvp_location = glGetUniformLocation(program, "MVP");
               //glUniformMatrix4fv(mvp_location, 
               //    1,
               //    GL_FALSE,
               //    (const GLfloat*)&matMVP);

    const GLint mvp_location = glGetUniformLocation(program, "matModel");
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&matModel);

    //const GLint mvp_location = glGetUniformLocation(program, "matView");
    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&matMVP);

    //const GLint mvp_location = glGetUniformLocation(program, "matProjection");
    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&matMVP);

    // uniform bool bUseObjectColour;
    GLint bUseObjectColour = glGetUniformLocation(program, "bUseObjectColour");

    if (pCurMesh->bOverrideObjectColour)
    {
        // bool doesn't really exist, it's a float...
        glUniform1f(bUseObjectColour, (GLfloat)GL_TRUE);    // or 1.0f
    }
    else
    {
        glUniform1f(bUseObjectColour, (GLfloat)GL_FALSE);   // or 0.0f
    }

    // Set the object colour
    // uniform vec4 objectColour;
    GLint objectColour_UL = glGetUniformLocation(program, "objectColour");
    glUniform4f(objectColour_UL,
        pCurMesh->objectColourRGBA.r,
        pCurMesh->objectColourRGBA.g,
        pCurMesh->objectColourRGBA.b,
        1.0f);


    // solid or wireframe, etc.
//        glPointSize(10.0f);
    if (pCurMesh->bIsWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    //        glDrawArrays(GL_TRIANGLES, 0, 3);
//            glDrawArrays(GL_TRIANGLES, 0, numberOfVertices_TO_DRAW);

    sModelDrawInfo meshToDrawInfo;
    if (::g_pMeshManager->FindDrawInfoByModelName(pCurMesh->modelFileName, meshToDrawInfo))
    {
        // Found the model
        glBindVertexArray(meshToDrawInfo.VAO_ID); 		// enable VAO(and everything else)
        //https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.xhtml
        glDrawElements(GL_TRIANGLES,
            meshToDrawInfo.numberOfIndices,
            GL_UNSIGNED_INT,
            (void*)0);

        glBindVertexArray(0); 			//disable VAO(and everything else)
    }


//    // Check if there are child meshes
//    for (unsigned int childIndex = 0; childIndex != pCurMesh->vec_pChildMeshes.size(); childIndex++)
//    {
//        DrawMesh(pCurMesh->vec_pChildMeshes[childIndex], program);
//    }

	return;
}


void DrawDebugSphere(glm::vec3 position, glm::vec4 RGBA, float scale, GLuint program)
{
    // Created the debug sphere, yet?
    if (!pDebugSphere)           // Same as if ( pDebugSphere == NULL )
    {
        pDebugSphere = new sMesh();
//        pDebugSphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N.ply";
        pDebugSphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N_uv.ply";
        pDebugSphere->positionXYZ = glm::vec3(0.0f, 5.0f, 0.0f);
        pDebugSphere->bIsWireframe = true;
        pDebugSphere->objectColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        pDebugSphere->uniqueFriendlyName = "Debug_Sphere";
        pDebugSphere->uniformScale = 10.0f;
        pDebugSphere->bDoNotLight = true;
    }
    // At this point the debug sphere is created

    pDebugSphere->bIsVisible = true;
    pDebugSphere->positionXYZ = position;
    pDebugSphere->bOverrideObjectColour = true;
    pDebugSphere->objectColourRGBA = RGBA;
    pDebugSphere->uniformScale = scale;

    DrawMesh(pDebugSphere, program);

    pDebugSphere->bIsVisible = false;

    return;
}