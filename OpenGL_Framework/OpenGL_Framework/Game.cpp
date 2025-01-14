#include "Game.h"
#include "Utilities.h"

/*
Keys:
$$$ - Particle Signal

*/

#define FULLSCREEN true
#define VSYNC true

Game::Game()
	: GBuffer(3), DeferredComposite(1), ShadowMap(0), EdgeMap(1), WorkBuffer1(1), WorkBuffer2(1), HudMap(1), ssaoWorkBuffer1(1), ssaoWorkBuffer2(1) //, godRaysBuffer1(1), godRaysBuffer2(1)
	//This constructor in the initializer list is to solve the issue of creating a frame buffer object without no default constructor
	//This will occur before the brackets of the constructor starts (Reference at Week #6 video Time: 47:00)
	//The number is the number of color textures
{
}

Game::~Game()
{
	delete updateTimer;

	//StaticGeometry.UnLoad();
	BloomHighPass.UnLoad();
	BlurHorizontal.UnLoad();
	BlurVertical.UnLoad();
	BloomComposite.UnLoad();
	GBufferPass.UnLoad();
	DeferredLighting.UnLoad();
	SobelPass.UnLoad();
	AniShader.UnLoad();
	HudShader.UnLoad();
	PointLight.UnLoad();
	ParticleProgram.UnLoad();
	boxMesh.Unload();
	boxTexture.Unload();
	HudObj.Unload();
	P1Hud.Unload();
	P1Bar.Unload();
	P2Hud.Unload();
	P2Bar.Unload();
	StepTexture.Unload();
	SSAO.UnLoad();
	SSAOBlur.UnLoad();
}

bool WGLExtensionSupported(const char *extension_name)
{
	// this is pointer to function which returns pointer to string with list of all wgl extensions
	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

	// determine pointer to wglGetExtensionsStringEXT function
	_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

	if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
	{
		// string was not found
		return false;
	}

	// extension is supported
	return true;
}

void Game::initializeGame()
{

	//Only needs to be done once
	glEnable(GL_DEPTH_TEST);
	
	//glutFullScreen();
	InitFullScreenQuad();
	if (FULLSCREEN)
		glutFullScreen();

	//init vsync
	PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;
	PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = NULL;

	if (WGLExtensionSupported("WGL_EXT_swap_control"))
	{
		// Extension is supported, init pointers.
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

		// this is another function from WGL_EXT_swap_control extension
		wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
	}
	if (VSYNC)
		wglSwapIntervalEXT(1);
	else
		wglSwapIntervalEXT(0);


//=================================================================//
	//Load All Game Objects
	///Object(*mesh path*, *texture path*, *enable blending?* = false by deafult);
		
	//load objects for scene
	gameObjects.push_back(new Object("./Assets/Models/basicCourtHigh", "./Assets/Textures/basicCourt.png", "default_court"));
	gameObjects.push_back(new Object("./Assets/Models/basicFloor", "./Assets/Textures/basicFloor.png", "default_floor"));
	default_court_objs.push_back("default_floor");
	gameObjects.push_back(new Object("./Assets/Models/BasicNets", "./Assets/Textures/net.png", "default_net", true));
	default_court_objs.push_back("default_net");
	gameObjects.push_back(new Object("./Assets/Models/chairs", "./Assets/Textures/chair.png", "default_chairs"));
	default_court_objs.push_back("default_chairs");
	gameObjects.push_back(new Object("./Assets/Models/lightsJumbo", "./Assets/Textures/lightJumboTex.png", "default_jumbotron", true));
	default_court_objs.push_back("default_jumbotron");
	gameObjects.push_back(new Object("./Assets/Models/ad", "./Assets/Textures/words.png", "default_words"));
	default_court_objs.push_back("default_words");
	gameObjects.push_back(new Object("./Assets/Models/bottles", "./Assets/Textures/bottleTex.png", "default_bottles", true));
	default_court_objs.push_back("default_bottles");
	gameObjects.push_back(new Object("./Assets/Models/score", "./Assets/Textures/score.png", "default_score"));
	default_court_objs.push_back("default_score");
	gameObjects.push_back(new Object("./Assets/Models/basicCrowd1", "./Assets/Textures/crowd.png", "default_crowd1"));
	default_court_objs.push_back("default_crowd1");
	gameObjects.push_back(new Object("./Assets/Models/basicCrowd2", "./Assets/Textures/crowd.png", "default_crowd2"));
	default_court_objs.push_back("default_crowd2");
	gameObjects.push_back(new Object("./Assets/Models/basicCrowd3", "./Assets/Textures/crowd.png", "default_crowd3"));
	default_court_objs.push_back("default_crowd3");

	//gameObjects.push_back(new Object("./Assets/Models/nets", "./Assets/Textures/smokeNet.png", "smoke_net"));
	//default_court_objs.push_back("smoke_net");


	gameObjects.push_back(new Object("./Assets/Models/basicBricks", "./Assets/Textures/basicBricks.png", "default_bricks"));
	default_court_objs.push_back("default_bricks");

	//load objects for knight scene
	gameObjects.push_back(new Object("./Assets/Models/knightCourt1", "./Assets/Textures/knightCourt.png", "knight_court"));
	//gameObjects.push_back(new Object("./Assets/Models/knightFloor", "./Assets/Textures/knightFloor.png", "knight_floor"));
	//knight_court_objs.push_back("knight_floor");

	gameObjects.push_back(new Object("./Assets/Models/knightBleachers", "./Assets/Textures/knightBleachers.png", "knight_bleachers"));
	knight_court_objs.push_back("knight_bleachers");
	gameObjects.push_back(new Object("./Assets/Models/knightChair", "./Assets/Textures/knightChair.png", "knight_chairs"));
	knight_court_objs.push_back("knight_chairs");
	gameObjects.push_back(new Object("./Assets/Models/knightCrowd1", "./Assets/Textures/crowd.png", "knight_crowd1"));
	knight_court_objs.push_back("knight_crowd1");
	gameObjects.push_back(new Object("./Assets/Models/knightCrowd2", "./Assets/Textures/crowd.png", "knight_crowd2"));
	knight_court_objs.push_back("knight_crowd2");
	gameObjects.push_back(new Object("./Assets/Models/knightCrowd3", "./Assets/Textures/crowd.png", "knight_crowd3"));
	knight_court_objs.push_back("knight_crowd3");
	gameObjects.push_back(new Object("./Assets/Models/knightTorches", "./Assets/Textures/knightTorch.png", "knight_torch"));
	knight_court_objs.push_back("knight_torch");
	gameObjects.push_back(new Object("./Assets/Models/knightBricks", "./Assets/Textures/knightBricks.png", "knight_bricks"));
	knight_court_objs.push_back("knight_bricks");
	gameObjects.push_back(new Object("./Assets/Models/knightJumbo", "./Assets/Textures/knightJumbo.png", "knight_jumbo"));
	knight_court_objs.push_back("knight_jumbo");
	gameObjects.push_back(new Object("./Assets/Models/ad", "./Assets/Textures/words.png", "default_words"));
	knight_court_objs.push_back("default_words");
	gameObjects.push_back(new Object("./Assets/Models/nets", "./Assets/Textures/fireNet.png", "fire_net"));
	knight_court_objs.push_back("fire_net");

	//load objects for ninja scene
	gameObjects.push_back(new Object("./Assets/Models/ninjaCourt", "./Assets/Textures/ninjaCourt.png", "ninja_court"));

	gameObjects.push_back(new Object("./Assets/Models/ninjaBack", "./Assets/Textures/ninjaBack.png", "ninja_background"));
	ninja_court_objs.push_back("ninja_background");
	gameObjects.push_back(new Object("./Assets/Models/ninjaBridge", "./Assets/Textures/ninjaBridge.png", "ninja_bridge"));
	ninja_court_objs.push_back("ninja_bridge");
	gameObjects.push_back(new Object("./Assets/Models/ninjaCrowd1", "./Assets/Textures/crowd.png", "ninja_crowd1"));
	ninja_court_objs.push_back("ninja_crowd1");
	gameObjects.push_back(new Object("./Assets/Models/ninjaCrowd2", "./Assets/Textures/crowd.png", "ninja_crowd2"));
	ninja_court_objs.push_back("ninja_crowd2");
	gameObjects.push_back(new Object("./Assets/Models/ninjaCrowd3", "./Assets/Textures/crowd.png", "ninja_crowd3"));
	ninja_court_objs.push_back("ninja_crowd3");
	gameObjects.push_back(new Object("./Assets/Models/ninjaSides", "./Assets/Textures/ninjaSides.png", "ninja_sides"));
	ninja_court_objs.push_back("ninja_sides");
	gameObjects.push_back(new Object("./Assets/Models/score", "./Assets/Textures/score.png", "ninja_score"));
	ninja_court_objs.push_back("ninja_score");
	gameObjects.push_back(new Object("./Assets/Models/knightJumbo", "./Assets/Textures/knightJumbo.png", "ninja_jumbo"));
	ninja_court_objs.push_back("ninja_jumbo");
	gameObjects.push_back(new Object("./Assets/Models/ad", "./Assets/Textures/words.png", "default_words"));
	ninja_court_objs.push_back("default_words");
	gameObjects.push_back(new Object("./Assets/Models/nets", "./Assets/Textures/petalNet.png", "petals_net"));
	ninja_court_objs.push_back("petals_net");


	//hitbox
	hitboxObj = new Object("./Assets/Models/Hitbox", "./Assets/Textures/redclear.png", "hitbox", true);
	

	//Load All Main Menu Objects
	menuObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/titleScreen.png", "background", true));
	if (FULLSCREEN)
		menuObjects[0]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	else
		menuObjects[0]->setScale(glm::vec3(WINDOW_WIDTH *0.35f, WINDOW_HEIGHT *0.51f, 1));
	menuObjects[0]->RotateY(90);
	menuObjects[0]->setPosition(glm::vec3(0, -555, -1));
	

	menuObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/startButton.png", "button1", true));
	if (FULLSCREEN) 
		menuObjects[1]->setScale(300.0f);
	else 
		menuObjects[1]->setScale(200.0f);
	menuObjects[1]->RotateY(90.0f);
	menuObjects[1]->setPosition(glm::vec3(570, -250, 0));


	menuObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/tutButton.png", "button2", true));
	if (FULLSCREEN)
		menuObjects[2]->setScale(300.0f);
	else
		menuObjects[2]->setScale(200.0f);
	menuObjects[2]->RotateY(90.0f);
	menuObjects[2]->setPosition(glm::vec3(570, -450, 0));

	menuObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/quitButton.png", "button3", true));
	if (FULLSCREEN)
		menuObjects[3]->setScale(300.0f);
	else
		menuObjects[3]->setScale(200.0f);
	menuObjects[3]->RotateY(90.0f);
	menuObjects[3]->setPosition(glm::vec3(570, -650, 0));


	//Load All Character Select Objects

	///background image
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/selectback.png", "background", true));
	if (FULLSCREEN)
		cssObjects[0]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	else
		cssObjects[0]->setScale(glm::vec3(WINDOW_WIDTH *0.35f, WINDOW_HEIGHT *0.51f, 1));
	cssObjects[0]->RotateY(90);
	cssObjects[0]->setPosition(glm::vec3(0, -555, -1));

	///knight Icon
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/KnightIcon.png", "knightIcon", true));
	if (FULLSCREEN)
		cssObjects[1]->setScale(120.0f);
	else
		cssObjects[1]->setScale(80.0f);
	cssObjects[1]->RotateY(90.0f);
	cssObjects[1]->setPosition(glm::vec3(-200, -100, 0));

	///ninja Icon
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/NinjaIcon.png", "ninjaIcon", true));
	if (FULLSCREEN)
		cssObjects[2]->setScale(120.0f);
	else
		cssObjects[2]->setScale(80.0f);
	cssObjects[2]->RotateY(90.0f);
	cssObjects[2]->setPosition(glm::vec3(200, -100, 0));

	///p1 Icon
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/p1select.png", "p1Select", true));
	if (FULLSCREEN)
		cssObjects[3]->setScale(120.0f);
	else
		cssObjects[3]->setScale(80.0f);
	cssObjects[3]->RotateY(90.0f);
	cssObjects[3]->setPosition(glm::vec3(-200, -100, 2));

	///p2 Icon
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/p2select.png", "p2Select", true));
	if (FULLSCREEN)
		cssObjects[4]->setScale(120.0f);
	else
		cssObjects[4]->setScale(80.0f);
	cssObjects[4]->RotateY(90.0f);
	cssObjects[4]->setPosition(glm::vec3(200, -100, 1));


	///p1 knight pic
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/P1KnightPic.png", "p1KnightPic", true));
	if (FULLSCREEN)
		cssObjects[5]->setScale(300.0f);
	else
		cssObjects[5]->setScale(200.0f);
	cssObjects[5]->RotateY(90.0f);
	cssObjects[5]->setPosition(glm::vec3(-530, -550, 2));

	///p2 knight pic
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/P2KnightPic.png", "p2KnightPic", true));
	if (FULLSCREEN)
		cssObjects[6]->setScale(300.0f);
	else
		cssObjects[6]->setScale(200.0f);
	cssObjects[6]->RotateY(90.0f);
	cssObjects[6]->setPosition(glm::vec3(530, -550, 1));

	///p1 ninja pic
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/P1NinjaPic.png", "p1NinjaPic", true));
	if (FULLSCREEN)
		cssObjects[7]->setScale(300.0f);
	else
		cssObjects[7]->setScale(200.0f);
	cssObjects[7]->RotateY(90.0f);
	cssObjects[7]->setPosition(glm::vec3(-530, -550, 2));
	cssObjects[7]->hide = true;

	///p2 nijna pic
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/P2NinjaPic.png", "p2NinjaPic", true));
	if (FULLSCREEN)
		cssObjects[8]->setScale(300.0f);
	else
		cssObjects[8]->setScale(200.0f);
	cssObjects[8]->RotateY(90.0f);
	cssObjects[8]->setPosition(glm::vec3(530, -550, 1));
	cssObjects[8]->hide = true;
	// Stage Select Screen

///background image
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/stageSelect.png", "background", true));
	if (FULLSCREEN)
		sssObjects[0]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	else
		sssObjects[0]->setScale(glm::vec3(WINDOW_WIDTH *0.35f, WINDOW_HEIGHT *0.51f, 1));
	sssObjects[0]->RotateY(90);
	sssObjects[0]->setPosition(glm::vec3(0, -555, -1));

	///knight stage Icon
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/basicCourtSelect.png", "courtIconDefault", true));
	if (FULLSCREEN)
		sssObjects[1]->setScale(120.0f);
	else
		sssObjects[1]->setScale(80.0f);
	sssObjects[1]->RotateY(90.0f);
	sssObjects[1]->setPosition(glm::vec3(-350, -500, 0));

	///default stage Icon
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/knightCourtSelect.png", "courtIconKnight", true));
	if (FULLSCREEN)
		sssObjects[2]->setScale(120.0f);
	else
		sssObjects[2]->setScale(80.0f);
	sssObjects[2]->RotateY(90.0f);
	sssObjects[2]->setPosition(glm::vec3(0, -500, 0));

	///ninja stage Icon
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/ninjaCourtSelect.png", "courtIconNinja", true));
	if (FULLSCREEN)
		sssObjects[3]->setScale(120.0f);
	else
		sssObjects[3]->setScale(80.0f);
	sssObjects[3]->RotateY(90.0f);
	sssObjects[3]->setPosition(glm::vec3(350, -500, 0));

	///select Icon
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/p1select.png", "select", true));
	if (FULLSCREEN)
		sssObjects[4]->setScale(120.0f);
	else
		sssObjects[4]->setScale(80.0f);
	sssObjects[4]->RotateY(90.0f);
	sssObjects[4]->setPosition(glm::vec3(-350, -500, 2));

	///basic court pic
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/basicStage.png", "courtPicDefault", true));
	if (FULLSCREEN)
		sssObjects[5]->setScale(glm::vec3(500.0f, 350.0f, 1.0f));
	else
		sssObjects[5]->setScale(glm::vec3(330.0f, 220.0f, 1.0f));
	sssObjects[5]->RotateY(90.0f);
	sssObjects[5]->setPosition(glm::vec3(0, -300, 0));

	///knight court pic
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/knightStage.png", "courtPicKnight", true));
	if (FULLSCREEN)
		sssObjects[6]->setScale(glm::vec3(500.0f, 350.0f, 1.0f));
	else
		sssObjects[6]->setScale(glm::vec3(330.0f, 220.0f, 1.0f));
	sssObjects[6]->RotateY(90.0f);
	sssObjects[6]->setPosition(glm::vec3(0, -300, 0));

	///ninja court pic
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/ninjaStage.png", "courtPicNinja", true));
	if (FULLSCREEN)
		sssObjects[7]->setScale(glm::vec3(500.0f, 350.0f, 1.0f));
	else
		sssObjects[7]->setScale(glm::vec3(330.0f, 220.0f, 1.0f));
	sssObjects[7]->RotateY(90.0f);
	sssObjects[7]->setPosition(glm::vec3(0, -300, 0));

	//End Screen Assets
	///Background image
	endObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/knightRedWin.png", "redKnightEnd", true));
	if (FULLSCREEN)
		endObjects[0]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	else
		endObjects[0]->setScale(glm::vec3(WINDOW_WIDTH *0.35f, WINDOW_HEIGHT *0.51f, 1));
	endObjects[0]->RotateY(90);
	endObjects[0]->setPosition(glm::vec3(0, -555, -1));

	endObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/knightBlueWin.png", "blueKnightEnd", true));
	if (FULLSCREEN)
		endObjects[1]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	else
		endObjects[1]->setScale(glm::vec3(WINDOW_WIDTH *0.35f, WINDOW_HEIGHT *0.51f, 1));
	endObjects[1]->RotateY(90);
	endObjects[1]->setPosition(glm::vec3(0, -555, -1));

	endObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/ninjaRedWin.png", "redNinjaRed", true));
	if (FULLSCREEN)
		endObjects[2]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	else
		endObjects[2]->setScale(glm::vec3(WINDOW_WIDTH *0.35f, WINDOW_HEIGHT *0.51f, 1));
	endObjects[2]->RotateY(90);
	endObjects[2]->setPosition(glm::vec3(0, -555, -1));

	endObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/ninjaBlueWin.png", "blueNinjaEnd", true));
	if (FULLSCREEN)
		endObjects[3]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	else
		endObjects[3]->setScale(glm::vec3(WINDOW_WIDTH *0.35f, WINDOW_HEIGHT *0.51f, 1));
	endObjects[3]->RotateY(90);
	endObjects[3]->setPosition(glm::vec3(0, -555, -1));

	endObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/tie.png", "tieGameEnd", true));
	if (FULLSCREEN)
		endObjects[4]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	else
		endObjects[4]->setScale(glm::vec3(WINDOW_WIDTH *0.35f, WINDOW_HEIGHT *0.51f, 1));
	endObjects[4]->RotateY(90);
	endObjects[4]->setPosition(glm::vec3(0, -555, -1));

	//Tutorial Screen Assets
	///Background image
	tutObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/howTo.png", "tutorial", true));
	if (FULLSCREEN)
		tutObjects[0]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	else
		tutObjects[0]->setScale(glm::vec3(WINDOW_WIDTH *0.35f, WINDOW_HEIGHT *0.51f, 1));
	tutObjects[0]->RotateY(90);
	tutObjects[0]->setPosition(glm::vec3(0, -555, -1));

//================================================================//
	//Init PointLights
	///PointLightObj(*position*, *color*, *name*, *active?* = false by default);
	pointLights.push_back(new PointLightObj(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), "p1Score", false));
	pointLights.push_back(new PointLightObj(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), "p2Score", false));
	pointLights.push_back(new PointLightObj(glm::vec3(-18.0f, 10.0f, -19.0f), glm::vec3(1.0f, 1.0f, 0.6f), "lightLeft", false));
	pointLights.push_back(new PointLightObj(glm::vec3(18.0f, 10.0f, -19.0f), glm::vec3(1.0f, 1.0f, 0.6f), "lightRight", false));
	pointLights.push_back(new PointLightObj(glm::vec3(0.0f, 10.0f, -19.0f), glm::vec3(0.6f, 0.6f, 0.0f), "lightCenter", false));

	for (int i= 0; i < 1000; i++) {
		
		thousandLights.push_back(new PointLightObj(glm::vec3(std::rand() % 200 - 100, std::rand() % 200 - 100, std::rand() % 200 - 100), glm::vec3(1.0f, 1.0f, 1.0f), "RandomLight" + to_string(i) , false));
		thousandLights[i]->active = true;
	}


