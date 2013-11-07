//
//  pong4_app.h
//  layer1
//
//  Created by Mark Woulfe on 08/10/2013.
//  Copyright (c) 2013 Andy Thomason. All rights reserved.
//

namespace octet {
  
  class box_4 {
  protected:
    // where is our box (overkill for a ping game!)
    mat4t modelToWorld;
    
    // half the width of the box
    float halfWidth;
    
    // half the height of the box
    float halfHeight;
    
    // what color is our box
    vec4 color;
    
  public:
    box_4() {
    }
    
    virtual void init(const vec4 &_color, float x, float y, float w, float h) {
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
      halfWidth = w * 0.5f;
      halfHeight = h * 0.5f;
      color = _color;
    }
    
    virtual void render(color_shader &shader, mat4t &cameraToWorld) {
      // build a projection matrix: model -> world -> camera -> projection
      // the projection space is the cube -1 <= x/w, y/w, z/w <= 1
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
      
      // set up the uniforms for the shader
      shader.render(modelToProjection, color);
      
      // this is an array of the positions of the corners of the box in 3D
      // a straight "float" here means this array is being generated here at runtime.
      float vertices[] = {
        -halfWidth, -halfHeight, 0,
        halfWidth, -halfHeight, 0,
        halfWidth,  halfHeight, 0,
        -halfWidth,  halfHeight, 0,
      };
      
      // attribute_pos (=0) is position of each corner
      // each corner has 3 floats (x, y, z)
      // there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
      glEnableVertexAttribArray(attribute_pos);
      
      
      // finally, draw the box (4 vertices)
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    
    // move the object
    void translate(float x, float y) {
      modelToWorld.translate(x, y, 0);
    }
    
    ////////////////////ADDED CODE////////////////////
    
    //super basic scaling (increase,decrease width and height)
    void scale(float w, float h){
      halfWidth += w;
      halfHeight += h;
    }
    
    //funtions used to get the width and height of a box, used for checking wether to render a bat or not
    float getWidth(){
      return halfWidth;
    }
    
    float getHeight(){
      return halfHeight;
    }
    ////////////////////ADDED CODE////////////////////
    
    // position the object relative to another.
    void set_relative(box_4 &rhs, float x, float y) {
      modelToWorld = rhs.modelToWorld;
      modelToWorld.translate(x, y, 0);
    }
    
    // return true if this box collides with another.
    // note the "const"s which say we do not modify either box
    bool collides_with(const box_4 &rhs) const {
      float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];
      float dy = rhs.modelToWorld[3][1] - modelToWorld[3][1];
      
      // both distances have to be under the sum of the halfwidths
      // for a collision
      return
      (fabsf(dx) < halfWidth + rhs.halfWidth) &&
      (fabsf(dy) < halfHeight + rhs.halfHeight)
      ;
    }
  };
  
  ////////////////////ADDED CODE////////////////////
  
  //powerUp class derived from box
  class powerUp : public box_4{
  protected:
    //a boolean used to show if the powerup should be displayed or not
    bool powerUpVisible = false;
    
    // what texture is on our powerUp
    int texture;
    
  public:
    powerUp(){
      texture = 0;
    }
    
    //overriden init and render to use my texture shader instead of the basic color one
    void init(int _texture, float x, float y, float w, float h) {
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
      halfWidth = w * 0.5f;
      halfHeight = h * 0.5f;
      texture = _texture;
    }
    
    void render(texture_shader &shader, mat4t &cameraToWorld) {
    
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
      
      // set up opengl to draw textured triangles using sampler 0 (GL_TEXTURE0)
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);
      
      // use "old skool" rendering
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      shader.render(modelToProjection, 0);
    
      // this is an array of the positions of the corners of the sprite in 3D
      // a straight "float" here means this array is being generated here at runtime.
      float vertices[] = {
        -halfWidth, -halfHeight, 0,
        halfWidth, -halfHeight, 0,
        halfWidth,  halfHeight, 0,
        -halfWidth,  halfHeight, 0,
      };
      
      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
      glEnableVertexAttribArray(attribute_pos);
      
      // this is an array of the positions of the corners of the texture in 2D
      static const float uvs[] = {
        0,  0,
        1,  0,
        1,  1,
        0,  1,
      };
      
