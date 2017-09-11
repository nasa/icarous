/* Copyright (c) 2006-2016, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <cassert>
#include <cmath>
#include <csignal>
#include <cstring>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "Debug.hh"

#include "MyOpenGL.hh"
#include "Macros.hh"
#include "EnergySources.hh"
#include "MazeTerrain.hh"
#include "Goals.hh"
#include "IpcRobotAdapter.hh"
#include "Robot.hh"
#include "RobotPositionServer.hh"

#define WINDOW_WIDTH	1024
#define WINDOW_HEIGHT	1024

static int windowId;
static bool cleanUp = false;
static MazeTerrain* terrain = NULL;
static EnergySources* resources = NULL;
static Goals* goals = NULL;
static RobotPositionServer* robotPoseServer = NULL;
static IpcRobotAdapter* ipcAdapter = NULL;
static std::vector<RobotBase*> robotList;
static std::vector<Robot*> animatedRobotList;
static pthread_t animationThread;


static void cleanUpFunction(void)
{
  cleanUp = true;

  // Terminate comms
  delete ipcAdapter;

  // Terminate animation thread
  pthread_join(animationThread, NULL);
  animatedRobotList.clear();

  delete terrain;
  delete resources;
  delete goals;
  delete robotPoseServer;
  
  for (std::vector<RobotBase*>::const_iterator iter = robotList.begin();
       iter != robotList.end();
       ++iter)
    delete *iter;
}

/* Signal handler for SIGINT. */
static void SIGINT_handler (int signum)
{
  assert (signum == SIGINT);
  std::cout << "Terminating simulator" << std::endl;
  cleanUp = true;
}

GLvoid ReSizeGLScene(int Width, int Height)
{
  glViewport(0, 0, (GLint)Width, (GLint)Height);
}

void display2(void)
{
  /* clear window */

  glClear(GL_COLOR_BUFFER_BIT);
  
  goals->displayGoals();
  resources->displayEnergySources();
  terrain->displayFixedTerrain();
  
  for(std::vector<RobotBase*>::const_iterator iter = robotList.begin();
      iter != robotList.end(); ++iter)
    (*iter)->displayRobot();
  
  glFlush();
  glutSwapBuffers();
}


void init()
{
  /* set clear color to black */
  glClearColor (0.0, 0.0, 0.0, 0.0);
  
  /* set fill color to white */
  glColor3f(1.0, 1.0, 1.0);
  
  /* set up standard orthogonal view with clipping */
  /* box as cube of side 2 centered at origin */
  /* This is default view and these statement could be removed */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}


void *threadLoop (void * /* ignored */)
{
  while (!cleanUp) {
    for (std::vector<Robot *>::iterator it = animatedRobotList.begin();
         it != animatedRobotList.end();
         ++it)
      (*it)->updateRobotPosition();
    sleep(1);
  }
  std::cout << "Exiting robot animation thread " << std::endl;
  return NULL;
}

void idleFunc(void)
{
  if (cleanUp)
    exit(0);
  usleep(40000); // update 25 times/sec
  glutPostRedisplay();
}