//================================================================//
	//Load Hud Obj and Texture

	std::vector<std::string> hud1;
	hud1.push_back("./Assets/Models/UI_Object");
	HudObj.LoadFromFile(hud1);

	/*if (!P1Hud.Load("./Assets/Textures/PlayerOneHud.png"))
	{
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}*/
	if (!P1Bar.Load("./Assets/Textures/PlayerOneBar.png"))
	{
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	/*if (!P2Hud.Load("./Assets/Textures/PlayerTwoHud.png"))
	{
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}*/
	if (!P2Bar.Load("./Assets/Textures/PlayerTwoBar.png"))
	{
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	if (!StepTexture.Load("./Assets/Textures/StepTexture.png"))
	{
		std::cout << "Step Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	StepTexture.SetNearestFilter();


//===================================================================//
	//Init Controls and Players


	inputs = { false, false, false, false, false, false, false, false, false, false, false }; //up, left, down, right, X, Y, A, LeftFull, RightFull, R, B


	knightTemp = (new Knight("./Assets/Models/Knight", "./Assets/Textures/player1.png"));
	ninjaTemp = (new Ninja("./Assets/Models/Knight", "./Assets/Textures/player1ninja.png"));
	//player1
	//players[0] = (new Knight("./Assets/Models/Knight.obj", "./Assets/Textures/player1.png"));

	inputs2 = { false, false, false, false, false, false, false, false, false, false, false }; //up, left, down, right, X, Y, A, LeftFull, RightFull, R, B

	//player2
	//players[1] = (new Ninja("./Assets/Models/Knight.obj", "./Assets/Textures/player2.png"));


//====================================================================//
	//Load Shaders
	if (!HudShader.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/HudShader.frag"))
	{
		std::cout << "GBP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!GBufferPass.Load("./Assets/Shaders/StaticGeometry.vert", "./Assets/Shaders/GBufferPass.frag"))
	{
		std::cout << "GBP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!BloomHighPass.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BloomHighPass.frag"))
	{
		std::cout << "BHP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
	if (!SSAO.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/SSAO.frag"))
	{
		std::cout << "BHP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}	
	if (!SSAOBlur.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/SSAOBlur.frag"))
	{
		std::cout << "BHP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}


	if (!BlurHorizontal.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BlurHorizontal.frag"))
	{
		std::cout << "BH Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!BlurVertical.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BlurVertical.frag"))
	{
		std::cout << "BV Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!BloomComposite.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BloomComposite.frag"))
	{
		std::cout << "BC Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!DeferredLighting.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/DeferredLighting.frag"))
	{
		std::cout << "DL Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!AniShader.Load("./Assets/Shaders/AnimationShader.vert", "./Assets/Shaders/GBufferPass.frag"))
	{
		std::cout << "AS Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!PointLight.Load("./Assets/Shaders/PassThroughLight.vert", "./Assets/Shaders/PointLight.frag"))
	{
		std::cout << "SL Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
	if (!AdShader.Load("./Assets/Shaders/AdShader.vert", "./Assets/Shaders/GBufferPass.frag"))
	{
		std::cout << "ADS Shaders failed to initialize. \n";
		system("pause");
		exit(0);
	}

	if (!GrayScale.Load("./Assets/Shaders/Passthrough.vert", "./Assets/Shaders/GreyScalePost.frag"))
	{
		std::cout << "GS Shaders failed to initialize. \n";
		system("pause");
		exit(0);
	}

	if (!Sepia.Load("./Assets/Shaders/Passthrough.vert", "./Assets/Shaders/Sepia.frag"))
	{
		std::cout << "S Shaders failed to initialize. \n";
		system("pause");
		exit(0);
	}

	if (!Negative.Load("./Assets/Shaders/Passthrough.vert", "./Assets/Shaders/Negative.frag"))
	{
		std::cout << "N Shaders failed to initialize. \n";
		system("pause");
		exit(0);
	}

	if (!NetShader.Load("./Assets/Shaders/NetShader.vert", "./Assets/Shaders/GBufferPass.frag"))
	{
		std::cout << "NS Shaders failed to initialize. \n";
		system("pause");
		exit(0);
	}

	if (!SobelPass.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Toon/Sobel.frag"))
	{
		std::cout << "SP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
	//Load the god rays shaders here

//================================================================//
	//Particle Program

	if (!ParticleProgram.Load(
		"./Assets/Shaders/Particles/BillBoard.vert",
		"./Assets/Shaders/Particles/BillBoard.frag",
		"./Assets/Shaders/Particles/BillBoard.geom"))
	{
		std::cout << "PP failed to initialize.\n";
		system("pause");
		exit(0);
	}

	/*if (!ConfettiEffectBlueRight.Init("./Assets/Textures/BlueConfetti.png", (unsigned int)50, (unsigned int)30))
	{
		std::cout << "Confetti Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	//Missing .Set which is what the video uses***
	ConfettiEffectBlueRight.LerpAlpha = glm::vec2(0.4f, 0.8f);
	ConfettiEffectBlueRight.LerpSize = glm::vec2(1.0f, 2.0f);
	ConfettiEffectBlueRight.RangeLifetime = glm::vec2(2.0f, 3.0f);
	ConfettiEffectBlueRight.RangeVelocity = glm::vec2(-5.0f, 5.0f);
	ConfettiEffectBlueRight.RangeX = glm::vec2(12.0f, 12.0f);
	ConfettiEffectBlueRight.RangeY = glm::vec2(20.0f, 20.0f);
	ConfettiEffectBlueRight.RangeZ = glm::vec2(-15.0f, -15.0f);
	ConfettiEffectBlueRight.HaveGravity = true;
	ConfettiEffectBlueRight.Mass = 2.0f;
	ConfettiEffectBlueRight.Gravity = 0.2f;*/

	/*if (!ConfettiEffectBlueLeft.Init("./Assets/Textures/BlueConfetti.png", (unsigned int)50, (unsigned int)30))
	{
		std::cout << "Confetti Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	ConfettiEffectBlueLeft.LerpAlpha = glm::vec2(0.4f, 0.8f);
	ConfettiEffectBlueLeft.LerpSize = glm::vec2(1.0f, 2.0f);
	ConfettiEffectBlueLeft.RangeLifetime = glm::vec2(2.0f, 3.0f);
	ConfettiEffectBlueLeft.RangeVelocity = glm::vec2(-5.0f, 5.0f);
	ConfettiEffectBlueLeft.RangeX = glm::vec2(-12.0f, -12.0f);
	ConfettiEffectBlueLeft.RangeY = glm::vec2(20.0f, 20.0f);
	ConfettiEffectBlueLeft.RangeZ = glm::vec2(-15.0f, -15.0f);
	ConfettiEffectBlueLeft.HaveGravity = true;
	ConfettiEffectBlueLeft.Mass = 2.0f;
	ConfettiEffectBlueLeft.Gravity = 0.2f;*/

	//better version
	//ConfettiEffectRedRight.PartiParse("./Assets/Data/text.txt", "./Assets/Textures/RedConfetti.png");
	
	
	NinjaPetals.PartiParse("./Assets/Data/petals.txt", "./Assets/Textures/petals.png");
	NinjaPetals2.PartiParse("./Assets/Data/petals.txt", "./Assets/Textures/petals2.png");
	
	NinjaPetals.mainField.init();
	NinjaPetals2.mainField.init();

	//Depricated code
	//if (!ConfettiEffectRedRight.Init("./Assets/Textures/RedConfetti.png", (unsigned int)50, (unsigned int)30))
	//{
	//	std::cout << "Confetti Particle-Effect failed ot initialize.\n";
	//	system("pause");
	//	exit(0);
	//}
	////Missing .Set which is what the video uses***
	//ConfettiEffectRedRight.LerpAlpha = glm::vec2(0.4f, 0.8f);
	//ConfettiEffectRedRight.LerpSize = glm::vec2(1.0f, 2.0f);
	//ConfettiEffectRedRight.RangeLifetime = glm::vec2(2.0f, 3.0f);
	//ConfettiEffectRedRight.RangeVelocity = glm::vec2(-5.0f, 5.0f);
	//ConfettiEffectRedRight.RangeX = glm::vec2(12.0f, 12.0f);
	//ConfettiEffectRedRight.RangeY = glm::vec2(20.0f, 20.0f);
	//ConfettiEffectRedRight.RangeZ = glm::vec2(-15.0f, -15.0f);
	//ConfettiEffectRedRight.HaveGravity = true;
	//ConfettiEffectRedRight.Mass = 2.0f;
	//ConfettiEffectRedRight.Gravity = 0.2f;


	knightLeftNet.PartiParse("./Assets/Data/leftNet.txt", "./Assets/Textures/fire2.png");
	knightRightNet.PartiParse("./Assets/Data/rightNet.txt", "./Assets/Textures/fire2.png");
	knightLeftNet1.PartiParse("./Assets/Data/leftNet1.txt", "./Assets/Textures/fire1.png");
	knightRightNet1.PartiParse("./Assets/Data/rightNet1.txt", "./Assets/Textures/fire1.png");

	basicLeftNet.PartiParse("./Assets/Data/leftNet1.txt", "./Assets/Textures/puff2.png");
	basicRightNet.PartiParse("./Assets/Data/rightNet1.txt", "./Assets/Textures/puff2.png");
	basicLeftNet1.PartiParse("./Assets/Data/leftNet.txt", "./Assets/Textures/puff1.png");
	basicRightNet1.PartiParse("./Assets/Data/rightNet.txt", "./Assets/Textures/puff1.png");

	ninjaLeftNet.PartiParse("./Assets/Data/leftNet.txt", "./Assets/Textures/flower2.png");
	ninjaRightNet.PartiParse("./Assets/Data/rightNet.txt", "./Assets/Textures/flower2.png");
	ninjaLeftNet1.PartiParse("./Assets/Data/leftNet1.txt", "./Assets/Textures/flower1.png");
	ninjaRightNet1.PartiParse("./Assets/Data/rightNet1.txt", "./Assets/Textures/flower1.png");

	ConfettiEffectRedRight.PartiParse("./Assets/Data/redRight.txt", "./Assets/Textures/red.png");
	ConfettiEffectOrangeRight.PartiParse("./Assets/Data/redRight.txt", "./Assets/Textures/orange.png");

	ConfettiEffectRedLeft.PartiParse("./Assets/Data/left.txt", "./Assets/Textures/red.png");
	ConfettiEffectOrangeLeft.PartiParse("./Assets/Data/left.txt", "./Assets/Textures/orange.png");

	ConfettiEffectBlueLeft.PartiParse("./Assets/Data/left.txt", "./Assets/Textures/blue.png");
	ConfettiEffectPurpleLeft.PartiParse("./Assets/Data/left.txt", "./Assets/Textures/purple.png");

	ConfettiEffectBlueRight.PartiParse("./Assets/Data/redRight.txt", "./Assets/Textures/blue.png");
	ConfettiEffectPurpleRight.PartiParse("./Assets/Data/redRight.txt", "./Assets/Textures/purple.png");


	//if (!ConfettiEffectRedLeft.Init("./Assets/Textures/RedConfetti.png", (unsigned int)50, (unsigned int)30))
	//{
	//	std::cout << "Confetti Particle-Effect failed ot initialize.\n";
	//	system("pause");
	//	exit(0);
	//}
	//ConfettiEffectRedLeft.LerpAlpha = glm::vec2(0.4f, 0.8f);
	//ConfettiEffectRedLeft.LerpSize = glm::vec2(1.0f, 2.0f);
	//ConfettiEffectRedLeft.RangeLifetime = glm::vec2(2.0f, 3.0f);
	//ConfettiEffectRedLeft.RangeVelocity = glm::vec2(-5.0f, 5.0f);
	//ConfettiEffectRedLeft.RangeX = glm::vec2(-12.0f, -12.0f);
	//ConfettiEffectRedLeft.RangeY = glm::vec2(20.0f, 20.0f);
	//ConfettiEffectRedLeft.RangeZ = glm::vec2(-15.0f, -15.0f);
	//ConfettiEffectRedLeft.HaveGravity = true;
	//ConfettiEffectRedLeft.Mass = 2.0f;
	//ConfettiEffectRedLeft.Gravity = 0.2f;

	if (!DustLand.Init("./Assets/Textures/fog.png", (unsigned int)50, (unsigned int)2))
	{
		std::cout << "Dust Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	DustLand.LerpAlpha = glm::vec2(0.8f, 0.0f);
	DustLand.LerpSize = glm::vec2(1.0f, 3.0f);
	DustLand.RangeLifetime = glm::vec2(0.1f, 0.5f);
	DustLand.RangeVelocity = glm::vec2(3.0f, 7.0f);
	DustLand.RangeZ = glm::vec2(-2.1f, -2.0f);
	DustLand.InitialXRange = glm::vec2(-15.0f, 15.0f);
	DustLand.InitialYRange = glm::vec2(0.1f, 0.5f);
	DustLand.HaveGravity = true;
	DustLand.Mass = 1.0f;
	DustLand.Gravity = -0.15f;

	if (!DustDashL.Init("./Assets/Textures/fog.png", (unsigned int)50, (unsigned int)1))
	{
		std::cout << "Dust Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	DustDashL.LerpAlpha = glm::vec2(1.0f, 0.0f);
	DustDashL.LerpSize = glm::vec2(1.5f, 3.0f);
	DustDashL.RangeLifetime = glm::vec2(0.1f, 0.5f);
	DustDashL.RangeVelocity = glm::vec2(1.0f, 5.0f);
	DustDashL.RangeZ = glm::vec2(-2.1f, -2.1f);
	DustDashL.InitialXRange = glm::vec2(-5.0f, -3.0f);
	DustDashL.InitialYRange = glm::vec2(0.5f, 1.0f);
	DustDashL.HaveGravity = true;
	DustDashL.Mass = 1.0f;
	DustDashL.Gravity = -0.15f;

	if (!DustDashR.Init("./Assets/Textures/fog.png", (unsigned int)50, (unsigned int)1))
	{
		std::cout << "Dust Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	DustDashR.LerpAlpha = glm::vec2(1.0f, 0.0f);
	DustDashR.LerpSize = glm::vec2(1.5f, 3.0f);
	DustDashR.RangeLifetime = glm::vec2(0.1f, 0.5f);
	DustDashR.RangeVelocity = glm::vec2(1.0f, 5.0f);
	DustDashR.RangeZ = glm::vec2(-2.1f, -2.1f);
	DustDashR.InitialXRange = glm::vec2(3.0f, 5.0f);
	DustDashR.InitialYRange = glm::vec2(0.5f, 1.0f);
	DustDashR.HaveGravity = true;
	DustDashR.Mass = 1.0f;
	DustDashR.Gravity = -0.15f;

	if (!DustLaunch.Init("./Assets/Textures/fog.png", (unsigned int)100, (unsigned int)5))
	{
		std::cout << "Dust Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	DustLaunch.LerpAlpha = glm::vec2(0.5f, 0.0f);
	DustLaunch.LerpSize = glm::vec2(1.5f, 2.0f);
	DustLaunch.RangeLifetime = glm::vec2(0.1f, 1.5f);
	DustLaunch.RangeVelocity = glm::vec2(1.0f, 2.0f);
	DustLaunch.RangeZ = glm::vec2(1.0f, 1.0f);
	DustLaunch.InitialXRange = glm::vec2(-1.0f, 1.0f);
	DustLaunch.InitialYRange = glm::vec2(-2.0f, 2.0f);
	DustLaunch.HaveGravity = true;
	DustLaunch.Mass = 1.0f;
	DustLaunch.Gravity = 0.01f;

	if (!HitSparkR.Init("./Assets/Textures/Spark.png", (unsigned int)500, (unsigned int)30))
	{
		std::cout << "Spark Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	HitSparkR.LerpAlpha = glm::vec2(0.5f, 0.0f);
	HitSparkR.LerpSize = glm::vec2(0.2f, 0.3f);
	HitSparkR.RangeLifetime = glm::vec2(0.9f, 1.2f);
	HitSparkR.RangeVelocity = glm::vec2(10.0f, 20.0f);
	HitSparkR.RangeZ = glm::vec2(-1.0f, -1.0f);
	HitSparkR.InitialXRange = glm::vec2(-0.1f, 1.0f);
	HitSparkR.InitialYRange = glm::vec2(-0.4f, 0.9f);
	HitSparkR.HaveGravity = true;
	HitSparkR.Mass = 1.0f;
	HitSparkR.Gravity = 0.6f;
	HitSparkR.noiseOn = false;
	HitSparkR.noiseStrength = 1;

	if (!HitSparkL.Init("./Assets/Textures/Spark.png", (unsigned int)500, (unsigned int)30))
	{
		std::cout << "Spark Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	HitSparkL.LerpAlpha = glm::vec2(0.5f, 0.0f);
	HitSparkL.LerpSize = glm::vec2(0.2f, 0.3f);
	HitSparkL.RangeLifetime = glm::vec2(0.9f, 1.2f);
	HitSparkL.RangeVelocity = glm::vec2(10.0f, 20.0f);
	HitSparkL.RangeZ = glm::vec2(-1.0f, -1.0f);
	HitSparkL.InitialXRange = glm::vec2(-1.0f, 0.1f);
	HitSparkL.InitialYRange = glm::vec2(-0.4f, 0.9f);
	HitSparkL.HaveGravity = true;
	HitSparkL.Mass = 1.0f;
	HitSparkL.Gravity = 0.6f;
	HitSparkL.noiseOn = false;
	HitSparkL.noiseStrength = 1;

	if (!MeterFlame1.Init("./Assets/Textures/redFog.png", (unsigned int)500, (unsigned int)20))
	{
		std::cout << "MeterFlame Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	MeterFlame1.LerpAlpha = glm::vec2(0.1f, 0.0f);
	MeterFlame1.LerpSize = glm::vec2(1.0f, 1.5f);
	MeterFlame1.RangeLifetime = glm::vec2(0.2f, 0.6f);
	MeterFlame1.RangeVelocity = glm::vec2(1.0f, 5.0f);
	MeterFlame1.RangeZ = glm::vec2(-1.0f, -1.0f);
	MeterFlame1.InitialXRange = glm::vec2(-5.0f, 5.0f);
	MeterFlame1.InitialYRange = glm::vec2(0.0f, 1.0f);
	MeterFlame1.HaveGravity = true;
	MeterFlame1.Mass = 1.0f;
	MeterFlame1.Gravity = -0.3f;
	MeterFlame1.noiseOn = false;
	MeterFlame1.noiseStrength = 10;
	
	if (!MeterFlame2.Init("./Assets/Textures/blueFog.png", (unsigned int)500, (unsigned int)20))
	{
		std::cout << "MeterFlame Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	MeterFlame2.LerpAlpha = glm::vec2(0.1f, 0.0f);
	MeterFlame2.LerpSize = glm::vec2(1.0f, 1.5f);
	MeterFlame2.RangeLifetime = glm::vec2(0.2f, 0.6f);
	MeterFlame2.RangeVelocity = glm::vec2(1.0f, 5.0f);
	MeterFlame2.RangeZ = glm::vec2(-1.0f, -1.0f);
	MeterFlame2.InitialXRange = glm::vec2(-5.0f, 5.0f);
	MeterFlame2.InitialYRange = glm::vec2(0.0f, 1.0f);
	MeterFlame2.HaveGravity = true;
	MeterFlame2.Mass = 1.0f;
	MeterFlame2.Gravity = -0.3f;
	MeterFlame2.noiseOn = false;
	MeterFlame2.noiseStrength = 10;

	if (!KnightUltFX.Init("./Assets/Textures/RedConfetti.png", (unsigned int)500, (unsigned int)60))
	{
		std::cout << "Spark Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	KnightUltFX.LerpAlpha = glm::vec2(0.5f, 0.0f);
	KnightUltFX.LerpSize = glm::vec2(0.2f, 0.3f);
	KnightUltFX.RangeLifetime = glm::vec2(0.9f, 1.2f);
	KnightUltFX.RangeVelocity = glm::vec2(10.0f, 20.0f);
	KnightUltFX.RangeZ = glm::vec2(-1.0f, -1.0f);
	KnightUltFX.InitialXRange = glm::vec2(-1.0f, 1.0f);
	KnightUltFX.InitialYRange = glm::vec2(-0.4f, 0.9f);
	KnightUltFX.HaveGravity = true;
	KnightUltFX.Mass = 1.0f;
	KnightUltFX.Gravity = 0.6f;
	KnightUltFX.noiseOn = false;
	KnightUltFX.noiseStrength = 1;

	if (!NinjaUltFX1.Init("./Assets/Textures/fog.png", (unsigned int)500, (unsigned int)30))
	{
		std::cout << "Spark Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	NinjaUltFX1.LerpAlpha = glm::vec2(0.7f, 0.0f);
	NinjaUltFX1.LerpSize = glm::vec2(1.5f, 2.0f);
	NinjaUltFX1.RangeLifetime = glm::vec2(0.5f, 0.8f);
	NinjaUltFX1.RangeVelocity = glm::vec2(2.0f, 10.0f);
	NinjaUltFX1.RangeZ = glm::vec2(-1.0f, -1.0f);
	NinjaUltFX1.InitialXRange = glm::vec2(-1.0f, 1.0f);
	NinjaUltFX1.InitialYRange = glm::vec2(-0.4f, 0.9f);
	NinjaUltFX1.HaveGravity = true;
	NinjaUltFX1.Mass = 1.0f;
	NinjaUltFX1.Gravity = 0.05f;
	NinjaUltFX1.noiseOn = false;
	NinjaUltFX1.noiseStrength = 1;

	if (!NinjaUltFX2.Init("./Assets/Textures/fog.png", (unsigned int)500, (unsigned int)30))
	{
		std::cout << "Spark Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	NinjaUltFX2.LerpAlpha = glm::vec2(0.7f, 0.0f);
	NinjaUltFX2.LerpSize = glm::vec2(1.5f, 2.0f);
	NinjaUltFX2.RangeLifetime = glm::vec2(0.5f, 0.8f);
	NinjaUltFX2.RangeVelocity = glm::vec2(2.0f, 10.0f);
	NinjaUltFX2.RangeZ = glm::vec2(-1.0f, -1.0f);
	NinjaUltFX2.InitialXRange = glm::vec2(-1.0f, 1.0f);
	NinjaUltFX2.InitialYRange = glm::vec2(-0.4f, 0.9f);
	NinjaUltFX2.HaveGravity = true;
	NinjaUltFX2.Mass = 1.0f;
	NinjaUltFX2.Gravity = 0.05f;
	NinjaUltFX2.noiseOn = false;
	NinjaUltFX2.noiseStrength = 1;

//=======================================================================//
	//Init Scene & Frame Buffers
	if (FULLSCREEN) {
		GBuffer.InitDepthTexture(FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
		//0 is equal to 1 for the index. To make another color texture it is as easy as changing the list size in the contructor and copying the line below
		//These parameters can be changed to whatever you want
		GBuffer.InitColorTexture(0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE); //Flat color
		GBuffer.InitColorTexture(1, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_RGB16, GL_NEAREST, GL_CLAMP_TO_EDGE); //Normals (xyz)
		//Buffer explained at Week 10 time: 5:30 - 7:45
		GBuffer.InitColorTexture(2, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_RGB32F, GL_NEAREST, GL_CLAMP_TO_EDGE); //View Space Positions (xyz)
	}
	else {
		GBuffer.InitDepthTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
		//0 is equal to 1 for the index. To make another color texture it is as easy as changing the list size in the contructor and copying the line below
		//These parameters can be changed to whatever you want
		GBuffer.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE); //Flat color
		GBuffer.InitColorTexture(1, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB16, GL_NEAREST, GL_CLAMP_TO_EDGE); //Normals (xyz)
		//Buffer explained at Week 10 time: 5:30 - 7:45
		GBuffer.InitColorTexture(2, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB32F, GL_NEAREST, GL_CLAMP_TO_EDGE); //View Space Positions (xyz)
	}
	if (!GBuffer.CheckFBO())
	{
		std::cout << "GB FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (FULLSCREEN)
		DeferredComposite.InitColorTexture(0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	else
	DeferredComposite.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!DeferredComposite.CheckFBO())
	{
		std::cout << "DC FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	ShadowMap.InitDepthTexture(SHADOW_RESOLUTION, SHADOW_RESOLUTION);
	if (!ShadowMap.CheckFBO())
	{
		std::cout << "SM FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	//THis is a single channel texture explained at Week 11 time: ~3:30
	if (FULLSCREEN)
		EdgeMap.InitColorTexture(0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	else
		EdgeMap.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!EdgeMap.CheckFBO())
	{
		std::cout << "EM FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (FULLSCREEN)
		WorkBuffer1.InitColorTexture(0,FULLSCREEN_WIDTH / (unsigned int)BLOOM_DOWNSCALE, FULLSCREEN_HEIGHT / (unsigned int)BLOOM_DOWNSCALE, GL_RGB8/*GL_R11F_G11F_B10F*/, GL_LINEAR, GL_CLAMP_TO_EDGE); //These parameters can be changed to whatever you want
	else
	WorkBuffer1.InitColorTexture(0, WINDOW_WIDTH / (unsigned int)BLOOM_DOWNSCALE, WINDOW_HEIGHT / (unsigned int)BLOOM_DOWNSCALE, GL_RGB8/*GL_R11F_G11F_B10F*/, GL_LINEAR, GL_CLAMP_TO_EDGE); //These parameters can be changed to whatever you want
	if (!WorkBuffer1.CheckFBO())
	{
		std::cout << "WB1 FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (FULLSCREEN)
		WorkBuffer2.InitColorTexture(0, FULLSCREEN_WIDTH / (unsigned int)BLOOM_DOWNSCALE, FULLSCREEN_HEIGHT / (unsigned int)BLOOM_DOWNSCALE, /*GL_R11F_G11F_B10F*/GL_RGB8, GL_LINEAR, GL_CLAMP_TO_EDGE); //These parameters can be changed to whatever you want
	else
	WorkBuffer2.InitColorTexture(0, WINDOW_WIDTH / (unsigned int)BLOOM_DOWNSCALE, WINDOW_HEIGHT / (unsigned int)BLOOM_DOWNSCALE, /*GL_R11F_G11F_B10F*/GL_RGB8, GL_LINEAR, GL_CLAMP_TO_EDGE); //These parameters can be changed to whatever you want
	if (!WorkBuffer2.CheckFBO())
	{
		std::cout << "WB2 FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}
	//0 is equal to 1 for the index. To make another color texture it is as easy as changing the list size in the contructor and copying the line below
	//These parameters can be changed to whatever you want
	if (FULLSCREEN)
		HudMap.InitColorTexture(0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	else
	HudMap.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!HudMap.CheckFBO())
	{
		std::cout << "HudMap FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (FULLSCREEN)
		ssaoWorkBuffer1.InitColorTexture(0,FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	else
		ssaoWorkBuffer1.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!ssaoWorkBuffer1.CheckFBO())
	{
		std::cout << "ssaoFBO FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (FULLSCREEN)
		ssaoWorkBuffer2.InitColorTexture(0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	else
		ssaoWorkBuffer2.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!ssaoWorkBuffer2.CheckFBO())
	{
		std::cout << "ssaoBlurFBO FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	// generate sample kernel
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;
	for (unsigned int i = 0; i < 16; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator)* 0.95 + 0.05);
		sample = glm::normalize(sample);
		//sample *= randomFloats(generator);
		float scale = float(i) / 16.0f;

		// scale samples s.t. they're more aligned to center of kernel
		scale = lerp(0.25f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	// generate noise texture
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		ssaoNoise.push_back(noise);
	}
	//load it
	if (!noiseTexture.Load(ssaoNoise))
	{
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}


//================================================================//
	//Camera Init

	/*CameraTransform.Translate(vec3(0.0f, 7.5f, 20.0f));
	CameraTransform.RotateX(-15.0f);*/
	if (FULLSCREEN) {
		GameCamera.CameraProjection = Transform::PerspectiveProjection(60.0f, (float)FULLSCREEN_WIDTH / (float)FULLSCREEN_HEIGHT, 1.0f, 10000.0f);
		//ShadowProjection.OrthographicProjection(35.0f, -35.0f, 35.0f, -35.0f, -10.0f, 100.0f);
		ShadowProjection = Transform::OrthographicProjection(-35.0f, 35.0f, 35.0f, -35.0f, -25.0f, 100.0f);
		hudProjection = Transform::OrthographicProjection((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, -10.0f, 100.0f);
	}
	else {
		GameCamera.CameraProjection = Transform::PerspectiveProjection(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 10000.0f);
		//ShadowProjection.OrthographicProjection(35.0f, -35.0f, 35.0f, -35.0f, -10.0f, 100.0f);
		ShadowProjection = Transform::OrthographicProjection(-35.0f, 35.0f, 35.0f, -35.0f, -25.0f, 100.0f);
		hudProjection = Transform::OrthographicProjection((float)WINDOW_WIDTH * -0.5f, (float)WINDOW_WIDTH * 0.5f, (float)WINDOW_HEIGHT * 0.5f, (float)WINDOW_HEIGHT * -0.5f, -10.0f, 100.0f);
	}

//=================================================================//
	//Init Hitboxes for Net

	Hitbox *hurt1 = new Hitbox(glm::vec3(25.1f, 12.5f, 0.0f), 2.0f);
	Netbox.push_back(hurt1);
	Hitbox *hurt2 = new Hitbox(glm::vec3(-25.1f, 12.5f, 0.0f), 2.0f);
	Netbox.push_back(hurt2);
	score1 = 0;
	score2 = 0;
	p1Score = false;
	p2Score = false;


	loadTime();//load time gui textures and obj
	//players[0]->setPosition(glm::vec3(-5, 0, 0));
	//players[1]->setPosition(glm::vec3(5, 0, 0));

	//start timer
	updateTimer = new Timer();

//=================================================================//
	//Sound Stuff

	//Background sounds
	gameTheme.Load("./Assets/Media/GameMusic.wav", false, true);
	mumble.Load("./Assets/Media/Mumble.wav", true, true);

	//Game state sounds
	cheer.Load("./Assets/Media/Cheer.wav", true, false);
	horn.Load("./Assets/Media/Horn.wav", true, false);
	
	//Menu Sounds
	select.Load("./Assets/Media/SelectSound.wav", true, false);
	menuMove.Load("./Assets/Media/MenuMove.wav", true, false);

	//Announcing Sounds
	assassin.Load("./Assets/Media/Assassin.wav", true, false);
	gaurdian.Load("./Assets/Media/Gaurdian.wav", true, false);
	oneMin.Load("./Assets/Media/1Minute.wav", true, false);
	thirtySec.Load("./Assets/Media/30Seconds.wav", true, false);
	fight.Load("./Assets/Media/Fight.wav", true, false);

	//Player Action Sounds
	knightWalk.Load("./Assets/Media/KnightWalk.wav", true, false);
	knightWalk1.Load("./Assets/Media/KnightWalk1.wav", true, false);
	knightWalk2.Load("./Assets/Media/KnightWalk2.wav", true, false);

	ninjaWalk.Load("./Assets/Media/NinjaWalk.wav", true, false);
	ninjaWalk1.Load("./Assets/Media/NinjaWalk1.wav", true, false);
	ninjaWalk2.Load("./Assets/Media/NinjaWalk2.wav", true, false);

	knightJump.Load("./Assets/Media/KnightJump.wav", true, false);
	knightJump1.Load("./Assets/Media/KnightJump1.wav", true, false);
	knightJump2.Load("./Assets/Media/KnightJump2.wav", true, false);

	ninjaJump.Load("./Assets/Media/NinjaJump.wav", true, false);
	ninjaJump1.Load("./Assets/Media/NinjaJump1.wav", true, false);
	ninjaJump2.Load("./Assets/Media/NinjaJump2.wav", true, false);

	hit.Load("./Assets/Media/Hit.wav", true, false);
	hit1.Load("./Assets/Media/Hit1.wav", true, false);
	hit2.Load("./Assets/Media/Hit2.wav", true, false);

	dash.Load("./Assets/Media/Dash.wav", true, false);
	dash1.Load("./Assets/Media/Dash1.wav", true, false);
	dash2.Load("./Assets/Media/Dash2.wav", true, false);

	attack.Load("./Assets/Media/Attack.wav", true, false);
	attack1.Load("./Assets/Media/Attack1.wav", true, false);
	attack2.Load("./Assets/Media/Attack2.wav", true, false);

	knightUlt.Load("./Assets/Media/KnightUlt.wav", true, false);
	knightUltEnd.Load("./Assets/Media/KnightUltEnd.wav", true, false);
	ninjaUlt.Load("./Assets/Media/NinjaUlt.wav", true, false);

	defaultPos = { 0.0f, 0.0f, 0.0f };
	p1Pos = { 0.0f, 0.0f, 0.0f };
	p2Pos = { 0.0f, 0.0f, 0.0f };

	themeChannel = gameTheme.Play(defaultPos, defaultPos, true);
	themeChannel->setVolume(0.7f);
	mumbleChannel = mumble.Play(defaultPos, defaultPos, true);
	//gameSound.Load("./Assets/Media/GameMusic.wav");

	//Set Priorities, 0 being the highest priority and 256 being the lowest priority
	themeChannel->setPriority(256);
	mumbleChannel->setPriority(255);
	selectionChannel->setPriority(10);
	otherChannel->setPriority(10);
	knightChannel->setPriority(0);
	ninjaChannel->setPriority(0);

	Sound::engine.listener.pos = { GameCamera.pos.x, GameCamera.pos.y, 50.0f };

	//Create a pitch shift effect
	Sound::engine.system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &pitchShift);
	//pitchShift->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.5f);

	Sound::engine.system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &highPass);}


void Game::update()
{
	Sound::engine.update();
	//auto end = chrono::steady_clock::now();
	//auto start = chrono::steady_clock::now();
	if (scene == 5)
	{
		updateTutScreen();

		if (!soundNormalized)
		{
			mumble.Stop(mumbleChannel);
			//themeChannel->removeDSP(pitchShift);
			gameTheme.Stop(themeChannel);
			themeChannel = gameTheme.Play(defaultPos, defaultPos, true);
			themeChannel->setVolume(0.7f);


			mumbleChannel = mumble.Play(defaultPos, defaultPos, true);

			soundNormalized = true;
			soundPitched = false;
			soundHighPassed = false;
		}
	}
	else if (scene == 4)
	{
		updateEndScreen();

		if (!soundHighPassed)
		{
			mumble.Stop(mumbleChannel);
			gameTheme.Stop(themeChannel);
			themeChannel = gameTheme.Play(defaultPos, defaultPos, true);

			highPass->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, 1000.0f);
			themeChannel->addDSP(0, highPass);

			pitchShift->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.8f);
			themeChannel->addDSP(0, pitchShift);

			themeChannel->setVolume(0.8f);

			mumbleChannel = mumble.Play(defaultPos, defaultPos, true);

			soundPitched = false;
			soundNormalized = false;
			soundHighPassed = true;
		}
	}
	else if (scene == 3) {
		updateScene();

		/*if (!soundPlaying)
		{
			soundChannel->setPaused(true);

			gameSoundPos = { 0.0f, 0.0f, 0.0f };
			gameSoundChannel = gameSound.Play(true);
			soundPlaying = true;
		}

		Sound::SetPosition(gameSoundChannel, gameSoundPos);
		Sound::engine.update();*/

		//gameTheme.Stop(themeChannel);
		//themeChannel = gameTheme.Play();

		if (!soundPitched)
		{
			mumble.Stop(mumbleChannel);
			pitchShift->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.5f);
			themeChannel->addDSP(0, pitchShift);

			themeChannel->setFrequency(60000.0f);
			
			themeChannel->setVolume(0.5f);

			//mumbleChannel = mumble.Play(deaultPos, deaultPos, true);

			soundPitched = true;
			soundNormalized = false;
			soundHighPassed = false;
		}

	}
	else if (scene == 2) {
		updateSSS();
		
		if (!soundNormalized)
		{
			mumble.Stop(mumbleChannel);
			//themeChannel->removeDSP(pitchShift);
			gameTheme.Stop(themeChannel);
			themeChannel = gameTheme.Play(defaultPos, defaultPos, true);

			themeChannel->setVolume(0.7f);

			mumbleChannel = mumble.Play(defaultPos, defaultPos, true);
			
			soundNormalized = true;
			soundPitched = false;
			soundHighPassed = false;
		}
	}
	else if (scene == 1) {
		updateCSS();
		
		if (!soundNormalized)
		{
			mumble.Stop(mumbleChannel);
			//themeChannel->removeDSP(pitchShift);
			gameTheme.Stop(themeChannel);
			themeChannel = gameTheme.Play(defaultPos, defaultPos, true);

			mumbleChannel = mumble.Play(defaultPos, defaultPos, true);
			
			themeChannel->setVolume(0.7f);

			soundNormalized = true;
			soundPitched = false;
			soundHighPassed = false;
		}
	}
	else {
		updateMenu();
		
		if (!soundNormalized)
		{
			mumble.Stop(mumbleChannel);
			//themeChannel->removeDSP(pitchShift);
			gameTheme.Stop(themeChannel);
			themeChannel = gameTheme.Play(defaultPos, defaultPos, true);

			mumbleChannel = mumble.Play(defaultPos, defaultPos, true);
			
			themeChannel->setVolume(0.7f);

			soundNormalized = true;
			soundPitched = false;
			soundHighPassed = false;
		}
	}
	//end = chrono::steady_clock::now();
	//cout << chrono::duration_cast<chrono::microseconds>(end - start).count() << ":";

}

void Game::updateTutScreen()
{
	updateTimer->tick();
	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

	updateInputs();

	//press back to main menu
	if (inputs[5] || inputs2[5]) {
		lastInputTime = 0.0f;
		inputs[5] = 0;
		inputs2[5] = 0;

		stageDone = false;
		stageVal = 1;
		scene = 0;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Char = 0;
		p2Char = 0;
		p1Done = false;
		p2Done = false;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
	}
}

void Game::updateEndScreen()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();
	float deltaTime = updateTimer->getElapsedTimeSeconds();
	static float tempTime;
	tempTime += deltaTime;

	if (tempTime >= 1.5f)
		updateInputs();

	XBoxController.SetVibration(0, 0, 0);//controller 0, power 0 on left and right (off)
	XBoxController.SetVibration(1, 0, 0);//controller , power 0 on left and right (off)

	//Reset confetti efects so they are not on screen when game starts up again
	ConfettiEffectBlueLeft.Reset();
	ConfettiEffectBlueRight.Reset();
	ConfettiEffectOrangeLeft.Reset();
	ConfettiEffectOrangeRight.Reset();
	ConfettiEffectPurpleLeft.Reset();
	ConfettiEffectPurpleRight.Reset();
	ConfettiEffectRedLeft.Reset();
	ConfettiEffectRedRight.Reset();
	NinjaPetals.Reset();
	NinjaPetals.Reset();

	//press
	if (inputs[6] || inputs2[6] || inputs[5] || inputs2[5]) {
		//Play Select Sound
		selectionChannel = select.Play(defaultPos, defaultPos, false);
		
		lastInputTime = 0.0f;
		inputs[6] = 0;
		inputs2[6] = 0;
		endGame = true;
	}

	//Once A is pressed go back to menu
	if (endGame)
	{
		scene = 0;
		p1Char = 0;
		p2Char = 0;
		p1KnightWin = false;
		p1NinjaWin = false;
		p2KnightWin = false;
		p2NinjaWin = false;
		tieGame = false;
		endGame = false;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
		TotalGameTime = 0.0f;
		deltaTime = 0;
		tempTime = 0;
		updateTimer = new Timer();
	}
}

void Game::updateMenu()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();
	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

	updateInputs();

	//check if move input
	unsigned int oldButton = selectedButton;
	if (TotalGameTime - lastInputTime > 0.2f) {
		if (inputs[0] || inputs2[0]) {
			selectedButton--;
			//Play menu option move sound
			selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
		}
		else if (inputs[2] || inputs2[2]) {
			selectedButton++;
			//Play menu option move sound
			selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
		}
	}

	//correction
	if (selectedButton < 1)
		selectedButton = 1;
	else if (selectedButton > 3)
			selectedButton = 3;

	//change button sizes
	if (selectedButton != oldButton) {
		lastInputTime = TotalGameTime;
		if (selectedButton == 1) {
			if (FULLSCREEN) {
				findObjects(0, "button1")->setScale(1.1f * 300.0f);
				findObjects(0, "button2")->setScale((1.0f / 1.1f) * 300.0f);
				findObjects(0, "button3")->setScale((1.0f / 1.1f) * 300.0f);
			}
			else {
				findObjects(0, "button1")->setScale(1.1f * 200.0f);
				findObjects(0, "button2")->setScale((1.0f / 1.1f) * 200.0f);
				findObjects(0, "button3")->setScale((1.0f / 1.1f) * 200.0f);
			}
		}
		else if (selectedButton == 2) {
			if (FULLSCREEN) {
				findObjects(0, "button2")->setScale(1.1f * 300.0f);
				findObjects(0, "button1")->setScale((1.0f / 1.1f) * 300.0f);
				findObjects(0, "button3")->setScale((1.0f / 1.1f) * 300.0f);
			}
			else {
				findObjects(0, "button2")->setScale(1.1f * 200.0f);
				findObjects(0, "button1")->setScale((1.0f / 1.1f) * 200.0f);
				findObjects(0, "button3")->setScale((1.0f / 1.1f) * 200.0f);
			}
		}
		else if (selectedButton == 3) {
			if (FULLSCREEN) {
				findObjects(0, "button3")->setScale(1.1f * 300.0f);
				findObjects(0, "button1")->setScale((1.0f / 1.1f) * 300.0f);
				findObjects(0, "button2")->setScale((1.0f / 1.1f) * 300.0f);
			}
			else {
				findObjects(0, "button3")->setScale(1.1f * 200.0f);
				findObjects(0, "button1")->setScale((1.0f / 1.1f) * 200.0f);
				findObjects(0, "button2")->setScale((1.0f / 1.1f) * 200.0f);
			}
		}
	}

	//press
	if (inputs[6] || inputs2[6]) {

		//Play Select Sound
		selectionChannel = select.Play(defaultPos, defaultPos, false);

		if (selectedButton == 1) {
			scene = 1;
			lastInputTime = 0.0f;
			inputs[6] = 0;
			inputs2[6] = 0;
		}
		else if (selectedButton == 2)
		{
			scene = 5;
			lastInputTime = 0.0f;
			inputs[6] = 0;
			inputs2[6] = 0;
		}
		else if (selectedButton == 3) {
			exit(0);
		}
	}

}

void Game::updateCSS()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();
	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

	updateInputs();

	//Go back to main menu
	if (inputs[5] || inputs2[5]) {
		lastInputTime = 0.0f;
		inputs[5] = 0;
		inputs2[5] = 0;

		stageDone = false;
		stageVal = 1;
		scene = 0;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Char = 0;
		p2Char = 0;
		p1Done = false;
		p2Done = false;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
	}

	//check inputs
	if (TotalGameTime - lastInputTime > 0.2f) { //This delay needs to be tested
		if (!p1Done) {
			if (inputs[8])
			{
				lastInputTime = TotalGameTime;
				p1Char--;
				//Play menu option move sound
				selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
			}
			else if (inputs[7])
			{
				lastInputTime = TotalGameTime;
				p1Char++;
				//Play menu option move sound
				selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
			}
		}

		if (!p2Done) {
			if (inputs2[8])
			{
				lastInputTime = TotalGameTime;
				p2Char--;
				//Play menu option move sound
				selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
			}
			else if (inputs2[7])
			{
				lastInputTime = TotalGameTime;
				p2Char++;
				//Play menu option move sound
				selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
			}
		}
	}

	//correction
	if (p1Char < 1)
		p1Char = 1;
	else if (p1Char > 2)
		p1Char = 2;
	if (p2Char < 1)
		p2Char = 1;
	else if (p2Char > 2)
		p2Char = 2;

	//move curser
	if (p1Char == 1) {
		findObjects(1, "p1Select")->setPosition(
			findObjects(1, "knightIcon")->getPosition());

		findObjects(1, "p1KnightPic")->hide = false;
		findObjects(1, "p1NinjaPic")->hide = true;
	}
	else if (p1Char == 2) {
		findObjects(1, "p1Select")->setPosition(
			findObjects(1, "ninjaIcon")->getPosition());

		findObjects(1, "p1KnightPic")->hide = true;
		findObjects(1, "p1NinjaPic")->hide = false;
	}
	if (p2Char == 1) {
		findObjects(1, "p2Select")->setPosition(
			findObjects(1, "knightIcon")->getPosition());

		findObjects(1, "p2KnightPic")->hide = false;
		findObjects(1, "p2NinjaPic")->hide = true;
	}
	else if (p2Char == 2) {
		findObjects(1, "p2Select")->setPosition(
			findObjects(1, "ninjaIcon")->getPosition());

		findObjects(1, "p2KnightPic")->hide = true;
		findObjects(1, "p2NinjaPic")->hide = false;
	}


	//press
	if (!p1Done) {
		if (inputs[6]) {
			//Play Select Sound
			selectionChannel = select.Play(defaultPos, defaultPos, false);

			if (p1Char == 1) {
				p1Done = true;
				//Announce gaurdian
				selectionChannel = gaurdian.Play(defaultPos, defaultPos, false);
				selectionChannel->setVolume(3.0f);
			}
			else if (p1Char == 2) {
				p1Done = true;
				//Announce assassin
				selectionChannel = assassin.Play(defaultPos, defaultPos, false);
				selectionChannel->setVolume(1.0f);
			}
		}
	}
	if (!p2Done) {
		if (inputs2[6]) {
			//Play Select Sound
			selectionChannel = select.Play(defaultPos, defaultPos, false);

			if (p2Char == 1) {
				p2Done = true;
				//Announce gaurdian
				selectionChannel = gaurdian.Play(defaultPos, defaultPos, false);
				selectionChannel->setVolume(3.0f);
			}
			else if (p2Char == 2) {
				p2Done = true;
				//Announce assassin
				selectionChannel = assassin.Play(defaultPos, defaultPos, false);
				selectionChannel->setVolume(1.0f);
			}
		}
	}

	//Both Done
	if (p1Done && p2Done) {

		/*if (p1Char == 1) {
			players[0] = new Knight(knightTemp);
			players[0]->texture.Load("./Assets/Textures/player1.png");
		}
		else if (p1Char == 2) {
			players[0] = new Ninja(ninjaTemp);
			players[0]->texture.Load("./Assets/Textures/player1ninja.png");
		}

		if (p2Char == 1) {
			players[1] = new Knight(knightTemp);
			players[1]->texture.Load("./Assets/Textures/player2.png");
		}
		else if (p2Char == 2) {
			players[1] = new Ninja(ninjaTemp);
			players[1]->texture.Load("./Assets/Textures/player2ninja.png");
		}*/
		selectionChannel->setVolume(1.0f);
		scene = 2;
		lastInputTime = 0.0f;
		TotalGameTime = 0.0f;
		deltaTime = 0;
		updateTimer = new Timer();
	}
}

void Game::updateSSS()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();
	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

	updateInputs();

	//Go back to main menu
	if (inputs[5] || inputs2[5]) {
		lastInputTime = 0.0f;
		inputs[5] = 0;
		inputs2[5] = 0;

		stageDone = false;
		stageVal = 1;
		scene = 0;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Char = 0;
		p2Char = 0;
		p1Done = false;
		p2Done = false;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
	}

	//check if move input
	if (TotalGameTime - lastInputTime > 0.2f && !stageDone) {
		if (inputs[7] || inputs2[7]) {
			stageVal++;
			lastInputTime = TotalGameTime;
			//Play menu option move sound
			selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
		}
		else if (inputs[8] || inputs2[8]) {
			stageVal--;
			lastInputTime = TotalGameTime;
			//Play menu option move sound
			selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
		}
	}

	//correction
	if (stageVal < 1)
		stageVal = 1;
	else if (stageVal > 3)
		stageVal = 3;

	//move curser
	if (stageVal == 1) {
		findObjects(2, "select")->setPosition(
			findObjects(2, "courtIconDefault")->getPosition());

		findObjects(2, "courtPicDefault")->hide = false;
		findObjects(2, "courtPicKnight")->hide = true;
		findObjects(2, "courtPicNinja")->hide = true;
	}
	else if (stageVal == 2) {
		findObjects(2, "select")->setPosition(
			findObjects(2, "courtIconKnight")->getPosition());

		findObjects(2, "courtPicDefault")->hide = true;
		findObjects(2, "courtPicKnight")->hide = false;
		findObjects(2, "courtPicNinja")->hide = true;
	}
	else if (stageVal == 3) {
		findObjects(2, "select")->setPosition(
			findObjects(2, "courtIconNinja")->getPosition());

		findObjects(2, "courtPicDefault")->hide = true;
		findObjects(2, "courtPicKnight")->hide = true;
		findObjects(2, "courtPicNinja")->hide = false;
	}


	//press
	if ((inputs[6] || inputs2[6]) && !stageDone) {
		//Play Select Sound
		selectionChannel = select.Play(defaultPos, defaultPos, false);
		selectionChannel = fight.Play(defaultPos, defaultPos, false);
		
		lastInputTime = 0.0f;
		inputs[6] = 0;
		inputs2[6] = 0;
		stageDone = true;
	}

	//Both Done
	if (stageDone) {

		//load default court
		if (stageVal == 1) {

			//hide ninja court
			findObjects(3, "ninja_court")->hide = true;
			for (int i = 0; i < (int)ninja_court_objs.size(); i++)
				findObjects(3, ninja_court_objs[i])->hide = true;
			//hide knight court
			findObjects(3, "knight_court")->hide = true;
			for (int i = 0; i < (int)knight_court_objs.size(); i++)
				findObjects(3, knight_court_objs[i])->hide = true;
			//show default court
			findObjects(3, "default_court")->hide = false;
			for (int i = 0; i < (int)default_court_objs.size(); i++)
				findObjects(3, default_court_objs[i])->hide = false;

		}
		//load knight court
		else if (stageVal == 2) {

			//hide ninja court
			findObjects(3, "ninja_court")->hide = true;
			for (int i = 0; i < (int)ninja_court_objs.size(); i++)
				findObjects(3, ninja_court_objs[i])->hide = true;
			//hide default court
			findObjects(3, "default_court")->hide = true;
			for (int i = 0; i < (int)default_court_objs.size(); i++)
				findObjects(3, default_court_objs[i])->hide = true;
			//show knight court
			findObjects(3, "knight_court")->hide = false;
			for (int i = 0; i < (int)knight_court_objs.size(); i++)
				findObjects(3, knight_court_objs[i])->hide = false;

		}
		//load ninja court
		else if (stageVal == 3) {

			//hide default court
			findObjects(3, "default_court")->hide = true;
			for (int i = 0; i < (int)default_court_objs.size(); i++)
				findObjects(3, default_court_objs[i])->hide = true;
			//hide knight court
			findObjects(3, "knight_court")->hide = true;
			for (int i = 0; i < (int)knight_court_objs.size(); i++)
				findObjects(3, knight_court_objs[i])->hide = true;
			//show knight court
			findObjects(3, "ninja_court")->hide = false;
			for (int i = 0; i < (int)ninja_court_objs.size(); i++)
				findObjects(3, ninja_court_objs[i])->hide = false;

		}

		//Player 1 knight
		if (p1Char == 1) {
			players[0] = new Knight(knightTemp);
			players[0]->bodyTexture.Load("./Assets/Textures/player1.png");

			if (!P1Hud.Load("./Assets/Textures/PlayerOneHud.png"))
			{
				std::cout << "BKG Texture failed to load.\n";
				system("pause");
				exit(0);
			}
		}
		//Player 1 ninja
		else if (p1Char == 2) {
			players[0] = new Ninja(ninjaTemp);
			players[0]->bodyTexture.Load("./Assets/Textures/player1ninja.png");

			isNinja1 = true;

			if (!P1Hud.Load("./Assets/Textures/PlayerOneHudNinja.png"))
			{
				std::cout << "BKG Texture failed to load.\n";
				system("pause");
				exit(0);
			}
			if (!P1Line.Load("./Assets/Textures/Line1.png"))
			{
				std::cout << "Line Texture failed to load.\n";
				system("pause");
				exit(0);
			}
		}

		//Player 2 knight
		if (p2Char == 1) {
			players[1] = new Knight(knightTemp);
			players[1]->bodyTexture.Load("./Assets/Textures/player2.png");

			if (!P2Hud.Load("./Assets/Textures/PlayerTwoHud.png"))
			{
				std::cout << "BKG Texture failed to load.\n";
				system("pause");
				exit(0);
			}
		}
		//Player 2 ninja
		else if (p2Char == 2) {
			players[1] = new Ninja(ninjaTemp);
			players[1]->bodyTexture.Load("./Assets/Textures/player2ninja.png");

			isNinja2 = true;

			if (!P2Hud.Load("./Assets/Textures/PlayerTwoHudNinja.png"))
			{
				std::cout << "BKG Texture failed to load.\n";
				system("pause");
				exit(0);
			}
			if (!P2Line.Load("./Assets/Textures/Line2.png"))
			{
				std::cout << "Line Texture failed to load.\n";
				system("pause");
				exit(0);
			}
		}

		scene = 3;
		gameDone = false;
		score1 = 0;
		score2 = 0;
		players[0]->respawn();
		players[1]->respawn();
		players[0]->setPosition(glm::vec3(-5, 0, 0));
		players[1]->setPosition(glm::vec3(5, 0, 0));
		players[1]->facingRight = false;
		//updateTimer = new Timer();
		TotalGameTime = 0.0f;
		deltaTime = 0;
		updateTimer = new Timer();
	}
}

void Game::updateScene()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();

	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;
	//character collisions
		///Allow Player to pass through one another, but will disallow them to stand in the same spot, will slowly push eachother awayy
	float diffx = (players[0]->getPosition().x) - (players[1]->getPosition().x);//difference between characters x
	float diffy = (players[0]->getPosition().y) - (players[1]->getPosition().y);//difference between characters y
	if (abs(diffx) < 0.6f && abs(diffx) != 0 && abs(diffy) < 1.5f) {//if they are touching
		//push them off
		players[0]->setPosition(players[0]->getPosition() + glm::vec3(((diffx / abs(diffx))*0.01f), 0, 0));
		players[1]->setPosition(players[1]->getPosition() + glm::vec3(((diffx / abs(diffx))*-0.01f), 0, 0));
	}

	//rumble fr first 5 frames of either player being hit;
	for (int i = 0; i < 2; i++) {
		if (players[i]->isHit() && players[i]->currentFrame < 5) {
			XBoxController.SetVibration(0, 10, 10);//controller 0, power 10 on left and right
			XBoxController.SetVibration(1, 10, 10);//controller 1, power 10 on left and right
			GameCamera.rumble = true;
			players[i]->activeTexture = &(players[i]->hurtTexture);
		}
		else if (players[i]->isHit() && players[i]->currentFrame == 6) {
			XBoxController.SetVibration(0, 0, 0);//controller 0, power 0 on left and right (off)
			XBoxController.SetVibration(1, 0, 0);//controller , power 0 on left and right (off)
			GameCamera.rumble = false;
			players[i]->activeTexture = &(players[i]->bodyTexture);
		}
	}

	//new hitbox collisions
	for (unsigned int i = 0; i < players[0]->getHitboxes().size(); i++) {
		for (unsigned int j = 0; j < players[1]->getHurtboxes().size(); j++) {

			glm::vec3 diff = players[0]->getHitboxes()[i]->getPosition() - players[1]->getHurtboxes()[j]->getPosition();
			float size = (players[0]->getHitboxes()[i]->getSize() + players[1]->getHurtboxes()[j]->getSize()) *0.5f;
			if (/*diff.Length()*/ glm::length(diff) < size) {

				XBoxController.SetVibration(0, 10, 10);
				XBoxController.SetVibration(1, 10, 10);
				Sleep(40);
				XBoxController.SetVibration(0, 0, 0);
				XBoxController.SetVibration(1, 0, 0);

				//if (players[1]->ultMode) {//add only in front condition
				//	//players[1]->blockSuccessful = true;
				//	players[0]->getHitboxes()[i]->setDone();
				//	i = 100;
				//	j = 100;
				//}
			//	else {
					players[1]->hit(players[0]->getHitboxes()[i]);

					if (players[0]->ultMode == false)
					{
						players[0]->comboAdd();
						playHit1 = true;
					}
					players[0]->getHitboxes()[i]->setDone();

					if (players[0]->type == 1 && players[0]->ultMode == true)
					{
						players[1]->hitForce *= 2.5;
						playHit1 = true;
					}

					i = 100;
					j = 100;
				//}
			}
		}
	}
	for (unsigned int i = 0; i < players[1]->getHitboxes().size(); i++) {
		for (unsigned int j = 0; j < players[0]->getHurtboxes().size(); j++) {

			glm::vec3 diff = players[1]->getHitboxes()[i]->getPosition() - players[0]->getHurtboxes()[j]->getPosition();
			float size = (players[1]->getHitboxes()[i]->getSize() + players[0]->getHurtboxes()[j]->getSize()) *0.5f;
			if (/*diff.Length()*/ glm::length(diff) < size) {

				XBoxController.SetVibration(0, 10, 10);
				XBoxController.SetVibration(1, 10, 10);
				Sleep(40);
				XBoxController.SetVibration(0, 0, 0);
				XBoxController.SetVibration(1, 0, 0);

				//if (players[0]->ultMode) {//add only in front condition
				//	//players[0]->blockSuccessful = true;
				//	players[1]->getHitboxes()[i]->setDone();
				//	i = 100;
				//	j = 100;
				//}
				//else {
					players[0]->hit(players[1]->getHitboxes()[i]);
					if (players[1]->type == 1 && players[1]->ultMode == true)
					{
						players[0]->hitForce *= 2.5;
						playHit2 = true;
					}
					players[1]->getHitboxes()[i]->setDone();

					players[0]->hit(players[1]->getHitboxes()[i]);
					if (players[1]->ultMode == false)
					{
						players[1]->comboAdd();
						playHit2 = true;
					}

					i = 100;
					j = 100;
				//}
			}

		}
	}

	//Check Hurtboxes
	for (unsigned int i = 0; i < Netbox.size(); i++) {
		Netbox[i]->update((int)deltaTime, glm::vec3());
	}

	updateInputs();

	//Go back to main menu on pressing start button
	if (inputs[5] || inputs2[5]) {
		lastInputTime = 0.0f;
		inputs[5] = 0;
		inputs2[5] = 0;

		stageDone = false;
		stageVal = 1;
		scene = 0;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Char = 0;
		p2Char = 0;
		p1Done = false;
		p2Done = false;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
	}

	//make camera rumble when ult is activated
	if (players[0]->ultFrame1 || players[1]->ultFrame1) {
		players[0]->ultFrame1 = false;
		players[1]->ultFrame1 = false;
		ultRumbleTimer = 0;
	}
	else if (ultRumbleTimer < 5) {
		GameCamera.rumble = true;
		ultRumbleTimer++;
	}
	else if (ultRumbleTimer == 5) {
		GameCamera.rumble = false;
		ultRumbleTimer = 6;
	}
	
	players[0]->update((int)deltaTime, inputs);
	players[1]->update((int)deltaTime, inputs2);
	
	//knightUlt
	///Make the sound effect happen for player 1
	if (players[0]->type == 1 && players[0]->ultMode)
	{
		//Play sound effect
		if (!knightUltPlaying1)
		{
			knightChannel = knightUlt.Play(p1Pos, defaultPos, false);
			knightUltPlaying1 = true;
		}
	}
	else if (players[0]->type == 1 && players[0]->ultMode == false)
	{
		if (knightUltPlaying1)
		{
			knightChannel = knightUltEnd.Play(p1Pos, defaultPos, false);
			knightUltPlaying1 = false;
		}
	}
	///Make the sound effect happen for player 1
	if (players[1]->type == 1 && players[1]->ultMode)
	{
		//Play sound effect
		if (!knightUltPlaying2)
		{
			knightChannel = knightUlt.Play(p2Pos, defaultPos, false);
			knightUltPlaying2 = true;
		}
	}
	else if (players[1]->type == 1 && players[1]->ultMode == false)
	{
		if (knightUltPlaying2)
		{
			knightChannel = knightUltEnd.Play(p2Pos, defaultPos, false);
			knightUltPlaying2 = false;
		}
	}

	//nijnaUlt
	///before, check if u can even ult rn
	bool okToUlt = true;
	for (int j = 0; j < 2; j++) {
		if (players[j]->action == players[j]->ACTION_IN_NET || players[j]->action == players[j]->ACTION_RESPAWN) {
			okToUlt = false;
		}
	}
	//actual ult
	for (int i = 0; i < 2; i++) {
		//if cant ult, add back metre
		if (okToUlt == false && players[i]->type == 2 && players[i]->ultMode) {
			players[i]->ultMode = false;
			players[i]->setMeter(players[i]->getMeter() + 100);
		}
		//actual ult here
		else if (players[i]->type == 2 && players[i]->ultMode) {

			//Play sound effect
			if (!ninjaUltPlaying)
			{
				ninjaChannel->setVolume(2.0f);
				ninjaChannel = ninjaUlt.Play(defaultPos, defaultPos, false);
				ninjaChannel->setVolume(1.0f);
				ninjaUltPlaying = true;
			}

			//swap positions
			glm::vec3 temp = players[i]->getPosition();
			players[i]->setPosition(players[(i + 1) % 2]->getPosition());
			players[(i + 1) % 2]->setPosition(temp);
			//swap velocity
			//temp = players[i]->getVelocity();
			//players[i]->setVelocity(players[(i + 1) % 2]->getVelocity());
			//players[(i + 1) % 2]->setVelocity(temp);
			//swap acceleration
			temp = players[i]->getAccel();
			players[i]->setAccel(players[(i + 1) % 2]->getAccel());
			players[(i + 1) % 2]->setAccel(temp);

			//swap action
			int tempA = players[i]->action;
			players[i]->action = players[(i + 1) % 2]->action;
			players[(i + 1) % 2]->action = tempA;
			//swap frameLength
			int tempC = players[i]->activeFrames;
			players[i]->activeFrames = players[(i + 1) % 2]->activeFrames;
			players[(i + 1) % 2]->activeFrames = tempC;
			//swap current frame
			players[i]->currentFrame = players[i]->activeFrames - 2;
			players[(i + 1) % 2]->currentFrame = players[(i + 1) % 2]->activeFrames - 2;

			//turn ult off
			players[i]->partiQueue.push(ULTFX);						//$$$
			players[i]->ultMode = false;
			players[i]->ultFrame1 = true;

			//correct hit
			if (players[i]->action == players[i]->ACTION_HIT)
				players[i]->action = players[i]->ACTION_FALL;
			if (players[(i + 1) % 2]->action == players[(i + 1) % 2]->ACTION_HIT)
				players[(i + 1) % 2]->action = players[(i + 1) % 2]->ACTION_FALL;

			//Set bool to false so the sound effect can play again later
			ninjaUltPlaying = false;
		}
	}
	
	for (int i = 0; i < 2; i++) {
		//check 2 per loop
		///only checking 1 per frame caused issues with spawn timing (if statement)
		///checking everything in queue every frame caused issues with random frame drops (while loop)

		//particle listener
		if (players[0]->partiQueue.size() > 0) {
			//cout << "Spawned";
			switch (players[0]->partiQueue.front())
			{
			case LANDDUST:
				DustLand.RangeX = glm::vec2(players[0]->getPosition().x + 0.5f, players[0]->getPosition().x - 0.5f);
				DustLand.RangeY = glm::vec2(players[0]->getPosition().y - 1.4f, players[0]->getPosition().y - 1.6f);
				DustLand.Spawn(0.05f);
				break;
			case LDASHDUST:
				DustDashL.RangeX = glm::vec2(players[0]->getPosition().x, players[0]->getPosition().x - 1.0f);
				DustDashL.RangeY = glm::vec2(players[0]->getPosition().y - 1.4f, players[0]->getPosition().y - 1.6f);
				DustDashL.Spawn(0.05f);
				break;
			case RDASHDUST:
				DustDashR.RangeX = glm::vec2(players[0]->getPosition().x + 1.0f, players[0]->getPosition().x);
				DustDashR.RangeY = glm::vec2(players[0]->getPosition().y - 1.4f, players[0]->getPosition().y - 1.6f);
				DustDashR.Spawn(0.05f);
				break;
			case LAUNCHDUST:
				DustLaunch.RangeX = glm::vec2(players[0]->getPosition().x + 0.7f, players[0]->getPosition().x - 0.7f);
				DustLaunch.RangeY = glm::vec2(players[0]->getPosition().y + 1.5f, players[0]->getPosition().y + 3.0f);
				DustLaunch.Spawn(0.05f);
				break;
			case HITSPARKL:
				HitSparkL.RangeX = glm::vec2(players[0]->getPosition().x, players[0]->getPosition().x);
				HitSparkL.RangeY = glm::vec2(players[0]->getPosition().y + 1.5f, players[0]->getPosition().y + 1.5f);
				HitSparkL.Spawn(0.05f);
				break;
			case HITSPARKR:
				HitSparkR.RangeX = glm::vec2(players[0]->getPosition().x, players[0]->getPosition().x);
				HitSparkR.RangeY = glm::vec2(players[0]->getPosition().y + 1.5f, players[0]->getPosition().y + 1.5f);
				HitSparkR.Spawn(0.05f);
				break;
			case METERFLAME:
				MeterFlame1.RangeX = glm::vec2(players[0]->getPosition().x + 0.7f, players[0]->getPosition().x - 1.7f);
				MeterFlame1.RangeY = glm::vec2(players[0]->getPosition().y - 0.7f, players[0]->getPosition().y + 4.0f);
				MeterFlame1.Spawn(0.001f);
				break;
			case ULTFX:
				if (players[0]->type == 1) {
					KnightUltFX.RangeX = glm::vec2(players[0]->getPosition().x, players[0]->getPosition().x);
					KnightUltFX.RangeY = glm::vec2(players[0]->getPosition().y + 1.5f, players[0]->getPosition().y + 1.5f);
					KnightUltFX.Spawn(0.05f);
				}
				if (players[0]->type == 2) {
					NinjaUltFX1.RangeX = glm::vec2(players[0]->getPosition().x, players[0]->getPosition().x);
					NinjaUltFX1.RangeY = glm::vec2(players[0]->getPosition().y + 1.5f, players[0]->getPosition().y + 1.5f);
					NinjaUltFX1.Spawn(0.05f);
					NinjaUltFX2.RangeX = glm::vec2(players[1]->getPosition().x, players[1]->getPosition().x);
					NinjaUltFX2.RangeY = glm::vec2(players[1]->getPosition().y + 1.5f, players[1]->getPosition().y + 1.5f);
					NinjaUltFX2.Spawn(0.05f);
				}
				break;
			default:
				break;
			}

			players[0]->partiQueue.pop();
		}
		if (players[1]->partiQueue.size() > 0) {
			//cout << "Spawned";
			switch (players[1]->partiQueue.front())
			{
			case LANDDUST:
				DustLand.RangeX = glm::vec2(players[1]->getPosition().x + 0.5f, players[1]->getPosition().x - 0.5f);
				DustLand.RangeY = glm::vec2(players[1]->getPosition().y - 1.4f, players[1]->getPosition().y - 1.6f);
				DustLand.Spawn(0.05f);
				break;
			case LDASHDUST:
				DustDashL.RangeX = glm::vec2(players[1]->getPosition().x + 1.0f, players[1]->getPosition().x - 1.0f);
				DustDashL.RangeY = glm::vec2(players[1]->getPosition().y - 1.4f, players[1]->getPosition().y - 1.6f);
				DustDashL.Spawn(0.05f);
				break;
			case RDASHDUST:
				DustDashR.RangeX = glm::vec2(players[1]->getPosition().x + 1.0f, players[1]->getPosition().x - 1.0f);
				DustDashR.RangeY = glm::vec2(players[1]->getPosition().y - 1.4f, players[1]->getPosition().y - 1.6f);
				DustDashR.Spawn(0.05f);
				break;
			case LAUNCHDUST:
				DustLaunch.RangeX = glm::vec2(players[1]->getPosition().x + 0.7f, players[1]->getPosition().x - 0.7f);
				DustLaunch.RangeY = glm::vec2(players[1]->getPosition().y + 1.5f, players[1]->getPosition().y + 3.0f);
				DustLaunch.Spawn(0.05f);
				break;
			case HITSPARKL:
				HitSparkL.RangeX = glm::vec2(players[1]->getPosition().x, players[1]->getPosition().x);
				HitSparkL.RangeY = glm::vec2(players[1]->getPosition().y + 1.5f, players[1]->getPosition().y + 1.5f);
				HitSparkL.Spawn(0.05f);
				break;
			case HITSPARKR:
				HitSparkR.RangeX = glm::vec2(players[1]->getPosition().x, players[1]->getPosition().x);
				HitSparkR.RangeY = glm::vec2(players[1]->getPosition().y + 1.5f, players[1]->getPosition().y + 1.5f);
				HitSparkR.Spawn(0.05f);
				break;
			case METERFLAME:
				MeterFlame2.RangeX = glm::vec2(players[1]->getPosition().x + 0.7f, players[1]->getPosition().x - 1.7f);
				MeterFlame2.RangeY = glm::vec2(players[1]->getPosition().y - 0.7f, players[1]->getPosition().y + 4.0f);
				MeterFlame2.Spawn(0.001f);
				break;
			case ULTFX:
				if (players[1]->type == 1) {
					KnightUltFX.RangeX = glm::vec2(players[1]->getPosition().x, players[1]->getPosition().x);
					KnightUltFX.RangeY = glm::vec2(players[1]->getPosition().y + 1.5f, players[1]->getPosition().y + 1.5f);
					KnightUltFX.Spawn(0.05f);
				}
				if (players[1]->type == 2) {
					NinjaUltFX2.RangeX = glm::vec2(players[1]->getPosition().x, players[1]->getPosition().x);
					NinjaUltFX2.RangeY = glm::vec2(players[1]->getPosition().y + 1.5f, players[1]->getPosition().y + 1.5f);
					NinjaUltFX2.Spawn(0.05f);
					NinjaUltFX1.RangeX = glm::vec2(players[0]->getPosition().x, players[0]->getPosition().x);
					NinjaUltFX1.RangeY = glm::vec2(players[0]->getPosition().y + 1.5f, players[0]->getPosition().y + 1.5f);
					NinjaUltFX1.Spawn(0.05f);
				}
				break;
			default:
				break;
			}

			players[1]->partiQueue.pop();

		}
	}

	//new score code
	for (unsigned int i = 0; i < Netbox.size(); i++) {
		for (unsigned int j = 0; j < players[1]->getHurtboxes().size(); j++) {
			glm::vec3 diff = Netbox[i]->getPosition() - players[1]->getHurtboxes()[j]->getPosition();
			float size = (Netbox[i]->getSize() + players[1]->getHurtboxes()[j]->getSize()) *0.5f;
			if (/*diff.Length()*/ glm::length(diff) < size && players[1]->action != players[1]->ACTION_IN_NET) {
				//players[1]->respawn();
				players[1]->action = players[1]->ACTION_IN_NET;
				players[1]->activeFrames = 51;
				std::cout << std::endl << "Player 1 Scored" << std::endl;
				score1++;
				//i = 100;
				ConfettiEffectRedRight.Spawn(1.0f);
				ConfettiEffectRedLeft.Spawn(1.0f);
				ConfettiEffectOrangeRight.Spawn(1.0f);
				ConfettiEffectOrangeLeft.Spawn(1.0f);
				p1Score = true;
				j = 100;
				GameCamera.reset();

			}
		}
		for (unsigned int j = 0; j < players[0]->getHurtboxes().size(); j++) {
			glm::vec3 diff = Netbox[i]->getPosition() - players[0]->getHurtboxes()[j]->getPosition();
			float size = (Netbox[i]->getSize() + players[0]->getHurtboxes()[j]->getSize()) *0.5f;
			if (/*diff.Length()*/ glm::length(diff) < size && players[0]->action != players[0]->ACTION_IN_NET) {
				//players[0]->respawn();
				players[0]->action = players[0]->ACTION_IN_NET;
				players[0]->activeFrames = 51;
				std::cout << std::endl << "Player 2 Scored" << std::endl;
				score2++;
				//i = 100;
				ConfettiEffectBlueRight.Spawn(1.0f);
				ConfettiEffectBlueLeft.Spawn(1.0f);
				ConfettiEffectPurpleRight.Spawn(1.0f);
				ConfettiEffectPurpleLeft.Spawn(1.0f);
				p2Score = true;
				j = 100;
				GameCamera.reset();
			}
		}
	}

	//rumble while in net
	for (int i = 0; i < 2; i++) {
		if (players[i]->action == players[i]->ACTION_IN_NET) {
			XBoxController.SetVibration(i, 5, 5);
			XBoxController.SetVibration((i + 1) % 2, 0, 0);
		}
		else if (players[i]->action == players[i]->ACTION_RESPAWN) {
			XBoxController.SetVibration(i, 0, 0);
			XBoxController.SetVibration((i + 1) % 2, 0, 0);
		}
	}

	//DYNAMIC CAM
	//camera control using seek point and target zoom
	seekPoint.x = (players[1]->getPosition().x + players[0]->getPosition().x) * 0.5f;//seek point is inbetween the 2 players
	seekPoint.y = (players[1]->getPosition().y + players[0]->getPosition().y) * 0.5f;//seek point is inbetween the 2 players

	glm::vec2 p1;
	glm::vec2 p2;
	if (players[0]->action == players[0]->ACTION_IN_NET && players[1]->action == players[1]->ACTION_IN_NET) {
		seekPoint = glm::vec3(0, 3, 0);
		GameCamera.targetZoom = 20;
	}
	else if (players[0]->action == players[0]->ACTION_IN_NET && players[1]->action != players[1]->ACTION_IN_NET) {
		seekPoint = glm::vec3((players[0]->getPosition().x / abs(players[0]->getPosition().x) * 10), 2, 0);
		GameCamera.targetZoom = 20;
	}
	else if (players[0]->action != players[0]->ACTION_IN_NET && players[1]->action == players[1]->ACTION_IN_NET) {
		seekPoint = glm::vec3((players[1]->getPosition().x / abs(players[1]->getPosition().x) * 10), 2, 0);
		GameCamera.targetZoom = 20;
	}
	else if (players[0]->action == players[0]->ACTION_RESPAWN || players[1]->action == players[1]->ACTION_RESPAWN) {
			seekPoint = glm::vec3(0, 1, 0);
			GameCamera.targetZoom = 20;
		}
	else {
		if (seekPoint.x < -5) {
			p1 = glm::vec2(-25, 2);
			if (players[0]->getPosition().x < players[1]->getPosition().x) {
				p2 = glm::vec2(players[1]->getPosition().x, players[1]->getPosition().y);
				if (players[0]->getPosition().y > 2)
					p1.y = players[0]->getPosition().y;
			}
			else {
				p2 = glm::vec2(players[0]->getPosition().x, players[0]->getPosition().y);
				if (players[1]->getPosition().y > 2)
					p1.y = players[1]->getPosition().y;
			}
		}
		else if (seekPoint.x > 5) {
			p1 = glm::vec2(25, 2);
			if (players[0]->getPosition().x > players[1]->getPosition().x) {
				p2 = glm::vec2(players[1]->getPosition().x, players[1]->getPosition().y);
				if (players[0]->getPosition().y > 2)
					p1.y = players[0]->getPosition().y;
			}
			else {
				p2 = glm::vec2(players[0]->getPosition().x, players[0]->getPosition().y);
				if (players[1]->getPosition().y > 2)
					p1.y = players[1]->getPosition().y;
			}
		}
		else {
			p1 = players[0]->getPosition();
			p2 = players[1]->getPosition();
		}

		GameCamera.targetZoom = (abs(p1.x - p2.x) / 6) + 10;
		seekPoint.x = (p1.x + p2.x) * 0.5f;//seek point is inbetween the 2 players
		seekPoint.y = ((p1.y + p2.y) * 0.5f) - 2;//seek point is inbetween the 2 players
	}

	GameCamera.seekPoint = seekPoint;
	
	GameCamera.update();
	//GameCamera.CameraTransform = Transform::Identity();
	//GameCamera.CameraTransform.Translate(glm::vec3((players[1]->getPosition().x + players[0]->getPosition().x) / 2.0f, abs(sqrtf(dist*0.01f)*18.5f) + 10.0f + ((players[1]->getPosition().y + players[0]->getPosition().y) / 2.0f), (dist* 0.75f) + 9));

	hudTransform = Transform::Identity();
	hudTransform.RotateY(0.0f);

	ShadowTransform = Transform::Identity();
	ShadowTransform.RotateX(45.0f);
	//.RotateY(180.0f);
	ShadowTransform.Translate(glm::vec3(0.0f, 10.0f, 10.0f));
	ShadowTransform.RotateY(180.0f);
	
	Transform bias = Transform(0.5f, 0.0f, 0.0f, 0.5f,
					 0.0f, 0.5f, 0.0f, 0.5f,
					 0.0f, 0.0f, 0.5f, 0.5f,
					 0.0f, 0.0f, 0.0f, 1.0f);

	ViewToShadowMap = Transform::Identity();
	ViewToShadowMap = bias * ShadowProjection * ShadowTransform.GetInverse() * GameCamera.CameraTransform;
	//ShadowTransform.Translate(vec3(0.0f, 0.0f, 0.0f));


	///PARTICLE EFFECTS
	ParticleEffect::cameraOffset = GameCamera.pos.x;
	//cout << ParticleEffect::cameraOffset << ",";
	//Update Patricle Effects
	ConfettiEffectBlueRight.Update(deltaTime);
	ConfettiEffectBlueLeft.Update(deltaTime);
	ConfettiEffectRedRight.Update(deltaTime);
	ConfettiEffectRedLeft.Update(deltaTime);
	ConfettiEffectOrangeRight.Update(deltaTime);
	ConfettiEffectOrangeLeft.Update(deltaTime);
	ConfettiEffectPurpleRight.Update(deltaTime);
	ConfettiEffectPurpleLeft.Update(deltaTime);
	basicLeftNet.Update(deltaTime);
	basicRightNet.Update(deltaTime);
	basicLeftNet1.Update(deltaTime);
	basicRightNet1.Update(deltaTime);
	ninjaLeftNet.Update(deltaTime);
	ninjaRightNet.Update(deltaTime);
	ninjaLeftNet1.Update(deltaTime);
	ninjaRightNet1.Update(deltaTime);
	knightLeftNet.Update(deltaTime);
	knightRightNet.Update(deltaTime);
	knightLeftNet1.Update(deltaTime);
	knightRightNet1.Update(deltaTime);
	DustDashL.Update(deltaTime);
	DustDashR.Update(deltaTime);
	DustLand.Update(deltaTime);
	DustLaunch.Update(deltaTime);
	HitSparkL.Update(deltaTime);
	HitSparkR.Update(deltaTime);
	MeterFlame1.Update(deltaTime);
	MeterFlame2.Update(deltaTime);
	KnightUltFX.Update(deltaTime);
	NinjaUltFX1.Update(deltaTime);
	NinjaUltFX2.Update(deltaTime);

	if (NinjaPetals.Playing) {
		NinjaPetals.Update(deltaTime);
		NinjaPetals2.Update(deltaTime);

		//Vect Fields//
		glm::vec2 playerTransform1 = glm::vec2(players[0]->getPosition().x, players[0]->getPosition().y);
		glm::vec2 playerTransform2 = glm::vec2(players[1]->getPosition().x, players[1]->getPosition().y);

		NinjaPetals.mainField.update(playerTransform1, players[0]->movementDir, playerTransform2, players[1]->movementDir);
		NinjaPetals2.mainField.update(playerTransform1, players[0]->movementDir, playerTransform1, players[1]->movementDir);
	}
	//cout << playerTransform1.x << "," << playerTransform1.y << endl;

	//Sound Effects//
	p1Pos = { players[0]->getPosition().x, players[0]->getPosition().y, players[0]->getPosition().z };
	p2Pos = { players[1]->getPosition().x, players[1]->getPosition().y, players[1]->getPosition().z };

	//Generate random number for sounds
	randSound1 = RandomNum(maxRand);

#pragma region P1SoundEffects
	//Walk
	if (players[0]->action == 1)
	{
		static float walkTime;
		walkTime += updateTimer->getElapsedTimeSeconds();

		if (!p1Walk)
		{
			if (isNinja1)
			{
				if (walkTime >= 0.23f)
				{
					if (randSound1 == 0)
						ninjaChannel = ninjaWalk.Play(p1Pos, defaultPos, false);
					else if (randSound1 == 1)
						ninjaChannel = ninjaWalk1.Play(p1Pos, defaultPos, false);
					else if (randSound1 == 2)
						ninjaChannel = ninjaWalk2.Play(p1Pos, defaultPos, false);

					walkTime = 0.0f;
				}
			}
			else
			{
				if (walkTime >= 0.35f)
				{
					if (randSound1 == 0)
						knightChannel = knightWalk.Play(p1Pos, defaultPos, false);
					else if (randSound1 == 1)
						knightChannel = knightWalk1.Play(p1Pos, defaultPos, false);
					else if (randSound1 == 2)
						knightChannel = knightWalk2.Play(p1Pos, defaultPos, false);

					walkTime = 0.0f;
				}
			}
		}
	}
	else p1Walk = true;
	//Run
	if (players[0]->action == 2)
	{
		static float runTime;
		runTime += updateTimer->getElapsedTimeSeconds();

		if (!p1Run)
		{
			if (isNinja1)
			{
				if (randSound1 == 0)
					ninjaChannel = ninjaWalk.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 1)
					ninjaChannel = ninjaWalk1.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 2)
					ninjaChannel = ninjaWalk2.Play(p1Pos, defaultPos, false);
			}
			else
			{
				if (randSound1 == 0)
					knightChannel = knightWalk.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 1)
					knightChannel = knightWalk1.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 2)
					knightChannel = knightWalk2.Play(p1Pos, defaultPos, false);
			}
			p1Run = true;
		}

		if (runTime >= 0.2f)
		{
			p1Run = false;
			runTime = 0.0f;
		}
	}
	//Jump1
	if (players[0]->action == 5)
	{
		if (!p1Jump1)
		{
			if (isNinja1)
			{
				if (randSound1 == 0)
					ninjaChannel = ninjaJump.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 1)
					ninjaChannel = ninjaJump1.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 2)
					ninjaChannel = ninjaJump2.Play(p1Pos, defaultPos, false);
			}
			else
			{
				if (randSound1 == 0)
					knightChannel = knightJump.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 1)
					knightChannel = knightJump1.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 2)
					knightChannel = knightJump2.Play(p1Pos, defaultPos, false);
			}
			p1Jump1 = true;
		}
	}
	//Jump2
	if (players[0]->action == 6)
	{
		if (!p1Jump2)
		{
			if (isNinja1)
			{
				if (randSound1 == 0)
					ninjaChannel = ninjaJump.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 1)
					ninjaChannel = ninjaJump1.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 2)
					ninjaChannel = ninjaJump2.Play(p1Pos, defaultPos, false);
			}
			else
			{
				if (randSound1 == 0)
					knightChannel = knightJump.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 1)
					knightChannel = knightJump1.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 2)
					knightChannel = knightJump2.Play(p1Pos, defaultPos, false);
			}
			p1Jump2 = true;
		}
	}
	//Attack with hit -- check the opposing player to see if they are hit
	if (players[1]->action == 8 && playHit1 == true)
	{
		static float hitTime;
		hitTime += updateTimer->getElapsedTimeSeconds();

		if (!p1Hit)
		{
			if (isNinja1)
			{
				if (randSound1 == 0)
					ninjaChannel = hit.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 1)
					ninjaChannel = hit1.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 2)
					ninjaChannel = hit2.Play(p1Pos, defaultPos, false);
			}
			else
			{
				if (randSound1 == 0)
					knightChannel = hit.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 1)
					knightChannel = hit1.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 2)
					knightChannel = hit2.Play(p1Pos, defaultPos, false);
			}
			
			p1Hit = true;
			playHit1 = false;
		}

		if (hitTime >= 0.23f)
		{
			p1Hit = false;
			hitTime = 0.0f;
		}
	}
	//Dash
	if (players[0]->action == 22)
	{
		if (!p1Dash)
		{
			if (isNinja1)
			{
				if (randSound1 == 0)
					ninjaChannel = dash.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 1)
					ninjaChannel = dash1.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 2)
					ninjaChannel = dash2.Play(p1Pos, defaultPos, false);
			}
			else
			{
				if (randSound1 == 0)
					knightChannel = dash.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 1)
					knightChannel = dash1.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 2)
					knightChannel = dash2.Play(p1Pos, defaultPos, false);
			}
			p1Dash = true;
		}
	}
	//Attack without hit
	if (players[0]->action == 10 || players[0]->action == 11 || players[0]->action == 12 || players[0]->action == 13 ||
		players[0]->action == 18 || players[0]->action == 19 || players[0]->action == 20 || players[0]->action == 21 &&
		players[1]->action != 8)
	{
		if (!p1Attack)
		{
			if (isNinja1)
			{
				if (randSound1 == 0)
					ninjaChannel = attack.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 1)
					ninjaChannel = attack1.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 2)
					ninjaChannel = attack2.Play(p1Pos, defaultPos, false);
			}
			else
			{
				if (randSound1 == 0)
					knightChannel = attack.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 1)
					knightChannel = attack1.Play(p1Pos, defaultPos, false);
				else if (randSound1 == 2)
					knightChannel = attack2.Play(p1Pos, defaultPos, false);
			}
			p1Attack = true;
		}
	}
	else p1Attack = false;

	//Make sure sounds dont repeat and sound terrible
	//Walk
	if (players[0]->action != 1)
	{
		p1Walk = false;
	}
	//Run
	if (players[0]->action != 2)
	{
		p1Run = false;
	}
	//Jump1
	if (players[0]->action != 5)
	{
		p1Jump1 = false;
	}
	//Jump2
	if (players[0]->action != 6)
	{
		p1Jump2 = false;
	}
	//Hit
	if (players[1]->action != 8)
	{
		p1Hit = false;
	}
	//Dash
	if (players[0]->action != 22)
	{
		p1Dash = false;
	}
