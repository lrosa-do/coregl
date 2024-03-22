#include "Core.hpp"
#include "Camera.hpp"

 int screenWidth = 1024 ;
 int screenHeight = 768 ;

 Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
float mouseDeltaX = 0.0f;
float mouseDeltaY = 0.0f;
bool firstMouse = true;
bool IsMouseDown = false;

int main()
{

    Device * device = CreateDevice(screenWidth, screenHeight, "Hello World", true);
    if (!device)
    {
        return -1;
    }
 
    Driver * driver = device->GetDriver();

    // MeshBuilder::Instance().SetTexturePath("../assets/");
    // MeshBuilder::Instance().SetLoadPath("../assets/mesh/");


    //   VertexFormat::Element elements[] =
    // {
    //     VertexFormat::Element(VertexFormat::POSITION, 3),
    //     VertexFormat::Element(VertexFormat::TEXCOORD0, 2),
    //     VertexFormat::Element(VertexFormat::COLOR, 4)
        
    // };  


    // Mesh *mesh=MeshBuilder::Instance().LoadMesh(VertexFormat(elements, 3),"room.obj");
   

   

     driver->SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    RenderBatch * batch=  driver->CreateRenderBatch();
    batch->Init(1,1024);

    Font * font = Assets::Instance().GetDefaultFont();

    Shader * shader = Assets::Instance().GetShader("default");
   // Shader * shader3D = Assets::Instance().GetShader("textured");

  

    Assets::Instance().LoadTexture("wall", "../assets/wall.jpg");


      VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3),
        VertexFormat::Element(VertexFormat::TEXCOORD0, 2),
        VertexFormat::Element(VertexFormat::COLOR, 4)
        
    };  

      Mesh *cube = MeshBuilder::Instance().CreateCube(VertexFormat(elements, 3), 1,1,1);
      cube->AddMaterial(new TextureMaterial(Assets::Instance().GetTexture("wall")));


   MeshBuilder::Instance().SetTexturePath("../assets/");
      MeshBuilder::Instance().SetLoadPath("../assets/mesh/");
   



      Mesh  * room =MeshBuilder::Instance().LoadMesh(VertexFormat(elements, 3),"room.obj");

  
  
  


    font->SetBatch(batch);

    while (device->Run())
    {

        float deltaTime =  device->GetFrameTime();


        driver->Clear();


    driver->SetCullFace(true);
    driver->SetDepthTest(true);

    


         if (Keyboard::Down(KEY_W))
         {
             camera.ProcessKeyboard(FORWARD, deltaTime);
         } 
         if (Keyboard::Down(KEY_S))
         {
             camera.ProcessKeyboard(BACKWARD, deltaTime);
         }
         if (Keyboard::Down(KEY_A))
         {
			 camera.ProcessKeyboard(LEFT, deltaTime);
		 }
    
         if (Keyboard::Down(KEY_D))
         {
             camera.ProcessKeyboard(RIGHT, deltaTime);
         }


                 int xposIn, yposIn;
                u32 IsMouseDown = SDL_GetMouseState(&xposIn, &yposIn);


                  if ( IsMouseDown & SDL_BUTTON(SDL_BUTTON_LEFT) )
                  {

                         
                       
                          float xpos = static_cast<float>(xposIn);
                          float ypos = static_cast<float>(yposIn);

                          if (firstMouse)
                          {
                              lastX = xpos;
                              lastY = ypos;
                              firstMouse = false;
                          }

                          float xoffset = xpos - lastX;
                          float yoffset = lastY - ypos; 

                          lastX = xpos;
                          lastY = ypos;

                       camera.ProcessMouseMovement(xoffset, yoffset);


                  }
                  else
                  {
                      firstMouse = true;
                  }

    driver->SetShader(shader);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 2000.0f);



    glm::mat4 view = camera.GetViewMatrix();


    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 mvp3D = projection * view * model;

    shader->SetMatrix4("mvp", mvp3D);


     cube->Render(shader); //3d
     room->Render(shader); //3d


    mvp3D = projection * view * glm::mat4(1.0f);
    shader->SetMatrix4("mvp", mvp3D);


     batch->Grid(10,10,true);
     batch->Render();
   



   //2d

    driver->SetCullFace(false);
    driver->SetDepthTest(false);
    driver->SetBlend(true);
    driver->SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 mvp = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -10.0f, 10.0f);
    
    driver->SetShader(shader);
    shader->SetMatrix4("mvp", mvp);
    shader->SetInt("texture1", 0);





     batch->SetColor(255,255,255,255);
     batch->Line2D(100,100,200,200);

     font->Print(100,100,"Hello ");

     batch->Render();


        device->Swap();
    }

     room->Drop();
     cube->Drop();

    batch->Drop();
 
    
device->Drop();
    

    return 0;
}