      // attribute_uv is position in the texture of each corner
      // each corner (vertex) has 2 floats (x, y)
      // there is no gap between the 2 floats and hence the stride is 2*sizeof(float)
      glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)uvs );
      glEnableVertexAttribArray(attribute_uv);
      
      // finally, draw the sprite (4 vertices)
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      
    }
    
    //a function to access the boolean value powerUpVisible
    bool isPowerUpVisible(){
      return powerUpVisible;
    }
    
    //a function to set the boolean
    bool setPowerUpVisible(bool yes){
      if(yes)powerUpVisible = true;
      else powerUpVisible = false;
      return powerUpVisible;
    }
    
    //a virtual float called effect which will be used in the child classes
    virtual float effect(float effect){
      return effect;
    }
    
  };
  
  //a power up to increase the velocity of the ball
  class speedPowerUp : public powerUp{
  public:
    speedPowerUp(){
    }
    
    float effect(float velocity){
      
      float increasedVelocity = velocity*1.75;
      
      return increasedVelocity;
    }
    
  };
  
  //a power up to decrease the velocity of the ball
  class speedDownPowerUp : public powerUp{
  public:
    speedDownPowerUp(){
    }
    
    float effect(float velocity){
      
      float decreasedVelocity = velocity*0.75;
      
      return decreasedVelocity;
    }
    
  };
  
  ////////////////////ADDED CODE////////////////////
  
  class pong4_app : public octet::app {
    
    // Matrix to transform points in our camera space to the world.
    // This lets us move our camera
    mat4t cameraToWorld;
    
    // shader to draw a solid color
    color_shader color_shader_;
    
    //shader to draw our texture
    texture_shader texture_shader_;
    
    // what state is the game in?
    enum state_t {
      state_serving_left,
      state_serving_right,
      state_playing,
      state_game_over
    };
    state_t state;
    
    // counters for scores
    int scores[2];
    
    // game objects
    
    ////////////////////ADDED CODE////////////////////
    
    //creating two pointers to powerups speedUp and Down respectively and adding them into an array
    static const int powerUpArraySize = 2;
    
    speedPowerUp * speedUp = new speedPowerUp;
    speedDownPowerUp * speedDown = new speedDownPowerUp;
    
    powerUp * powerUps[powerUpArraySize] = {speedUp, speedDown};
    
    ////////////////////ADDED CODE////////////////////
    
    // court components
    box_4 court[4];
    
    // bats (NOW WITH 4 BATS INSTEAD OF 2)
    box_4 bat[4];
    
    // the ball
    box_4 ball;
    
    // velocity of the ball
    float ball_velocity_x;
    float ball_velocity_y;
    
    ////////////////////ADDED CODE////////////////////
    //code for keeping track of how much the bats have shrunk
    float topScaleValue = 0.f;
    float bottomScaleValue = 0.f;
    
    //boolean for AI bat movement
    bool move_right = true;
    bool move_left = true;
    bool left_or_right;
    
    ////////////////////ADDED CODE////////////////////
    
    // move the objects before drawing and do collision detection on the court
    void simulate() {
      
      // W and S move the left bat
      if (is_key_down('W') && !bat[0].collides_with(court[1]) && !bat[0].collides_with(bat[2])) {
        bat[0].translate(0, +0.2f);
      } else if (is_key_down('S') && !bat[0].collides_with(court[0]) && !bat[0].collides_with(bat[3])) {
        bat[0].translate(0, -0.2f);
      }
      
      // up and down arrow move the right bat
      if (is_key_down(key_up) && !bat[1].collides_with(court[1]) && !bat[1].collides_with(bat[2])) {
        bat[1].translate(0, +0.2f);
      } else if (is_key_down(key_down) && !bat[1].collides_with(court[0]) && !bat[1].collides_with(bat[3])) {
        bat[1].translate(0, -0.2f);
      }
      
      ///////////////////////ADDED CODE///////////////////////
      
      //wall collision detection and bat collision detection to change bats direction
                               
      if(bat[2].collides_with(court[3]) || bat[2].collides_with(bat[1])){
        move_right = false;
      }
      else if (bat[2].collides_with(court[2]) || bat[2].collides_with(bat[0])){
        move_right = true;
      }
      
      if (bat[3].collides_with(court[2]) || bat[3].collides_with(bat[0])){
        move_left = false;
      }
      else if(bat[3].collides_with(court[3]) || bat[3].collides_with(bat[1])){
        move_left = true;
      }
        
      // top bat moves back and forth
      if (!bat[2].collides_with(court[3]) && move_right) {
        bat[2].translate(+0.1f,0);
      } else if (!bat[2].collides_with(court[2])) {
        bat[2].translate(-0.1f,0);
      }
      
      // bottom bat moves back and forth the other direction
      if (!bat[3].collides_with(court[2]) && move_left) {
        bat[3].translate(-0.1f,0);
      } else if (!bat[3].collides_with(court[3])) {
        bat[3].translate(+0.1f,0);
      }
      
      if (state == state_serving_left) {
        // if we are serving, glue the ball to the left bat
        ball.set_relative(bat[0], 0.3f, 0);
        if (is_key_down(key_space)) {
          // space serves, changing the state
          state = state_playing;
          ball_velocity_x = 0.10f;
          ball_velocity_y = 0.10f;
        }
      } else if (state == state_serving_right) {
        // if we are serving, glue the ball to the right bat
        ball.set_relative(bat[1], -0.3f, 0);
        if (is_key_down(key_space)) {
          // space serves, changing the state
          state = state_playing;
          ball_velocity_x = -0.10f;
          ball_velocity_y = 0.10f;
        }
        
        ////////////////////ADDED CODE////////////////////
        
      } else if (state == state_playing) {
        
        // if we are playing, move the ball
        ball.translate(ball_velocity_x, ball_velocity_y);
        
        // check collision with the bats
        if (ball_velocity_x > 0 && ball.collides_with(bat[1])) {
          // to avoid internal bounces, only check the bats
          ball_velocity_x = -ball_velocity_x;
        } else if (ball_velocity_x < 0 && ball.collides_with(bat[0])) {
          ball_velocity_x = -ball_velocity_x;
        }
        
        ////////////////////ADDED CODE////////////////////
        
        //check collision with bats on top and bottom as long as they exist
        //and if the corresponding power up is not visible then set it to be visible and shrink the bat
        //otherwise keep the bat the same size
        if (ball_velocity_y > 0 && ball.collides_with(bat[2]) && bat[2].getWidth() > 0) {
            ball_velocity_y = -ball_velocity_y;
          if(powerUps[0]->isPowerUpVisible() == false){
              powerUps[0]->setPowerUpVisible(true);
              bat[2].scale(-0.5f,0);
              topScaleValue+=0.5f;
          }
        } else if (ball_velocity_y < 0 && ball.collides_with(bat[3]) && bat[3].getWidth() > 0) {
            ball_velocity_y = -ball_velocity_y;
          if(powerUps[1]->isPowerUpVisible() == false){
              powerUps[1]->setPowerUpVisible(true);
              bat[3].scale(-0.5f,0);
              bottomScaleValue+=0.5f;
          }
        }
        
        ////////////////////ADDED CODE////////////////////
        
        // check collision with the court top and bottom
        if (ball_velocity_y > 0 && ball.collides_with(court[1])) {
          ball_velocity_y = -ball_velocity_y;
        } else if (ball_velocity_y < 0 && ball.collides_with(court[0])) {
          ball_velocity_y = -ball_velocity_y;
        }
        
        // check collision with the court end zones
        if (ball.collides_with(court[2])) {
          scores[0]++;
          state = scores[0] >= 5 ? state_game_over : state_serving_left;
        } else if (ball.collides_with(court[3])) {
          scores[1]++;
          state = scores[1] >= 5 ? state_game_over : state_serving_right;
        }
        
        ////////////////////ADDED CODE////////////////////
        
        //loop through our powerups and if the ball collides with them and they are visible
        //do their effect then make them invisible again
        for (int i =0; i<powerUpArraySize;i++){
          if(ball.collides_with(*powerUps[i]) && powerUps[i]->isPowerUpVisible() == true){
            ball_velocity_x = powerUps[i]->effect(ball_velocity_x);
            ball_velocity_y = powerUps[i]->effect(ball_velocity_y);
            powerUps[i]->setPowerUpVisible(false);
          }
        }

        //adding in code for when the game finishes
      } else if (state == state_game_over){
        if (is_key_down('R')){
          
          //reset the scores
          scores[0] = scores[1] = 0;
          
          //randomize which bat starts with the ball
          left_or_right = rand()%2;
          
          if (left_or_right == true){
            state = state_serving_left;
          }
          else {
            state = state_serving_right;
          }
          
          //resize our top and bottom bats
          bat[2].scale(topScaleValue,0);
          bat[3].scale(bottomScaleValue,0);
          
          //remove all powerUps from the board
          for (int i =0; i<powerUpArraySize;i++){
            powerUps[i]->setPowerUpVisible(false);
          }
          
        }
      }
      
      ////////////////////ADDED CODE////////////////////
    }
    
  public:
    
    // this is called when we construct the class
    pong4_app(int argc, char **argv) : app(argc, argv) {
    }
    
    // this is called once OpenGL is initialized
    void app_init() {
      
      srand ((unsigned int)time(NULL));
      
      color_shader_.init();
      texture_shader_.init();
      cameraToWorld.loadIdentity();
      cameraToWorld.translate(0, 0, 5);
      
      ball.init(vec4(1, 1, 1, 1), 0, 0, 0.25f, 0.25f);
      bat[0].init(vec4(1, 1, 0, 1), -4.5f, 0, 0.2f, 1.5f);
      bat[1].init(vec4(0, 0, 1, 1),  4.5f, 0, 0.2f, 1.5f);
      
      ///////////////////////ADDED CODE///////////////////////
      
      bat[2].init(vec4(1, 0, 0, 1), 0, 3.5f, 4.0f, 0.2f);
      bat[3].init(vec4(0, 1, 0, 1), 0, -3.5f, 4.0f, 0.2f);
      
      ///////////////////////ADDED CODE///////////////////////
      
      court[0].init(vec4(1, 1, 1, 1), 0, -4, 10, 0.2f);
      court[1].init(vec4(1, 1, 1, 1), 0,  4, 10, 0.2f);
      court[2].init(vec4(1, 1, 1, 1), -5, 0, 0.4f, 8);
      court[3].init(vec4(1, 1, 1, 1), 5,  0, 0.4f, 8);
      
      ///////////////////////ADDED CODE///////////////////////
      
      //much like the reset code have the starting bat be random
      
      left_or_right = rand()%2;
      
      if (left_or_right == true){
      state = state_serving_left;
      }
      else {
      state = state_serving_right;
      }
      scores[0] = scores[1] = 0;
      
      ///////////////////////ADDED CODE///////////////////////
    }
    
    // this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      simulate();
      
      // set a viewport - includes whole window area
      glViewport(x, y, w, h);
      
      // clear the background to black
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      // allow Z buffer depth testing (closer objects are always drawn in front of far ones)
      glEnable(GL_DEPTH_TEST);
      
      // draw the ball
      ball.render(color_shader_, cameraToWorld);
      
      ///////////////////////ADDED CODE///////////////////////
      
      //loop through our powerups, if they are visible then render them, if they are not then
      //initialise them in a new random spot for when they become visible again
      
      GLuint powerUp1 = resources::get_texture_handle(GL_RGBA, "assets/pong4/speedUp.gif");
      GLuint powerUp2 = resources::get_texture_handle(GL_RGBA, "assets/pong4/speedDown.gif");
      
      for (int i =0; i<powerUpArraySize;i++){
        if(powerUps[i]->isPowerUpVisible() == true)powerUps[i]->render(texture_shader_, cameraToWorld);
        else if (i==0) powerUps[i]->init(powerUp1,((rand() % 6)-3)+ i, ((rand() % 4)-2)+ i, 0.64f, 0.64f);
        else powerUps[i]->init(powerUp2,((rand() % 6)-3)+ i, ((rand() % 4)-2)+ i, 0.64f, 0.64f);
      }
      
      ///////////////////////ADDED CODE///////////////////////
      
      // draw the bats
      for (int i = 0; i != 4; ++i) {
      //added check to only render the bat if it has a defined width (used for bats that can shrink)
        if(bat[i].getWidth() > 0) bat[i].render(color_shader_, cameraToWorld);
      }
      
      // draw the court
      for (int i = 0; i != 4; ++i) {
        court[i].render(color_shader_, cameraToWorld);
      }
    }
  };
}