#pragma endregion
	
	//Generate random number for sounds
	randSound2 = RandomNum(maxRand);

#pragma region P2SoundEffects
	//Walk
	if (players[1]->action == 1)
	{
		static float walkTime;
		walkTime += updateTimer->getElapsedTimeSeconds();

		if (isNinja2)
		{
			if (isNinja2)
			{
				if (walkTime >= 0.23f)
				{
					if (randSound2 == 0)
						ninjaChannel = ninjaWalk.Play(p2Pos, defaultPos, false);
					else if (randSound2 == 1)
						ninjaChannel = ninjaWalk1.Play(p2Pos, defaultPos, false);
					else if (randSound2 == 2)
						ninjaChannel = ninjaWalk2.Play(p2Pos, defaultPos, false);

					walkTime = 0.0f;
				}
			}
			else
			{
				if (walkTime >= 0.35f)
				{
					if (randSound2 == 0)
						knightChannel = knightWalk.Play(p2Pos, defaultPos, false);
					else if (randSound2 == 1)
						knightChannel = knightWalk1.Play(p2Pos, defaultPos, false);
					else if (randSound2 == 2)
						knightChannel = knightWalk2.Play(p2Pos, defaultPos, false);

					walkTime = 0.0f;
				}
			}
		}
	}
	else p2Walk = true;
	//Run
	if (players[1]->action == 2)
	{
		static float runTime;
		runTime += updateTimer->getElapsedTimeSeconds();

		if (!p2Run)
		{
			if (isNinja2)
			{
				if (randSound2 == 0)
					ninjaChannel = ninjaWalk.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 1)
					ninjaChannel = ninjaWalk1.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 2)
					ninjaChannel = ninjaWalk2.Play(p2Pos, defaultPos, false);
			}
			else
			{
				if (randSound2 == 0)
					knightChannel = knightWalk.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 1)
					knightChannel = knightWalk1.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 2)
					knightChannel = knightWalk2.Play(p2Pos, defaultPos, false);
			}
			p2Run = true;
		}

		if (runTime >= 0.2f)
		{
			p2Run = false;
			runTime = 0.0f;
		}
	}
	//Jump1
	if (players[1]->action == 5)
	{
		if (!p2Jump1)
		{
			if (isNinja2)
			{
				if (randSound2 == 0)
					ninjaChannel = ninjaJump.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 1)
					ninjaChannel = ninjaJump1.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 2)
					ninjaChannel = ninjaJump2.Play(p2Pos, defaultPos, false);
			}
			else
			{
				if (randSound2 == 0)
					knightChannel = knightJump.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 1)
					knightChannel = knightJump1.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 2)
					knightChannel = knightJump2.Play(p2Pos, defaultPos, false);
			}
			p2Jump1 = true;
		}
	}
	//Jump2
	if (players[1]->action == 6)
	{
		if (!p2Jump2)
		{
			if (isNinja2)
			{
				if (randSound2 == 0)
					ninjaChannel = ninjaJump.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 1)
					ninjaChannel = ninjaJump1.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 2)
					ninjaChannel = ninjaJump2.Play(p2Pos, defaultPos, false);
			}
			else
			{
				if (randSound2 == 0)
					knightChannel = knightJump.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 1)
					knightChannel = knightJump1.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 2)
					knightChannel = knightJump2.Play(p2Pos, defaultPos, false);
			}
			p2Jump2 = true;
		}
	}
	//Attack with hit -- check the opposing player to see if they are hit
	if (players[0]->action == 8 && playHit2 == true)
	{
		static float hitTime;
		hitTime += updateTimer->getElapsedTimeSeconds();

		if (!p2Hit)
		{
			if (isNinja2)
			{
				if (randSound2 == 0)
					ninjaChannel = hit.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 1)
					ninjaChannel = hit1.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 2)
					ninjaChannel = hit2.Play(p2Pos, defaultPos, false);
			}
			else
			{
				if (randSound2 == 0)
					knightChannel = hit.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 1)
					knightChannel = hit1.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 2)
					knightChannel = hit2.Play(p2Pos, defaultPos, false);

			}
			p2Hit = true;
			playHit2 = false;
		}

		if (hitTime >= 0.23f)
		{
			p1Hit = false;
			hitTime = 0.0f;
		}
	}
	//Dash
	if (players[1]->action == 22)
	{
		if (!p2Dash)
		{
			if (isNinja2)
			{
				if (randSound2 == 0)
					ninjaChannel = dash.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 1)
					ninjaChannel = dash1.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 2)
					ninjaChannel = dash2.Play(p2Pos, defaultPos, false);
			}
			else
			{
				if (randSound2 == 0)
					knightChannel = dash.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 1)
					knightChannel = dash1.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 2)
					knightChannel = dash2.Play(p2Pos, defaultPos, false);
			}
			p2Dash = true;
		}
	}
	//Attack without hit
	if (players[1]->action == 10 || players[1]->action == 11 || players[1]->action == 12 || players[1]->action == 13 ||
		players[1]->action == 18 || players[1]->action == 19 || players[1]->action == 20 || players[1]->action == 21 &&
		players[0]->action != 8)
	{
		if (!p2Attack)
		{
			if (isNinja2)
			{
				if (randSound2 == 0)
					ninjaChannel = attack.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 1)
					ninjaChannel = attack1.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 2)
					ninjaChannel = attack2.Play(p2Pos, defaultPos, false);
			}
			else
			{
				if (randSound2 == 0)
					knightChannel = attack.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 1)
					knightChannel = attack1.Play(p2Pos, defaultPos, false);
				else if (randSound2 == 2)
					knightChannel = attack2.Play(p2Pos, defaultPos, false);
			}
			p2Attack = true;
		}
	}
	else p2Attack = false;

	//Make sure sounds dont repeat and sound terrible
	//Walk
	if (players[1]->action != 1)
	{
		p2Walk = false;
	}
	//Run
	if (players[1]->action != 2)
	{
		p2Run = false;
	}
	//Jump1
	if (players[1]->action != 5)
	{
		p2Jump1 = false;
	}
	//Jump2
	if (players[1]->action != 6)
	{
		p2Jump2 = false;
	}
	//Hit -- check the opposing player to see if they are hit
	if (players[1]->action != 8)
	{
		p2Hit = false;
	}
	//Dash
	if (players[1]->action != 22)
	{
		p2Dash = false;
	}