void readRobotLocations(const std::string& fName)
{
  std::string dataStr;
  static std::string const delimiter = " ";
  std::ifstream myFile;

  myFile.open(fName.c_str());
  if (!myFile.is_open()) {
    std::cerr << "The file: " << fName << " does not exist" << std::endl;
    exit(-1);
  }
  while (!myFile.eof()) {
    std::getline(myFile, dataStr);
    if (dataStr.substr(0,1) != "%") {
      std::string::size_type lastPos = dataStr.find_first_not_of(delimiter, 0);
      std::string::size_type pos = dataStr.find_first_of(delimiter, lastPos);
      CHECK_READ_DATA(pos, lastPos, myFile);
      std::string name = dataStr.substr(lastPos, pos - lastPos);
          
      lastPos = dataStr.find_first_not_of(delimiter, pos);
      pos = dataStr.find_first_of(delimiter, lastPos);
      CHECK_READ_DATA(pos, lastPos, myFile);
      std::string temp = dataStr.substr(lastPos, pos - lastPos);
      int x = atoi(temp.c_str());

      lastPos = dataStr.find_first_not_of(delimiter, pos);
      pos = dataStr.find_first_of(delimiter, lastPos);
      CHECK_READ_DATA(pos, lastPos, myFile);
      temp = dataStr.substr(lastPos, pos - lastPos);
      int y = atoi(temp.c_str());

      lastPos = dataStr.find_first_not_of(delimiter, pos);
      pos = dataStr.find_first_of(delimiter, lastPos);
      CHECK_READ_DATA(pos, lastPos, myFile);
      temp = dataStr.substr(lastPos, pos - lastPos);
      double r = atof(temp.c_str());

      lastPos = dataStr.find_first_not_of(delimiter, pos);
      pos = dataStr.find_first_of(delimiter, lastPos);
      CHECK_READ_DATA(pos, lastPos, myFile);
      temp = dataStr.substr(lastPos, pos - lastPos);
      double g = atof(temp.c_str());

      lastPos = dataStr.find_first_not_of(delimiter, pos);
      pos = dataStr.find_first_of(delimiter, lastPos);
      CHECK_READ_DATA(pos, lastPos, myFile);
      temp = dataStr.substr(lastPos, pos - lastPos);
      double b = atof(temp.c_str());

      Robot* robot = new Robot(terrain, 
                               resources,
                               goals, 
                               robotPoseServer,
                               *ipcAdapter, 
                               name, 
                               x, y,
                               r, g, b);
      robotList.push_back((RobotBase *) robot);
      if ((name != "RobotYellow") && (name != "RobotBlue3"))
        animatedRobotList.push_back(robot);
    }
  }
  myFile.close();
}

int main(int argc, char** argv)
{ 
  std::string usage("Usage: robotSimulator [-w <window-width>] [-centralhost <host:port>] [-d <debug config file>];\
  window-width defaults to 1024\
  host:port defaults to localhost:1381\
  debug config file defaults to Debug.cfg");

  // parse command line parameters
  int width = WINDOW_WIDTH;
  std::string centralhost("localhost:1381");
  std::string debugConfig("Debug.cfg");

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-w") == 0)
      width = atoi(argv[++i]);
    else if (strcmp(argv[i], "-centralhost") == 0)
      centralhost = std::string(argv[++i]);
    else if (strcmp(argv[i], "-d") == 0)
      debugConfig = std::string(argv[++i]);
    else if (strcmp(argv[i], "-h") == 0) {
      std::cout << usage << std::endl;
      return 1;
    }
    else {
      std::cout << "Unknown option '" 
                << argv[i] 
                << "'.  " 
                << usage 
                << std::endl;
      return -1;
    }
  }

  std::ifstream config(debugConfig.c_str());
  if (config.good())
    PLEXIL::readDebugConfigStream(config);

  ipcAdapter = new IpcRobotAdapter(centralhost);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(width, width);
  glutInitWindowPosition(100, 100);
  windowId = glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(display2);
  glutReshapeFunc(ReSizeGLScene);

  terrain = new MazeTerrain("maze32.data");
  resources = new EnergySources("energySource.data", terrain->getHeight());
  goals = new Goals(terrain->getHeight(), 25.5);
  robotPoseServer = new RobotPositionServer(terrain->getHeight(), terrain->getWidth());

  readRobotLocations("Robots.data");
  glutIdleFunc(idleFunc);


  // Setup signal handling before starting the main OpenGL loop
  struct sigaction sa;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = 0;
  //Register the handler for SIGINT.
  sa.sa_handler = SIGINT_handler;
  sigaction (SIGINT, &sa, 0);

  // Needed to handle the case when the application is terminated by closing 
  // the window.
  atexit(cleanUpFunction);

  // Spawn robot animation thread
  pthread_attr_t attr;
  pthread_attr_init (&attr);
  if (pthread_create(&animationThread, &attr, threadLoop, NULL) !=  0) {
    std::cerr << "pthread_create failed for robot animation loop" << std::endl;
    exit(1);
  }

  glutMainLoop();

  return 0;
}