#pragma endregion

	if (p1Score == true)
	{
		if (!cheerPlaying1)
		{
			otherChannel = cheer.Play(defaultPos, defaultPos, false);
			cheerPlaying1 = true;
		}
	}

	if (p2Score == true)
	{
		if (!cheerPlaying2)
		{
			otherChannel = cheer.Play(defaultPos, defaultPos, false);
			cheerPlaying2 = true;
		}
	}

	//Announcer voice effects for times
	if (TotalGameTime >= 240.0f)
	{
		if (!onePlaying)
		{
			otherChannel->setVolume(3.0f);
			otherChannel = oneMin.Play(defaultPos, defaultPos, false);
			onePlaying = true;
		}
	}
	if (TotalGameTime >= 270.0f)
	{
		if (!thirtyPlaying)
		{
			otherChannel->setVolume(3.0f);
			otherChannel = thirtySec.Play(defaultPos, defaultPos, false);
			thirtyPlaying = true;
		}
	}

	//additional lights
	if (p1Score == true)
	{
		static float timer;
		timer += updateTimer->getElapsedTimeSeconds();

		if (stageVal != 3)
		{
			bool temp = false;

			float check = (timer - (int)timer);
			temp = (bool)(check >= 0.5f && check < 1.0f);

			if (timer >= 2.5f)
				temp = false;

			findLight("p1Score")->active = temp;

		}

		if (timer >= 3.0f)
		{
			p1Score = false;
			p2Score = false;
			cheerPlaying1 = false;
			timer = 0.0f;
		}
	}
	if (p2Score == true)
	{
		static float timer;
		timer += updateTimer->getElapsedTimeSeconds();

		if (stageVal != 3)
		{
			bool temp = false;

			float check = (timer - (int)timer);
			temp = (bool)(check >= 0.5f && check < 1.0f);

			if (timer >= 2.5f)
				temp = false;

			findLight("p2Score")->active = temp;
		}

		if (timer >= 3.0f)
		{
			p1Score = false;
			p2Score = false;
			cheerPlaying2 = false;
			timer = 0.0f;
		}
	}

	//End Game
	if (score1 == 10 || score2 == 10 || TotalGameTime >= 300)
	{
		//Make game black and white
		grayscale = true;

		//Delay timer before going to results screen
		static float tempTime;
		tempTime += updateTimer->getElapsedTimeSeconds();
		
		if (!hornPlaying)
		{
			otherChannel->setPriority(0);
			otherChannel = horn.Play(defaultPos, defaultPos, false);
			hornPlaying = true;
		}

		//Bring volume to 0
		decreaseVal -= 0.005f;
		themeChannel->setVolume(decreaseVal);

		//Check if player 1 won
		if (score1 > score2)
		{
			//Check what character they are
			if (p1Char == 1)
				p1KnightWin = true;
			else if (p1Char == 2)
				p1NinjaWin = true;
		}

		//Check if player 2 won
		if (score2 > score1)
		{
			//Check what character they are
			if (p2Char == 1)
				p2KnightWin = true;
			else if (p2Char == 2)
				p2NinjaWin = true;
		}

		if (score1 == score2)
		{
			tieGame = true;
		}

		if (tempTime >= 1.5f)
		{
			stageDone = false;
			stageVal = 1;
			scene = 4;
			TotalGameTime = 0.0f;
			lastInputTime = 0.0f;
			p1Char = 0;
			p2Char = 0;
			p1Done = false;
			p2Done = false;
			tempTime = 0.0f;
			decreaseVal = 1.0f;
			grayscale = false;
			isNinja1 = false;
			isNinja2 = false;
			hornPlaying = false;
			onePlaying = false;
			thirtyPlaying = false;
		}
	}

	///Court specific functionality
	//Basic Court
	if (stageVal == 1)
	{
		//Move the crowds up and down
		float movementVal1 = (cos((TotalGameTime) * 9 + 110) + .5f);
		findObjects(3, "default_crowd1")->transform.SetTranslation(glm::vec3(0.0f, movementVal1, 0.0f));

		float movementVal2 = (cos((TotalGameTime) * 5 + 55) + .75f);
		findObjects(3, "default_crowd2")->transform.SetTranslation(glm::vec3(0.0f, movementVal2, 0.0f));

		float movementVal3 = (cos((TotalGameTime * 7) + 25) + .25f);
		findObjects(3, "default_crowd3")->transform.SetTranslation(glm::vec3(0.0f, movementVal3, 0.0f));

		basicLeftNet.Spawn(1.0f);
		basicRightNet.Spawn(1.0f);
		basicLeftNet1.Spawn(1.0f);
		basicRightNet1.Spawn(1.0f);


		//Additional lights in the basic court
		findLight("lightLeft")->active = true;
		findLight("lightRight")->active = true;
		findLight("lightCenter")->active = true;
	}
	//Knight Court
	if (stageVal == 2)
	{
		//Move the crowds up and down
		float movementVal1 = (cos((TotalGameTime) * 9 + 110) + .6f);
		findObjects(3, "knight_crowd1")->transform.SetTranslation(glm::vec3(0.0f, movementVal1, 0.0f));

		float movementVal2 = (cos((TotalGameTime) * 5 + 55) + .85f);
		findObjects(3, "knight_crowd2")->transform.SetTranslation(glm::vec3(0.0f, movementVal2, 0.0f));

		float movementVal3 = (cos((TotalGameTime * 7) + 25) + .73f);
		findObjects(3, "knight_crowd3")->transform.SetTranslation(glm::vec3(0.0f, movementVal3, 0.0f));

		knightLeftNet.Spawn(1.0f);
		knightRightNet.Spawn(1.0f);
		knightLeftNet1.Spawn(1.0f);
		knightRightNet1.Spawn(1.0f);


		//Turn off the lights from the basic court
		findLight("lightLeft")->active = false;
		findLight("lightRight")->active = false;
		findLight("lightCenter")->active = false;
	}
	//Ninja Court
	if (stageVal == 3)
	{
		//Move the crowds up and down
		float movementVal1 = (cos((TotalGameTime) * 9 + 110) + 1.0f);
		findObjects(3, "ninja_crowd1")->transform.SetTranslation(glm::vec3(0.0f, movementVal1, 0.0f));

		float movementVal2 = (cos((TotalGameTime) * 5 + 55) + 1.25f);
		findObjects(3, "ninja_crowd2")->transform.SetTranslation(glm::vec3(0.0f, movementVal2, 0.0f));

		float movementVal3 = (cos((TotalGameTime * 7) + 25) + 1.13f);
		findObjects(3, "ninja_crowd3")->transform.SetTranslation(glm::vec3(0.0f, movementVal3, 0.0f));

		//Flower petal effect
		NinjaPetals.Spawn(1.0f);
		NinjaPetals2.Spawn(1.0f);
		ninjaLeftNet.Spawn(1.0f);
		ninjaRightNet.Spawn(1.0f);
		ninjaLeftNet1.Spawn(1.0f);
		ninjaRightNet1.Spawn(1.0f);


		//Turn off the lights from the basic court
		findLight("lightLeft")->active = false;
		findLight("lightRight")->active = false;
		findLight("lightCenter")->active = false;
	}
}
/*
***Always remember to ask the three questions***
What are we rendering
Where are we rendering it to
How are we rendering it
*/

void Game::draw()
{
	if (scene == 5)
	{
		drawTutScreen();
	}
	else if (scene == 4) {
		drawEndScreen();
	}
	else if (scene == 3) {
		drawScene();
	}
	else if (scene == 2) {
		drawSSS();
	}
	else if(scene == 1) {
		drawCSS();
	}
	else {
		drawMenu();
	}
}

void Game::drawTutScreen()
{
	/// Clear Buffers ///
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.1f, 0.2f, 0.3f, 0);
	DeferredComposite.Clear();

	glClearColor(0.0f, 0.0f, 0.0f, 0);
	GBuffer.Clear();
	ShadowMap.Clear();
	HudMap.Clear();
	WorkBuffer1.Clear();
	WorkBuffer2.Clear();

	/// Create Scene From GBuffer ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	DeferredLighting.Bind();
	DeferredLighting.SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting.SendUniform("uScene", 0);
	DeferredLighting.SendUniform("uShadowMap", 1);
	DeferredLighting.SendUniform("uNormalMap", 2);
	DeferredLighting.SendUniform("uPositionMap", 3);
	//DeferredLighting.SendUniform("uEdgeMap", 4);
	//DeferredLighting.SendUniform("uStepTexture", 4);

	DeferredLighting.SendUniform("LightDirection", glm::vec3(GameCamera.CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	if (darkMode) {
		DeferredLighting.SendUniform("LightAmbient", glm::vec3(0.0f, 0.0f, 0.0f)); 
	}
	else {
		DeferredLighting.SendUniform("LightAmbient", glm::vec3(0.6f, 0.6f, 0.6f)); 
	}
	DeferredLighting.SendUniform("LightDiffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting.SendUniform("LightSpecular", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting.SendUniform("LightSpecularExponent", 500.0f);

	DeferredComposite.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));



	DrawFullScreenQuad();

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?

	DeferredComposite.UnBind();
	DeferredLighting.UnBind();

	//===============================================================
		//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
	if (FULLSCREEN)
		gluOrtho2D((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, (float)FULLSCREEN_HEIGHT * 0.5f);//create ortho
	else
		gluOrtho2D((float)WINDOW_WIDTH * -0.5f, (float)WINDOW_WIDTH * 0.5f, (float)WINDOW_HEIGHT * -0.5f, (float)WINDOW_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

//////////////////////////
	//now ready to draw 2d
//////////////////////////
	GBufferPass.Bind();
	hudTransform = Transform::Identity();
	GBufferPass.SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", hudProjection.data, true);

	//draws everything in menu
	sortObjects(5);
	for (int i = 0; i < (int)tutObjects.size(); i++) {
		tutObjects[i]->draw(GBufferPass, 1);
	}

	GBufferPass.UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	DeferredComposite.UnBind();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	//===============================================================

		/// Compute High Pass ///
	if (FULLSCREEN)
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	else
		glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));
	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass.Bind();
	BloomHighPass.SendUniform("uTex", 0);
	BloomHighPass.SendUniform("uThreshold", 1.0f);
	WorkBuffer1.Bind();
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	WorkBuffer1.UnBind();
	BloomHighPass.UnBind();

	/// Compute Blur ///
	BloomHighPass.Bind();
	BloomHighPass.SendUniform("bloomOn", false);
	BloomHighPass.UnBind();

	if (FULLSCREEN)
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	else
		glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal.Bind();
		BlurHorizontal.SendUniform("uTex", 0);
		if (FULLSCREEN)
			BlurHorizontal.SendUniform("uPixelSize", 1.0f / FULLSCREEN_WIDTH);
		else
			BlurHorizontal.SendUniform("uPixelSize", 1.0f / WINDOW_WIDTH);
		WorkBuffer2.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer2.UnBind();
		BlurHorizontal.UnBind();

		//Vertical Blur
		BlurVertical.Bind();
		BlurVertical.SendUniform("uTex", 0);
		if (FULLSCREEN)
			BlurVertical.SendUniform("uPixelSize", 1.0f / FULLSCREEN_HEIGHT);
		else
			BlurVertical.SendUniform("uPixelSize", 1.0f / WINDOW_HEIGHT);
		WorkBuffer1.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer1.UnBind();
		BlurVertical.UnBind();

	}

	/// Composite To Back Buffer ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	BloomComposite.Bind();
	BloomComposite.SendUniform("uScene", 0);
	BloomComposite.SendUniform("uBloom", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	BloomComposite.UnBind();

	glutSwapBuffers();
}

void Game::drawEndScreen()
{
	/// Clear Buffers ///
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.1f, 0.2f, 0.3f, 0);
	DeferredComposite.Clear();

	glClearColor(0.0f, 0.0f, 0.0f, 0);
	GBuffer.Clear();
	ShadowMap.Clear();
	HudMap.Clear();
	WorkBuffer1.Clear();
	WorkBuffer2.Clear();
	
	/// Create Scene From GBuffer ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	DeferredLighting.Bind();
	DeferredLighting.SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting.SendUniform("uScene", 0);
	DeferredLighting.SendUniform("uShadowMap", 1);
	DeferredLighting.SendUniform("uNormalMap", 2);
	DeferredLighting.SendUniform("uPositionMap", 3);
	//DeferredLighting.SendUniform("uEdgeMap", 4);
	//DeferredLighting.SendUniform("uStepTexture", 4);

	DeferredLighting.SendUniform("LightDirection", glm::vec3(GameCamera.CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	DeferredLighting.SendUniform("LightAmbient", glm::vec3(0.6f, 0.6f, 0.6f)); //You can LERP through colours to make night to day cycles
	DeferredLighting.SendUniform("LightDiffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting.SendUniform("LightSpecular", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting.SendUniform("LightSpecularExponent", 500.0f);

	DeferredComposite.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));



	DrawFullScreenQuad();

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?

	DeferredComposite.UnBind();
	DeferredLighting.UnBind();

	//===============================================================
		//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
	if (FULLSCREEN)
		gluOrtho2D((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, (float)FULLSCREEN_HEIGHT * 0.5f);//create ortho
	else
		gluOrtho2D((float)WINDOW_WIDTH * -0.5f, (float)WINDOW_WIDTH * 0.5f, (float)WINDOW_HEIGHT * -0.5f, (float)WINDOW_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

//////////////////////////
	//now ready to draw 2d
//////////////////////////
	GBufferPass.Bind();
	hudTransform = Transform::Identity();
	GBufferPass.SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", hudProjection.data, true);

	//draws everything in menu
	sortObjects(4);
	for (int i = 0; i < (int)endObjects.size(); i++) {
		if (p1KnightWin)
			endObjects[0]->draw(GBufferPass, 1);
		if (p1NinjaWin)
			endObjects[2]->draw(GBufferPass, 1);
		if (p2KnightWin)
			endObjects[1]->draw(GBufferPass, 1);
		if (p2NinjaWin)
			endObjects[3]->draw(GBufferPass, 1);
		if (tieGame)
			endObjects[4]->draw(GBufferPass, 1);
	}

	GBufferPass.UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	DeferredComposite.UnBind();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	//===============================================================

		/// Compute High Pass ///
	BloomHighPass.Bind();
	BloomHighPass.SendUniform("bloomOn", false);
	BloomHighPass.UnBind();

	if (FULLSCREEN)
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	else
		glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));
	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass.Bind();
	BloomHighPass.SendUniform("uTex", 0);
	BloomHighPass.SendUniform("uThreshold", 1.0f);
	WorkBuffer1.Bind();
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	WorkBuffer1.UnBind();
	BloomHighPass.UnBind();

	/// Compute Blur ///
	if (FULLSCREEN)
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	else
		glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal.Bind();
		BlurHorizontal.SendUniform("uTex", 0);
		if (FULLSCREEN)
			BlurHorizontal.SendUniform("uPixelSize", 1.0f / FULLSCREEN_WIDTH);
		else
			BlurHorizontal.SendUniform("uPixelSize", 1.0f / WINDOW_WIDTH);
		WorkBuffer2.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer2.UnBind();
		BlurHorizontal.UnBind();

		//Vertical Blur
		BlurVertical.Bind();
		BlurVertical.SendUniform("uTex", 0);
		if (FULLSCREEN)
			BlurVertical.SendUniform("uPixelSize", 1.0f / FULLSCREEN_HEIGHT);
		else
			BlurVertical.SendUniform("uPixelSize", 1.0f / WINDOW_HEIGHT);
		WorkBuffer1.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer1.UnBind();
		BlurVertical.UnBind();
	}

	/// Composite To Back Buffer ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	BloomComposite.Bind();
	BloomComposite.SendUniform("uScene", 0);
	BloomComposite.SendUniform("uBloom", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	BloomComposite.UnBind();

	glutSwapBuffers();
}

void Game::drawScene()
{

	/// Clear Buffers ///
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.1f, 0.2f, 0.3f, 0);
	DeferredComposite.Clear();

	glClearColor(0.0f, 0.0f, 0.0f, 0);
	GBuffer.Clear();
	ShadowMap.Clear();
	HudMap.Clear();
	WorkBuffer1.Clear();
	WorkBuffer2.Clear();

	/// Generate The Shadow Map ///
	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);

	GBufferPass.Bind();
	GBufferPass.SendUniformMat4("uModel", Transform().data, true);
	//The reason of the inverse is because it is easier to do transformations
	GBufferPass.SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", ShadowProjection.data, true);

	ShadowMap.Bind();

	Object* temp;
	if (stageVal == 1)
		temp = findObjects(3, "default_court");
	else if (stageVal == 2)
		temp = findObjects(3, "knight_court");
	else
		temp = findObjects(3, "ninja_court");

	glBindVertexArray(temp->body.VAO);
	glDrawArrays(GL_TRIANGLES, 0, temp->body.GetNumVertices());

	//glBindVertexArray(Background.VAO);
	//glDrawArrays(GL_TRIANGLES, 0, Background.GetNumVertices());

	//GBufferPass.SendUniformMat4("uModel",playerOne->transform.data, true);
	//playerOne->drawShadow(GBufferPass, 1);
	//GBufferPass.SendUniformMat4("uModel", playerTwo->transform.data, true);
	//playerTwo->drawShadow(GBufferPass, 1);

	GBufferPass.SendUniformMat4("uModel", Transform().data, true);
	GBufferPass.SendUniformMat4("uModel", Transform().data, true);


	glDisable(GL_CULL_FACE);//should fix random holes in knight

	GBufferPass.UnBind();
	//draw p1 shadow
	AniShader.Bind();
	AniShader.SendUniformMat4("uModel", Transform().data, true);
	AniShader.SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
	AniShader.SendUniformMat4("uProj", ShadowProjection.data, true);
	AniShader.SendUniformMat4("uModel", players[0]->transform.data, true);
	players[0]->draw(AniShader, 0);
	AniShader.SendUniformMat4("uModel", Transform().data, true);

	//draw p2 shadow
	AniShader.Bind();
	AniShader.SendUniformMat4("uModel", Transform().data, true);
	AniShader.SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
	AniShader.SendUniformMat4("uProj", ShadowProjection.data, true);
	AniShader.SendUniformMat4("uModel", players[1]->transform.data, true);
	players[1]->draw(AniShader, 0);
	AniShader.SendUniformMat4("uModel", Transform().data, true);

	glEnable(GL_CULL_FACE);//turn it back on after for preformance

	glBindVertexArray(0);

	ShadowMap.UnBind();
	AniShader.UnBind();

	/// Generate The Scene ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	GBufferPass.Bind();
	GBufferPass.SendUniformMat4("uModel", Transform().data, true);
	//The reason of the inverse is because it is easier to do transformations
	GBufferPass.SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
	//GBufferPass.SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);

	//MAKE SURE TO KNOW WHAT VIEWSPACE YOU ARE WORKING IN
	GBufferPass.SendUniform("uTex", 0);

	GBuffer.Bind();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE0);

	///playerOne->draw(GBufferPass);
	///playerTwo->draw(GBufferPass);

	//draws everything in scene
	sortObjects(3);
	for (int i = 0; i < (int)gameObjects.size(); i++) {
		if (gameObjects[i] == findObjects(3, "default_words"))
		{
			AdShader.Bind();
			AdShader.SendUniformMat4("uModel", Transform().data, true);
			AdShader.SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
			AdShader.SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
			AdShader.SendUniform("uTex", 0);
			AdShader.SendUniform("uTime", TotalGameTime);
			gameObjects[i]->draw(AdShader, 1);

			AdShader.UnBind();
		}
		else if (gameObjects[i] == findObjects(3, "smoke_net") || gameObjects[i] == findObjects(3, "fire_net") || gameObjects[i] == findObjects(3, "petals_net"))
		{
			glEnable(GL_BLEND);
			NetShader.Bind();
			NetShader.SendUniformMat4("uModel", Transform().data, true);
			NetShader.SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
			NetShader.SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
			NetShader.SendUniform("uTex", 0);
			NetShader.SendUniform("uTime", TotalGameTime);
			gameObjects[i]->draw(NetShader, 1);

			NetShader.UnBind();
			glDisable(GL_BLEND);
		}
		else
		{
			GBufferPass.Bind();
			GBufferPass.SendUniformMat4("uModel", Transform().data, true);
			//The reason of the inverse is because it is easier to do transformations
			GBufferPass.SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
			GBufferPass.SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
			//GBufferPass.SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);

			//MAKE SURE TO KNOW WHAT VIEWSPACE YOU ARE WORKING IN
			GBufferPass.SendUniform("uTex", 0);
			gameObjects[i]->draw(GBufferPass, 1);
		}
	}

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	glBindVertexArray(0);

	//GBuffer.UnBind();

	//DRAW NET HITBOX CODE
	//	for (unsigned int i = 0; i < Netbox.size(); i++) {
	//		int modelLoc = glGetUniformLocation(GBufferPass.getProgram(), "uModel");
	//		glUniformMatrix4fv(modelLoc, 1, false, Netbox[i]->getTransform().data);
	//
	//		boxTexture.Bind();
	//		glBindVertexArray(boxMesh.VAO);
	//
	//		// Adjust model matrix for next object's location
	//		glDrawArrays(GL_TRIANGLES, 0, boxMesh.GetNumVertices());
	//		glUniformMatrix4fv(modelLoc, 1, false, Transform().data);
	//	}
	//	boxTexture.UnBind();


	players[0]->drawBoxes(GBufferPass);
	players[1]->drawBoxes(GBufferPass);
	//playerOne->draw(GBufferPass);
	//playerTwo->draw(GBufferPass);

	///Ani Shader///
	/*AniShader.Bind();
	static float aniTimer = 0.f;
	static int index = 0;
	aniTimer += updateTimer->getElapsedTimeSeconds();
	if (aniTimer > 1.0f)
	{
		aniTimer = 0.0f;
		index = (index + 1) % 2;
	}
	// Ask for the handles identfying the uniform variables in our shader.
	AniShader.SendUniformMat4("uModel", playerOne->transform.data, true);
	AniShader.SendUniformMat4("uView", CameraTransform.GetInverse().data, true);
	AniShader.SendUniformMat4("uProj", CameraProjection.data, true);
	AniShader.SendUniform("interp", aniTimer);
	AniShader.SendUniform("index", index);


	playerOne->draw(AniShader);
	playerTwo->draw(AniShader);

	AniShader.UnBind();*/


	glDisable(GL_CULL_FACE);//should fix random holes in knight

	AniShader.Bind();
	AniShader.SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	AniShader.SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
	players[0]->draw(AniShader, 1);

	AniShader.Bind();
	AniShader.SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	AniShader.SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
	players[1]->draw(AniShader, 1);

	glEnable(GL_CULL_FACE);//turn it back on after for preformance

	drawScore();
	drawTime();

	AniShader.UnBind();
	GBuffer.UnBind();
	GBufferPass.UnBind();

	//===============================================================
//ssao starts here:
	if (useSSAO) {
		ssaoWorkBuffer1.Clear();
		ssaoWorkBuffer2.Clear();
		SSAO.Bind();
		SSAO.SendUniform("uNormalTex", 0);
		SSAO.SendUniform("uTexNoise", 1);
		SSAO.SendUniform("uDepthMap", 2);
		if (FULLSCREEN) {
			SSAO.SendUniform("noiseScale", glm::vec2(FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT));
		}
		else {
			SSAO.SendUniform("noiseScale", glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT));
		}
		for (unsigned int i = 0; i < 16; ++i) {
			SSAO.SendUniform(("samples[" + std::to_string(i) + "]").c_str(), ssaoKernel[i]);
		}
		glm::mat4 ProjBiasMatrix = glm::mat4(
			2.0f, 0.0f, 0.0f, -1.0f,
			0.0f, 2.0f, 0.0f, -1.0f,
			0.0f, 0.0f, 2.0f, -1.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		
		ProjBiasMatrix = ProjBiasMatrix * GameCamera.CameraProjection.GetInverse().matData;

		SSAO.SendUniformMat4("uProjBiasMatrixInverse", glm::value_ptr(ProjBiasMatrix), true);
		SSAO.SendUniformMat4("projection", GameCamera.CameraProjection.data, true);

		ssaoWorkBuffer1.Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
		glActiveTexture(GL_TEXTURE1);
		noiseTexture.Bind();
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, GBuffer.GetDepthHandle());
		DrawFullScreenQuad();

		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		ssaoWorkBuffer1.UnBind();
		SSAO.UnBind();

		//// 3. blur SSAO texture to remove noise
		//ssaoBlurFBO.Bind();
		//glClear(GL_COLOR_BUFFER_BIT);

		//SSAOBlur.Bind();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, ssaoFBO.GetColorHandle(0));
		//DrawFullScreenQuad();
		//SSAOBlur.UnBind();
		//ssaoBlurFBO.UnBind();

	}
	//===============================================================



	/// Detect Edges ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	//Rednering it with the shader
	SobelPass.Bind();
	SobelPass.SendUniform("uNormalMap", 0);
	SobelPass.SendUniform("uDepthMap", 1);
	if (FULLSCREEN)
		SobelPass.SendUniform("uPixelSize", glm::vec2(1.0f / FULLSCREEN_WIDTH, 1.0f / FULLSCREEN_HEIGHT));
	else
		SobelPass.SendUniform("uPixelSize", glm::vec2(1.0f / WINDOW_WIDTH, 1.0f / WINDOW_HEIGHT));

	//Where we are rendering
	EdgeMap.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetDepthHandle());
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	EdgeMap.UnBind();
	SobelPass.UnBind();

	/// Create Scene From GBuffer ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DeferredLighting.Bind();
	DeferredLighting.SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting.SendUniform("uScene", 0);
	DeferredLighting.SendUniform("uShadowMap", 1);
	DeferredLighting.SendUniform("uNormalMap", 2);
	DeferredLighting.SendUniform("uPositionMap", 3);
	DeferredLighting.SendUniform("uEdgeMap", 4);
	DeferredLighting.SendUniform("uStepTexture", 5);
	DeferredLighting.SendUniform("ssao", 10);

	DeferredLighting.SendUniform("LightDirection", glm::vec3(GameCamera.CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	if (darkMode) {
		DeferredLighting.SendUniform("LightAmbient", glm::vec3(0.0f, 0.0f, 0.0f));
	}
	else { DeferredLighting.SendUniform("LightAmbient", glm::vec3(0.6f, 0.6f, 0.6f)); } //You can LERP through colours to make night to day cycles
	DeferredLighting.SendUniform("LightDiffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting.SendUniform("LightSpecular", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting.SendUniform("LightSpecularExponent", 500.0f);
	DeferredLighting.SendUniform("uToonActive", toonActive);
	DeferredLighting.SendUniform("uAmbientOcc", useSSAO);

	DeferredComposite.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, EdgeMap.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE5);
	StepTexture.Bind();
	//ssao
	if (useSSAO) {
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, ssaoWorkBuffer1.GetColorHandle(0));
	}

	DrawFullScreenQuad();

	//draw additional lights
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	PointLight.Bind();
	PointLight.SendUniform("uSceneAlbedo", 0);
	PointLight.SendUniform("uNormalMap", 2);
	PointLight.SendUniform("uPositionMap", 3);

	if (hundredParticleLight) {
		auto end = chrono::steady_clock::now();
		auto start = chrono::steady_clock::now();

		for (int i = 0; i < (int)thousandLights.size(); i++) {
			if (thousandLights[i]->active == true) {
				thousandLights[i]->draw(PointLight, GameCamera.CameraTransform);

				RenderCube();
			}
		}
		end = chrono::steady_clock::now();
		cout << chrono::duration_cast<chrono::milliseconds>(end - start).count() << ":";

	}
	for (int i = 0; i < (int)pointLights.size(); i++) {
		if (pointLights[i]->active == true) {
			PointLight.SendUniform("uRange", 100.0f);
			pointLights[i]->draw(PointLight, GameCamera.CameraTransform);

			DrawFullScreenQuad();
		}
	}

	PointLight.UnBind();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Could I do StepTexture.UnBInd()?
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?


	//draw particles
	ParticleProgram.Bind();
	ParticleProgram.SendUniform("uTex", 0);
	ParticleProgram.SendUniformMat4("uModel", ConfettiEffectBlueRight.transform.data, true);
	ParticleProgram.SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	ParticleProgram.SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);

	//render all particles
	ConfettiEffectRedRight.Render();
	ConfettiEffectRedLeft.Render();
	ConfettiEffectOrangeRight.Render();
	ConfettiEffectOrangeLeft.Render();
	ConfettiEffectPurpleRight.Render();
	ConfettiEffectPurpleLeft.Render();
	ConfettiEffectBlueRight.Render();
	ConfettiEffectBlueLeft.Render();
	basicLeftNet.Render();
	basicRightNet.Render();
	basicLeftNet1.Render();
	basicRightNet1.Render();
	ninjaLeftNet.Render();
	ninjaRightNet.Render();
	ninjaLeftNet1.Render();
	ninjaRightNet1.Render();
	knightLeftNet.Render();
	knightRightNet.Render();
	knightLeftNet1.Render();
	knightRightNet1.Render();
	if (stageVal == 3) {
		NinjaPetals.Render();
		NinjaPetals2.Render();
	}
	DustDashL.Render();
	DustDashR.Render();
	DustLand.Render();
	DustLaunch.Render();
	HitSparkL.Render();
	HitSparkR.Render();
	MeterFlame1.Render();
	MeterFlame2.Render();
	KnightUltFX.Render();
	NinjaUltFX1.Render();
	NinjaUltFX2.Render();

	ParticleProgram.UnBind();


	DeferredComposite.UnBind();
	DeferredLighting.UnBind();

	drawHUD();




		//Black and white
	if (grayscale == true)
	{
		GrayScale.Bind();
		GrayScale.SendUniform("uTex", 0);

		glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		GrayScale.UnBind();
	}
	//Sepia
	if (sepiaActive == true)
	{
		Sepia.Bind();
		Sepia.SendUniform("uTex", 0);

		glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		Sepia.UnBind();
	}
	//negativ
	if (negativeActive == true)
	{
		Negative.Bind();
		Negative.SendUniform("uTex", 0);

		glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		Negative.UnBind();
	}
	/// Compute High Pass ///

	BloomHighPass.Bind();
	BloomHighPass.SendUniform("bloomOn", true);
	BloomHighPass.UnBind();

	if (FULLSCREEN)
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	else
		glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));

	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass.Bind();
	BloomHighPass.SendUniform("uTex", 0);
	if (stageVal == 1)
	{
		BloomHighPass.SendUniform("uThreshold", BLOOM_THRESHOLD1);
		BloomHighPass.SendUniform("uThreshold2", BLOOM_THRESHOLDBRIGHT);
	}
	if (stageVal == 2)
	{
		BloomHighPass.SendUniform("uThreshold", BLOOM_THRESHOLD2);
		BloomHighPass.SendUniform("uThreshold2", BLOOM_THRESHOLDBRIGHT);
	}
	if (stageVal == 3)
	{
		BloomHighPass.SendUniform("uThreshold", BLOOM_THRESHOLD3);
		BloomHighPass.SendUniform("uThreshold2", BLOOM_THRESHOLDBRIGHT);
	}

	WorkBuffer1.Bind();

	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	WorkBuffer1.UnBind();

	BloomHighPass.UnBind();

	/// Compute Blur ///
	if (FULLSCREEN)
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	else
		glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal.Bind();
		BlurHorizontal.SendUniform("uTex", 0);
		if (FULLSCREEN)
			BlurHorizontal.SendUniform("uPixelSize", 1.0f / FULLSCREEN_WIDTH);
		else
			BlurHorizontal.SendUniform("uPixelSize", 1.0f / WINDOW_WIDTH);

		WorkBuffer2.Bind();

		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		WorkBuffer2.UnBind();

		BlurHorizontal.UnBind();

		//Vertical Blur
		BlurVertical.Bind();
		BlurVertical.SendUniform("uTex", 0);
		if (FULLSCREEN)
			BlurVertical.SendUniform("uPixelSize", 1.0f / FULLSCREEN_HEIGHT);
		else
			BlurVertical.SendUniform("uPixelSize", 1.0f / WINDOW_HEIGHT);

		WorkBuffer1.Bind();

		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		WorkBuffer1.UnBind();

		BlurVertical.UnBind();
	}

	//drawHUD();


	/// Composite To Back Buffer ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	BloomComposite.Bind();
	BloomComposite.SendUniform("uScene", 0);
	BloomComposite.SendUniform("uBloom", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	BloomComposite.UnBind();

	glutSwapBuffers();
}

void Game::drawCSS()
{
	/// Clear Buffers ///
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.1f, 0.2f, 0.3f, 0);
	DeferredComposite.Clear();

	glClearColor(0.0f, 0.0f, 0.0f, 0);
	GBuffer.Clear();
	ShadowMap.Clear();
	HudMap.Clear();
	WorkBuffer1.Clear();
	WorkBuffer2.Clear();

	/// Create Scene From GBuffer ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	DeferredLighting.Bind();
	DeferredLighting.SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting.SendUniform("uScene", 0);
	DeferredLighting.SendUniform("uShadowMap", 1);
	DeferredLighting.SendUniform("uNormalMap", 2);
	DeferredLighting.SendUniform("uPositionMap", 3);
	//DeferredLighting.SendUniform("uEdgeMap", 4);
	//DeferredLighting.SendUniform("uStepTexture", 4);

	DeferredLighting.SendUniform("LightDirection", glm::vec3(GameCamera.CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	DeferredLighting.SendUniform("LightAmbient", glm::vec3(0.6f, 0.6f, 0.6f)); //You can LERP through colours to make night to day cycles
	DeferredLighting.SendUniform("LightDiffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting.SendUniform("LightSpecular", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting.SendUniform("LightSpecularExponent", 500.0f);

	DeferredComposite.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));



	DrawFullScreenQuad();

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?

	DeferredComposite.UnBind();
	DeferredLighting.UnBind();

	//===============================================================
		//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
	if (FULLSCREEN)
		gluOrtho2D((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, (float)FULLSCREEN_HEIGHT * 0.5f);//create ortho
	else
		gluOrtho2D((float)WINDOW_WIDTH * -0.5f, (float)WINDOW_WIDTH * 0.5f, (float)WINDOW_HEIGHT * -0.5f, (float)WINDOW_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

//////////////////////////
	//now ready to draw 2d
//////////////////////////
	GBufferPass.Bind();
	hudTransform = Transform::Identity();
	GBufferPass.SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", hudProjection.data, true);

//draws everything in menu
	sortObjects(1);
	for (int i = 0; i < (int)cssObjects.size(); i++) {
		cssObjects[i]->draw(GBufferPass, 1);
	}

	GBufferPass.UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	DeferredComposite.UnBind();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	//===============================================================

	BloomHighPass.Bind();
	BloomHighPass.SendUniform("bloomOn", false);
	BloomHighPass.UnBind();

	/// Compute High Pass ///
	if (FULLSCREEN)
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	else
		glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));
	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass.Bind();
	BloomHighPass.SendUniform("uTex", 0);
	BloomHighPass.SendUniform("uThreshold", 1.0f);
	WorkBuffer1.Bind();
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	WorkBuffer1.UnBind();
	BloomHighPass.UnBind();

	/// Compute Blur ///
	if (FULLSCREEN)
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	else
		glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal.Bind();
		BlurHorizontal.SendUniform("uTex", 0);
		if (FULLSCREEN)
			BlurHorizontal.SendUniform("uPixelSize", 1.0f / FULLSCREEN_WIDTH);
		else
			BlurHorizontal.SendUniform("uPixelSize", 1.0f / WINDOW_WIDTH);
		WorkBuffer2.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer2.UnBind();
		BlurHorizontal.UnBind();

		//Vertical Blur
		BlurVertical.Bind();
		BlurVertical.SendUniform("uTex", 0);
		if (FULLSCREEN)
			BlurVertical.SendUniform("uPixelSize", 1.0f / FULLSCREEN_HEIGHT);
		else
			BlurVertical.SendUniform("uPixelSize", 1.0f / WINDOW_HEIGHT);
		WorkBuffer1.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer1.UnBind();
		BlurVertical.UnBind();
	}

	/// Composite To Back Buffer ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	BloomComposite.Bind();
	BloomComposite.SendUniform("uScene", 0);
	BloomComposite.SendUniform("uBloom", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	BloomComposite.UnBind();

	glutSwapBuffers();
}

void Game::drawSSS()
{
	/// Clear Buffers ///
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.1f, 0.2f, 0.3f, 0);
	DeferredComposite.Clear();

	glClearColor(0.0f, 0.0f, 0.0f, 0);
	GBuffer.Clear();
	ShadowMap.Clear();
	HudMap.Clear();
	WorkBuffer1.Clear();
	WorkBuffer2.Clear();

	/// Create Scene From GBuffer ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	DeferredLighting.Bind();
	DeferredLighting.SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting.SendUniform("uScene", 0);
	DeferredLighting.SendUniform("uShadowMap", 1);
	DeferredLighting.SendUniform("uNormalMap", 2);
	DeferredLighting.SendUniform("uPositionMap", 3);
	//DeferredLighting.SendUniform("uEdgeMap", 4);
	//DeferredLighting.SendUniform("uStepTexture", 4);

	DeferredLighting.SendUniform("LightDirection", glm::vec3(GameCamera.CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	DeferredLighting.SendUniform("LightAmbient", glm::vec3(0.6f, 0.6f, 0.6f)); //You can LERP through colours to make night to day cycles
	DeferredLighting.SendUniform("LightDiffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting.SendUniform("LightSpecular", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting.SendUniform("LightSpecularExponent", 500.0f);

	DeferredComposite.Bind();
		
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));

	DrawFullScreenQuad();

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?

	DeferredComposite.UnBind();
	DeferredLighting.UnBind();


	//===============================================================
		//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
	if (FULLSCREEN)
		gluOrtho2D((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, (float)FULLSCREEN_HEIGHT * 0.5f);//create ortho
	else
		gluOrtho2D((float)WINDOW_WIDTH * -0.5f, (float)WINDOW_WIDTH * 0.5f, (float)WINDOW_HEIGHT * -0.5f, (float)WINDOW_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

//////////////////////////
	//now ready to draw 2d
//////////////////////////
	GBufferPass.Bind();
	hudTransform = Transform::Identity();
	GBufferPass.SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", hudProjection.data, true);

	//draws everything in menu
	sortObjects(2);
	for (int i = 0; i < (int)sssObjects.size(); i++) {
		sssObjects[i]->draw(GBufferPass, 1);
	}

	GBufferPass.UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	DeferredComposite.UnBind();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	

	BloomHighPass.Bind();
	BloomHighPass.SendUniform("bloomOn", false);
	BloomHighPass.UnBind();

	/// Compute High Pass ///
	if (FULLSCREEN)
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	else
		glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));
	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass.Bind();
	BloomHighPass.SendUniform("uTex", 0);
	BloomHighPass.SendUniform("uThreshold", 1.0f);
	WorkBuffer1.Bind();
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	WorkBuffer1.UnBind();
	BloomHighPass.UnBind();

	/// Compute Blur ///
	if (FULLSCREEN)
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	else
		glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal.Bind();
		BlurHorizontal.SendUniform("uTex", 0);
		if (FULLSCREEN)
			BlurHorizontal.SendUniform("uPixelSize", 1.0f / FULLSCREEN_WIDTH);
		else
			BlurHorizontal.SendUniform("uPixelSize", 1.0f / WINDOW_WIDTH);
		WorkBuffer2.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer2.UnBind();
		BlurHorizontal.UnBind();

		//Vertical Blur
		BlurVertical.Bind();
		BlurVertical.SendUniform("uTex", 0);
		if (FULLSCREEN)
			BlurVertical.SendUniform("uPixelSize", 1.0f / FULLSCREEN_HEIGHT);
		else
			BlurVertical.SendUniform("uPixelSize", 1.0f / WINDOW_HEIGHT);
		WorkBuffer1.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer1.UnBind();
		BlurVertical.UnBind();
	}

	/// Composite To Back Buffer ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	BloomComposite.Bind();
	BloomComposite.SendUniform("uScene", 0);
	BloomComposite.SendUniform("uBloom", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	BloomComposite.UnBind();


	glutSwapBuffers();
}

void Game::drawHUD()
{
	//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
	//gluOrtho2D(0.0, 1.0, 0.0, 1.0);//create ortho
	if (FULLSCREEN)
		gluOrtho2D((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, (float)FULLSCREEN_HEIGHT * 0.5f);//create ortho
	else
		gluOrtho2D((float)WINDOW_WIDTH * -0.5f, (float)WINDOW_WIDTH * 0.5f, (float)WINDOW_HEIGHT * -0.5f, (float)WINDOW_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

	//////////////////////////
	//now ready to draw 2d
	//////////////////////////
	GBufferPass.Bind();
	hudTransform = Transform::Identity();
	//hudTransform.Translate(vec3(WINDOW_WIDTH * -0.5f, WINDOW_HEIGHT* -0.5f, 0));
	GBufferPass.SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", hudProjection.data, true);

	//Draw Player 1 HUD
	///draw quad for p1 pic
	Transform hudLoc = Transform::Identity();
	if (FULLSCREEN) {
		hudLoc.Scale(150.0f);
		hudLoc.Translate(glm::vec3(-675, -540, 0));
	}
	else {
		hudLoc.Scale(100.0f);
		hudLoc.Translate(glm::vec3(-450, -360, 0));
	}
	hudLoc.RotateY(90.0f);
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	P1Hud.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	P1Hud.UnBind();

	///draw quad for p1 bar
	hudLoc = Transform::Identity();

	if (FULLSCREEN) {
		hudLoc.Scale(glm::vec3(150.0f * (players[0]->getMeter() / 200.0f), 150.0f, 150.0f));
		hudLoc.Translate(glm::vec3(-675 - 1.05*(200.0f - players[0]->getMeter()), -540, 0));
	}
	else {
		hudLoc.Scale(glm::vec3(100.0f * (players[0]->getMeter() / 200.0f), 100.0f, 100));
		hudLoc.Translate(glm::vec3(-450 - 0.7*(200.0f - players[0]->getMeter()), -360, 0));
	}
	hudLoc.RotateY(90.0f);
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	P1Bar.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	P1Bar.UnBind();

	if (isNinja1)
	{
		///draw quad for p1 lines
		hudLoc = Transform::Identity();

		if (FULLSCREEN) {
			hudLoc.Scale(glm::vec3(150.0f, 150.0f, 150.0f));
			hudLoc.Translate(glm::vec3(-675, -540, 0));
		}
		else {
			hudLoc.Scale(glm::vec3(100.0f, 100.0f, 100));
			hudLoc.Translate(glm::vec3(-450, -360, 0));
		}
		hudLoc.RotateY(90.0f);
		GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

		P1Line.Bind();
		glBindVertexArray(HudObj.VAO);
		glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
		P1Line.UnBind();
	}

	//Draw Player 2 HUD
	///draw quad for p2 pic
	hudLoc = Transform::Identity();
	if (FULLSCREEN) {
		hudLoc.Scale(150.0f);
		hudLoc.Translate(glm::vec3(675, -540, 0));
	}
	else {
		hudLoc.Scale(100.0f);
		hudLoc.Translate(glm::vec3(450, -360, 0));
	}
	hudLoc.RotateY(90.0f);
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	P2Hud.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	P2Hud.UnBind();

	///draw quad for p2 bar
	hudLoc = Transform::Identity();
	if (FULLSCREEN) {
		hudLoc.Scale(glm::vec3(150.0f * (players[1]->getMeter() / 200.0f), 150.0f, 150.0f));
		hudLoc.Translate(glm::vec3(675 + 1.05*(200.0f - players[1]->getMeter()), -540, 0));
	}
	else {
		hudLoc.Scale(glm::vec3(100.0f * (players[1]->getMeter() / 200.0f), 100.0f, 100));
		hudLoc.Translate(glm::vec3(450 + 0.7*(200.0f - players[1]->getMeter()), -360, 0));
	}
	hudLoc.RotateY(90.0f);
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	P2Bar.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	P2Bar.UnBind();

	if (isNinja2)
	{
		///draw quad for p2 line
		hudLoc = Transform::Identity();
		if (FULLSCREEN) {
			hudLoc.Scale(glm::vec3(150.0f));
			hudLoc.Translate(glm::vec3(675, -540, 0));
		}
		else {
			hudLoc.Scale(glm::vec3(100.0f));
			hudLoc.Translate(glm::vec3(450, -360, 0));
		}
		hudLoc.RotateY(90.0f);
		GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

		P2Line.Bind();
		glBindVertexArray(HudObj.VAO);
		glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
		P2Line.UnBind();
	}

	GBufferPass.UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	//DeferredComposite.UnBind();
	DeferredComposite.UnBind();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
}

void Game::drawScore() {

	GBuffer.Bind();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	//glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	GBufferPass.Bind();
	GBufferPass.SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
	///score
	Transform hudLoc = Transform::Identity();
	hudLoc.Scale(2.0f);
	hudLoc.RotateX(-8.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(-0.8f, 3.1f, -12.52f));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);
	if (players[1]->action == players[0]->ACTION_IN_NET)
		time[rand() % 10]->Bind();
	else
		time[score1 % 10]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	///score
	/*hudLoc = Transform::Identity();
	hudLoc.Scale(2.0f);
	hudLoc.RotateX(-8.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(2.2f, 3.1f, -12.51f));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);
	time[10]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());*/
	///score
	hudLoc = Transform::Identity();
	hudLoc.Scale(2.0f);
	hudLoc.RotateX(-8.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(5.2f, 3.1f, -12.5f));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);
	if (players[0]->action == players[1]->ACTION_IN_NET)
		time[rand() % 10]->Bind();
	else
		time[score2 % 10]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());


	//glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

	GBufferPass.UnBind();
	GBuffer.UnBind();
}

void Game::drawMenu()
{
	/// Clear Buffers ///
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.1f, 0.2f, 0.3f, 0);
	DeferredComposite.Clear();

	glClearColor(0.0f, 0.0f, 0.0f, 0);
	GBuffer.Clear();
	ShadowMap.Clear();
	HudMap.Clear();
	WorkBuffer1.Clear();
	WorkBuffer2.Clear();

	/// Create Scene From GBuffer ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	DeferredLighting.Bind();
	DeferredLighting.SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting.SendUniform("uScene", 0);
	DeferredLighting.SendUniform("uShadowMap", 1);
	DeferredLighting.SendUniform("uNormalMap", 2);
	DeferredLighting.SendUniform("uPositionMap", 3);
	//DeferredLighting.SendUniform("uEdgeMap", 4);
	//DeferredLighting.SendUniform("uStepTexture", 4);

	DeferredLighting.SendUniform("LightDirection", glm::vec3(GameCamera.CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	DeferredLighting.SendUniform("LightAmbient", glm::vec3(0.6f, 0.6f, 0.6f)); //You can LERP through colours to make night to day cycles
	DeferredLighting.SendUniform("LightDiffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting.SendUniform("LightSpecular", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting.SendUniform("LightSpecularExponent", 500.0f);

	DeferredComposite.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));



	DrawFullScreenQuad();

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?

	DeferredComposite.UnBind();
	DeferredLighting.UnBind();

//===============================================================
	//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
	if (FULLSCREEN)
		gluOrtho2D((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, (float)FULLSCREEN_HEIGHT * 0.5f);//create ortho
	else
		gluOrtho2D((float)WINDOW_WIDTH * -0.5f, (float)WINDOW_WIDTH * 0.5f, (float)WINDOW_HEIGHT * -0.5f, (float)WINDOW_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

	//////////////////////////
	//now ready to draw 2d
	//////////////////////////
	GBufferPass.Bind();
	hudTransform = Transform::Identity();
	GBufferPass.SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", hudProjection.data, true);

	//draws everything in menu
	sortObjects(0);
	for (int i = 0; i < (int)menuObjects.size(); i++) {
		menuObjects[i]->draw(GBufferPass, 1);
	}

	GBufferPass.UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	//DeferredComposite.UnBind();
	DeferredComposite.UnBind();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

//===============================================================

	BloomHighPass.Bind();
	BloomHighPass.SendUniform("bloomOn", false);
	BloomHighPass.UnBind();

	/// Compute High Pass ///
	if (FULLSCREEN)
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	else
		glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));

	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass.Bind();
	BloomHighPass.SendUniform("uTex", 0);
	BloomHighPass.SendUniform("uThreshold", 1.0f);

	WorkBuffer1.Bind();

	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	WorkBuffer1.UnBind();

	BloomHighPass.UnBind();

	/// Compute Blur ///
	if (FULLSCREEN)
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	else
		glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal.Bind();
		BlurHorizontal.SendUniform("uTex", 0);
		if (FULLSCREEN)
			BlurHorizontal.SendUniform("uPixelSize", 1.0f / FULLSCREEN_WIDTH);
		else
			BlurHorizontal.SendUniform("uPixelSize", 1.0f / WINDOW_WIDTH);

		WorkBuffer2.Bind();

		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		WorkBuffer2.UnBind();

		BlurHorizontal.UnBind();

		//Vertical Blur
		BlurVertical.Bind();
		BlurVertical.SendUniform("uTex", 0);
		if (FULLSCREEN)
			BlurVertical.SendUniform("uPixelSize", 1.0f / FULLSCREEN_HEIGHT);
		else
			BlurVertical.SendUniform("uPixelSize", 1.0f / WINDOW_HEIGHT);

		WorkBuffer1.Bind();

		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		WorkBuffer1.UnBind();

		BlurVertical.UnBind();
	}

	//drawHUD();


	/// Composite To Back Buffer ///
	if (FULLSCREEN)
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	else
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	BloomComposite.Bind();
	BloomComposite.SendUniform("uScene", 0);
	BloomComposite.SendUniform("uBloom", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	BloomComposite.UnBind();


	glutSwapBuffers();
}

void Game::drawTime()
{
	GBuffer.Bind();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	//glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	GBufferPass.Bind();
	GBufferPass.SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);

	int timer = 300 - (int)TotalGameTime;
	if (timer < 0) timer = 0;
	int min = (int)timer / 60;
	int secT = ((int)timer % 60) / 10;
	int secO = ((int)timer % 60) % 10;
	if (min > 10) min = 10;
	if (secT >= 6) min = 10;
	if (secO > 10) min = 10;

	//Draw Time
	///min
	Transform hudLoc;
	hudLoc.Scale(2.0f);
	hudLoc.RotateX(14.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(-1.9f, 15.5f, -16.53f));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	time[min]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	///speerate
	hudLoc = Transform();
	hudLoc.Scale(2.0f);
	hudLoc.RotateX(14.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(0.1f, 15.5f, -16.52f));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	time[10]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	///sec tens
	hudLoc = Transform();
	hudLoc.Scale(2.0f);
	hudLoc.RotateX(14.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(2.1f, 15.5f, -16.51f));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	time[secT]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	///sec ones
	hudLoc = Transform();
	hudLoc.Scale(2.0f);
	hudLoc.RotateX(14.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(5.1f, 15.5f, -16.5f));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	time[secO]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	//unbind last used texture
	time[secO]->UnBind();

	//glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

	GBufferPass.UnBind();
	GBuffer.UnBind();
}

void Game::keyboardDown(unsigned char key, int mouseX, int mouseY)
{
	//std::cout << key << " : " << (int)key << std::endl;
	switch (key)
	{
	case 27: // the escape key
		exit(0);
		break;
	case 'q': 
		
		break;
	case 'w': //w
		//inputs[0] = true;
		break;
	case 'd': //d
		//inputs[1] = true;
		break;
	case 's': //s
		//inputs[2] = true;
		break;
	case 'a': //a
		//inputs[3] = true;
		break;
	case 'v': //n
		if (darkMode) {
			darkMode = false;
		}
		else {
			darkMode = true;
		}
		DeferredLighting.ReloadShader();
		break;
	case 'b': //m
		//inputs[5] = true;
		break;
	case 'i': //up
		//inputs2[0] = true;
		break;
	case 'k': //down
		//inputs2[2] = true;
		break;
	case 'j': //left
		//inputs2[3] = true;
		break;
	case '.': //a
		//inputs2[4] = true;
		break;
	case '/': //b
		//inputs2[5] = true;
		break;
	case 'h': //h
		if (hundredParticleLight) {
			hundredParticleLight = false;
		}
		else {
			hundredParticleLight = true;
		}
		PointLight.ReloadShader();
		break;
	case 'o': //h
		if (useSSAO) {
			useSSAO = false;
		}
		else {
			useSSAO = true;
		}
		SSAO.ReloadShader();
		SSAOBlur.ReloadShader();
		DeferredLighting.ReloadShader();
		break;

	}
}

void Game::keyboardUp(unsigned char key, int mouseX, int mouseY)
{
	switch(key)
	{
	case 'R': //w
	case 'r': //w
		if (scene == 3) {
			scene = 3;
			gameDone = false;
			score1 = 0;
			score2 = 0;
			players[0]->respawn();
			players[0]->setPosition(glm::vec3(-5, 0, 0));
			players[0]->facingRight = true;
			players[1]->respawn();
			players[1]->setPosition(glm::vec3(5, 0, 0));
			players[1]->facingRight = false;
			//updateTimer = new Timer();
			TotalGameTime = 0.0f;
			lastInputTime = 0.0f;
		}
		else {
			//players[0] = (new Knight("./Assets/Models/Knight.obj", "./Assets/Textures/player1.png"));
			//players[1] = (new Ninja("./Assets/Models/Knight.obj", "./Assets/Textures/player2.png"));
			p1Char = 1;
			p2Char = 2;
			players[0] = new Knight(knightTemp);
			players[1] = new Ninja(ninjaTemp);
			players[1]->bodyTexture.Load("./Assets/Textures/player2ninja.png");
			stageVal = 1;
			//hide ninja court
			findObjects(3, "ninja_court")->hide = true;
			for (int i = 0; i < (int)ninja_court_objs.size(); i++)
				findObjects(3, ninja_court_objs[i])->hide = true;
			//hide knight court
			findObjects(3, "knight_court")->hide = true;
			for (int i = 0; i < (int)knight_court_objs.size(); i++)
				findObjects(3, knight_court_objs[i])->hide = true;
			//show default court
			findObjects(3, "default_court")->hide = false;
			for (int i = 0; i < (int)default_court_objs.size(); i++)
				findObjects(3, default_court_objs[i])->hide = false;
			scene = 3;
			gameDone = false;
			score1 = 0;
			score2 = 0;
			players[0]->respawn();
			players[0]->setPosition(glm::vec3(-5, 0, 0));
			players[0]->facingRight = true;
			players[1]->respawn();
			players[1]->setPosition(glm::vec3(5, 0, 0));
			players[1]->facingRight = false;
			//updateTimer = new Timer();
			TotalGameTime = 0.0f;
			lastInputTime = 0.0f;
		}
		break;
	case 'Q': //w
	case 'q': //w
		stageDone = false;
		stageVal = 1;
		scene = 0;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Done = false;
		p2Done = false;
		p1Char = 0;
		p2Char = 0;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
		break;
	case 'W': //w
	case 'w': //w
		stageDone = false;
		stageVal = 1;
		scene = 1;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Done = false;
		p2Done = false;
		p1Char = 0;
		p2Char = 0;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;

		break;
	case 'E': //w
	case 'e': //w
		scene = 2;
		stageDone = false;
		stageVal = 1;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Done = false;
		p2Done = false;
		p1Char = 1;
		p2Char = 1;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;

		break;
	case 'd': //d
		//inputs[1] = false;
		stageDone = false;
		stageVal = 1;
		scene = 4;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Done = false;
		p2Done = false;
		p1Char = 0;
		p2Char = 0;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
		break;
	case 's': //s
		//inputs[2] = false;
		break;
	case 'a': //a
		//inputs[3] = false;
		break;
	case 'v': //n
		//inputs[4] = false;
		break;
	case 'b': //m
		if (grayscale == false)
			grayscale = true;
		else if (grayscale == true)
			grayscale = false;
		break;
	case 'i': //up
		//inputs2[0] = false;
		break;
	case 'l': //right
		//inputs2[1] = false;
		break;
	case 'k': //down
		//inputs2[2] = false;
		break;
	case 'j': //left
		//inputs2[3] = false;
		break;
	case 't':
		if (toonActive)
		{
			toonActive = false;
			std::cout << "toon OFF" << std::endl;
		}
		else if (!toonActive)
		{
			toonActive = true;
			std::cout << "toon ON" << std::endl;
		}
		break;
	case '.': //a
		//GBufferPass.ReloadShader();
		//AdShader.ReloadShader();
		//BloomHighPass.ReloadShader();
		PointLight.ReloadShader();
		//NetShader.ReloadShader();
		//DeferredLighting.ReloadShader();
		std::cout << "Reloaded Shaders\n";
		//inputs2[4] = false;
		break;
	case '/': //b
		//inputs2[5] = false;
		break;
	case 'm':
		for (int i = 0; i < 2; i++)
			players[i]->setMeter(100);
		break;
	case 'g':
		if (sepiaActive == false)
			sepiaActive = true;
		else if (sepiaActive == true)
			sepiaActive = false;
		break;
	case 'n':
		if (negativeActive == false)
			negativeActive = true;
		else if (negativeActive == true)
			negativeActive = false;
		break;
	}
}

void Game::mouseClicked(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN) 
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:

			break;
		case GLUT_RIGHT_BUTTON:
		
		    
			break;
		case GLUT_MIDDLE_BUTTON:

			break;
		}
	}
	else
	{

	}
}

/*
 * mouseMoved(x,y)
 * - this occurs only when the mouse is pressed down
 *   and the mouse has moved.  you are given the x,y locations
 *   in window coordinates (from the top left corner) and thus 
 *   must be converted to screen coordinates using the screen to window pixels ratio
 *   and the y must be flipped to make the bottom left corner the origin.
 */
void Game::mouseMoved(int x, int y)
{
}

//Random number generator to determine sound to be played
int Game::RandomNum(int max)
{
	//Max number is actually max-1
	int temp = rand() % max;
	return temp;
}

void Game::updateInputs()
{

	//Controler Ready.
	//Keep last frames inputs saved for comparing, to track ButtonPress, and ButtonRelease.
	//if was pressed las frame but not this frame, ButtonRelease() == true
	//if  not pressed last frame, but is pressed this frame, ButtonPress() == true
	Aold = Anew;
	Bold = Bnew;
	//Yold = Ynew;
	Xold = Xnew;
	Sold = Snew;
	LBold = LBnew;
	RBold = RBnew;
	LTold = LTnew;
	RTold = RTnew;
	//Update inputs
	XBoxController.DownloadPackets(2);

	if (XBoxController.GetConnected(0)) {
		//Allow Release and Press
		if (XBoxController.GetButton(0, Input::A)) {
			Anew = true;
			pad = true;
		}
		else Anew = false;
		if (XBoxController.GetButton(0, Input::B)) {
			Bnew = true;
			pad = true;
		}
		else Bnew = false;
		if (XBoxController.GetButton(0, Input::Start)) {
			Snew = true;
			pad = true;
		}
		else Snew = false;
		/*if (XBoxController.GetButton(0, Input::Y)) {
			Ynew = true;
			pad = true;
		}
		else Ynew = false;*/
		if (XBoxController.GetButton(0, Input::X)) {
			Xnew = true;
			pad = true;
		}
		else Xnew = false;
		if (XBoxController.GetButton(0, Input::RB)) {
			RBnew = true;
			pad = true;
		}
		else RBnew = false;
		if (XBoxController.GetButton(0, Input::LB)) {
			LBnew = true;
			pad = true;
		}
		else LBnew = false;
		/*if (XBoxController.GetButton(0, Input::Start)) {
			Snew = true;
			pad = true;
		}
		else Snew = false;*/

		XBoxController.GetTriggers(0, lTrig, rTrig);
		if (rTrig > 0.5) {
			RTnew = true;
			pad = true;
		}
		else RTnew = false;
		if (lTrig > 0.5) {
			LTnew = true;
			pad = true;
		}
		else LTnew = false;


		Input::Stick lStick, rStick;
		XBoxController.GetSticks(0, lStick, rStick);
		if ((lStick.xAxis <= -0.45 || lStick.xAxis >= 0.45))
			pad = true;

		//pressed
		if (lStick.xAxis >= 0.9)//if key held
		{
			inputs[7] = true;
		}
		if (lStick.xAxis <= -0.9)//if key held
		{
			inputs[8] = true;
		}
		if (lStick.xAxis >= 0.45)//if key held
			inputs[1] = true;
		if (lStick.xAxis <= -0.45)//if key held
			inputs[3] = true;
		if (lStick.yAxis >= 0.7)//if key held
			inputs[0] = true;
		if (lStick.yAxis <= -0.7)//if key held
			inputs[2] = true;
		if (Xnew == true && Xold == false)
			inputs[4] = true;
		/*if (Ynew == true && Yold == false)
			inputs[5] = true;*/
		if (Snew == true && Sold == false)
			inputs[5] = true;
		if (Anew == true && Aold == false)
			inputs[6] = true;
		else inputs[6] = false;//A = only on pressed not held

		//ultimate
		if ((LBnew && RBnew) && !(LBold && RBold))
			inputs[9] = true;
		else
			inputs[9] = false;

		//dash
		if (LTnew || RTnew)
			inputs[10] = true;
		else
			inputs[10] = false;

		//release
		if (lStick.xAxis < 0.9)//if key held
		{
			inputs[7] = false;
		}
		if (lStick.xAxis > -0.9)//if key held
		{
			inputs[8] = false;
		}
		if (lStick.xAxis < 0.45)//if key held
			inputs[1] = false;
		if (lStick.xAxis > -0.45)//if key held
			inputs[3] = false;
		if (lStick.yAxis < 0.7)//if key held
			inputs[0] = false;
		if (lStick.yAxis > -0.7)//if key held
			inputs[2] = false;
		if (Xnew == false && Xold == true)
			inputs[4] = false;
		/*if (Ynew == false && Yold == true)
			inputs[5] = false;*/
		if (Snew == false && Sold == true)
			inputs[5] = false;
		if (Anew == false && Aold == true)
			inputs[6] = false;

	}
	else {

		inputs = { false, false, false, false, false, false, false, false, false, false, false }; //up, left, down, right, X, Y, A, LeftFull, RightFull, R, B
	}

	//PLAYER 2

	//Controler Ready.
	//Keep last frames inputs saved for comparing, to track ButtonPress, and ButtonRelease.
	//if was pressed las frame but not this frame, ButtonRelease() == true
	//if  not pressed last frame, but is pressed this frame, ButtonPress() == true
	Aold2 = Anew2;
	Bold2 = Bnew2;
	//Yold2 = Ynew2;
	Xold2 = Xnew2;
	Sold2 = Snew2;
	LBold2 = LBnew2;
	RBold2 = RBnew2;
	LTold2 = LTnew2;
	RTold2 = RTnew2;

	if (XBoxController.GetConnected(1)) {
		//Allow Release and Press
		if (XBoxController.GetButton(1, Input::A)) {
			Anew2 = true;
			pad = true;
		}
		else Anew2 = false;
		if (XBoxController.GetButton(1, Input::B)) {
			Bnew2 = true;
			pad = true;
		}
		else Bnew2 = false;
		if (XBoxController.GetButton(1, Input::Start)) {
			Snew2 = true;
			pad = true;
		}
		else Snew2 = false;
		/*if (XBoxController.GetButton(1, Input::Y)) {
			Ynew2 = true;
			pad = true;
		}
		else Ynew2 = false;*/
		if (XBoxController.GetButton(1, Input::X)) {
			Xnew2 = true;
			pad = true;
		}
		else Xnew2 = false;
		if (XBoxController.GetButton(1, Input::RB)) {
			RBnew2 = true;
			pad = true;
		}
		else RBnew2 = false;
		if (XBoxController.GetButton(1, Input::LB)) {
			LBnew2 = true;
			pad = true;
		}
		else LBnew2 = false;

		XBoxController.GetTriggers(1, lTrig2, rTrig2);
		if (rTrig2 > 0.5f) {
			RTnew2 = true;
			pad = true;
		}
		else RTnew2 = false;
		if (lTrig2 > 0.5f) {
			LTnew2 = true;
			pad = true;
		}
		else LTnew2 = false;


		Input::Stick lStick2, rStick2;
		XBoxController.GetSticks(1, lStick2, rStick2);
		if ((lStick2.xAxis <= -0.45 || lStick2.xAxis >= 0.45))
			pad = true;


		//pressed
		if (lStick2.xAxis >= 0.9)//if key held
		{
			inputs2[7] = true;
		}
		if (lStick2.xAxis <= -0.9)//if key held
		{
			inputs2[8] = true;
		}
		if (lStick2.xAxis >= 0.45)//if key held
			inputs2[1] = true;
		if (lStick2.xAxis <= -0.45)//if key held
			inputs2[3] = true;
		if (lStick2.yAxis >= 0.7)//if key held
			inputs2[0] = true;
		if (lStick2.yAxis <= -0.7)//if key held
			inputs2[2] = true;
		if (Xnew2 == true && Xold2 == false)
			inputs2[4] = true;
		/*if (Ynew2 == true && Yold2 == false)
			inputs2[5] = true;*/
		if (Snew2 == true && Sold2 == false)
			inputs2[5] = true;
		if (Anew2 == true && Aold2 == false)
			inputs2[6] = true;
		else inputs2[6] = false;//A = only on pressed not held

		//ultimate
		if ((LBnew2 && RBnew2) && !(LBold2 && RBold2))
			inputs2[9] = true;
		else
			inputs2[9] = false;

		//dash
		if (LTnew2 || RTnew2)
			inputs2[10] = true;
		else
			inputs2[10] = false;

		//released
		if (lStick2.xAxis < 0.9)//if key held
		{
			inputs2[7] = false;
		}
		if (lStick2.xAxis > -0.9)//if key held
		{
			inputs2[8] = false;
		}
		if (lStick2.xAxis < 0.45)//if key held
			inputs2[1] = false;
		if (lStick2.xAxis > -0.45)//if key held
			inputs2[3] = false;
		if (lStick2.yAxis < 0.7)//if key held
			inputs2[0] = false;
		if (lStick2.yAxis > -0.7)//if key held
			inputs2[2] = false;
		if (Xnew2 == false && Xold2 == true)
			inputs2[4] = false;
		/*if (Ynew2 == false && Yold2 == true)
			inputs2[5] = false;*/
		if (Snew2 == false && Sold2 == true)
			inputs2[5] = false;
		if (Anew2 == false && Aold2 == true)
			inputs2[6] = false;
	}
	else {


		inputs2 = { false, false, false, false, false, false, false, false, false, false, false }; //up, left, down, right, X, Y, A, LeftFull, RightFull, R, B
	}
}

void Game::loadTime() {
	
	for (int i = 0; i <= 10; i++)
	{
		Texture * temp = new Texture();
		if (!temp->Load("./Assets/Textures/" + std::to_string(i) + ".png"))
		{
			std::cout << "Time Texture failed to load.\n";
			system("pause");
			exit(0);
		}
		time.push_back(temp);
	}
}

//clears all particles
void Game::clearParticles()
{
	ConfettiEffectBlueLeft.Clear();
	ConfettiEffectBlueRight.Clear();
	ConfettiEffectOrangeLeft.Clear();
	ConfettiEffectOrangeRight.Clear();
	ConfettiEffectPurpleLeft.Clear();
	ConfettiEffectPurpleRight.Clear();
	ConfettiEffectRedLeft.Clear();
	ConfettiEffectRedRight.Clear();
	NinjaPetals.Clear();
	NinjaPetals.Clear();

}

void Game::sortObjects(unsigned int scene) {
	if (scene == 3) {
		//go through list
		for (int i = 0; i < (int)(gameObjects.size() - 1); i++) {
			for (int j = i + 1; j < (int)gameObjects.size(); j++) {

				float dist1 = (float)(GameCamera.CameraTransform.GetTranslation() - gameObjects[i]->transform.GetTranslation()).length();
				float dist2 = (float)(GameCamera.CameraTransform.GetTranslation() - gameObjects[j]->transform.GetTranslation()).length();

				if ((dist1 < dist2 && gameObjects[i]->blending) || (gameObjects[i]->blending && !gameObjects[j]->blending)) {
					Object* temp = gameObjects[i];
					gameObjects[i] = gameObjects[j];
					gameObjects[j] = temp;
				}

			}
		}
	}
	else if (scene == 0){
		//go through list
		for (int i = 0; i < (int)(menuObjects.size() - 1); i++) {
			for (int j = i + 1; j < (int)menuObjects.size(); j++) {

				float dist1 = (float)(GameCamera.CameraTransform.GetTranslation() - menuObjects[i]->transform.GetTranslation()).length();
				float dist2 = (float)(GameCamera.CameraTransform.GetTranslation() - menuObjects[j]->transform.GetTranslation()).length();

				if ((dist1 < dist2 && menuObjects[i]->blending) || (menuObjects[i]->blending && !menuObjects[j]->blending)) {
					Object* temp = menuObjects[i];
					menuObjects[i] = menuObjects[j];
					menuObjects[j] = temp;
				}

			}
		}
	}
	else if (scene == 1) {
		//go through list
		for (int i = 0; i < (int)(cssObjects.size() - 1); i++) {
			for (int j = i + 1; j < (int)cssObjects.size(); j++) {

				float dist1 = (float)(GameCamera.CameraTransform.GetTranslation() - cssObjects[i]->transform.GetTranslation()).length();
				float dist2 = (float)(GameCamera.CameraTransform.GetTranslation() - cssObjects[j]->transform.GetTranslation()).length();

				if ((dist1 < dist2 && cssObjects[i]->blending) || (cssObjects[i]->blending && !cssObjects[j]->blending)) {
					Object* temp = cssObjects[i];
					cssObjects[i] = cssObjects[j];
					cssObjects[j] = temp;
				}

			}
		}
	}
	else if (scene == 2) {
		//go through list
		for (int i = 0; i < (int)(sssObjects.size() - 1); i++) {
			for (int j = i + 1; j < (int)sssObjects.size(); j++) {

				float dist1 = (float)(GameCamera.CameraTransform.GetTranslation() - sssObjects[i]->transform.GetTranslation()).length();
				float dist2 = (float)(GameCamera.CameraTransform.GetTranslation() - sssObjects[j]->transform.GetTranslation()).length();

				if ((dist1 < dist2 && sssObjects[i]->blending) || (sssObjects[i]->blending && !sssObjects[j]->blending)) {
					Object* temp = sssObjects[i];
					sssObjects[i] = sssObjects[j];
					sssObjects[j] = temp;
				}

			}
		}
	}
}

Object* Game::findObjects(unsigned int scene, std::string _name)
{
	if (scene == 3) {
		for (int i = 0; i < (int)gameObjects.size(); i++) {

			if (gameObjects[i]->name == _name)
				return gameObjects[i];

		}
	}
	else if (scene == 0){
		for (int i = 0; i < (int)menuObjects.size(); i++) {

			if (menuObjects[i]->name == _name)
				return menuObjects[i];

		}
	}
	else if (scene == 1) {
		for (int i = 0; i < (int)cssObjects.size(); i++) {

			if (cssObjects[i]->name == _name)
				return cssObjects[i];

		}
	}
	else if (scene == 2) {
		for (int i = 0; i < (int)sssObjects.size(); i++) {

			if (sssObjects[i]->name == _name)
				return sssObjects[i];

		}
	}
	return nullptr;
}

PointLightObj* Game::findLight(std::string _name)
{
	for (int i = 0; i < (int)pointLights.size(); i++) {

		if (pointLights[i]->name == _name)
			return pointLights[i];

	}
	return nullptr;
}