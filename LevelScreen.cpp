// Pyroteknix
// Developed as coursework for Abertay University
// Based on framework by Dr. Henry Fortuna
// Copyright Sarah Herzog, 2013, all rights reserved.
//
// LevelScreen
//      Screen containing a game level


// |----------------------------------------------------------------------------|
// |								Includes									|
// |----------------------------------------------------------------------------|
#include "LevelScreen.h"


// |----------------------------------------------------------------------------|
// |							   Constructor									|
// |----------------------------------------------------------------------------|
LevelScreen::LevelScreen() :
    Screen(),
	m_crosshair(0),
	m_aimSpeed(10),
	m_cannon(0),
	m_firework(0),
	m_budget(0),
	m_costCross(0),
	m_cooldown(0.0f),
	m_accumulatedTime(0.0f),
	m_budgetDisplay(0)
{
	Debug ("LevelScreen: object instantiated.");
}
     

// |----------------------------------------------------------------------------|
// |							  Copy Constructor								|
// |----------------------------------------------------------------------------|
LevelScreen::LevelScreen(const LevelScreen&) {
	Debug ("LevelScreen: object copied.");
}


// |----------------------------------------------------------------------------|
// |							   Destructor									|
// |----------------------------------------------------------------------------|
LevelScreen::~LevelScreen() {
	Debug ("LevelScreen: object destroyed.");
}


// |----------------------------------------------------------------------------|
// |							   Initialize									|
// |----------------------------------------------------------------------------|
bool LevelScreen::Initialize() {
	Debug ("LevelScreen::Initialize() called.");

    // Initialize parent class
    Screen::Initialize();
	
	// Create game object for cannon
	m_cannon = new GameObject;
	m_cannon->SetModel("cannon");
	m_cannon->SetTexture("cannon");
	m_cannon->SetPosition(Coord(0.0f,0.0f,0.0f));
	m_cannon->SetOrientation(Coord(-3.14f/4.0f,0.0f,0.0f));
	m_gameObjects.push_back(m_cannon);
	
	// Create game object for base
	GameObject* base = new GameObject;
	base->SetModel("base");
	base->SetTexture("base");
	base->SetPosition(Coord(0.0f,0.0f,0.0f));
	m_gameObjects.push_back(base);
	
	// Create crosshair
	m_crosshair = new Image2D;
	m_crosshair->SetModel("quad");
	m_crosshair->SetTexture("crosshair");
	m_crosshair->SetX(0);
	m_crosshair->SetY(0);
	m_crosshair->SetWidth(32);
	m_crosshair->SetHeight(32);
	m_crosshair->SetDepth(0xFFFFF0);
	m_crosshair->SetTint(0x80,0x00,0x00,0x80);
	m_overlayObjects.push_back(m_crosshair);
	
	// Firework TEST
	m_firework = new Firework;
	m_firework->Initialize();
	m_firework->SetTarget(Coord(0.0f,250.0f,-150.0f));
	m_gameObjects.push_back(m_firework);
	
	// Firework costs setup
	m_budget = 2500;
	m_costCross = 60;
	m_cooldown = 1.0f;
	m_accumulatedTime = 0.0f;
	m_budgetDisplay = new Text;
	m_budgetDisplay->SetFont("defaultFont");
	m_budgetDisplay->SetTexture("defaultFont");
	char buffer [50];
	sprintf (buffer, "Budget: $%d", m_budget);
	m_budgetDisplay->SetString(buffer);
	m_budgetDisplay->SetPosition(Coord(150.0f,170.0f,0.0f));
	m_overlayObjects.push_back(m_budgetDisplay);
	
	// Cost display
	Image2D* costs = new Image2D;
	costs->SetModel("quad");
	costs->SetTexture(AssetManager::GetSingleton().GetBlackTransparentTexture("controls"));
	costs->SetX(-250);
	costs->SetY(170);
	costs->SetWidth(128);
	costs->SetHeight(128);
	costs->SetDepth(0xFFFFF1);
	costs->SetTint(0x80,0x80,0x80,0x80);
	m_overlayObjects.push_back(costs);
	// Cross Cost
	Text* crossCost = new Text;
	crossCost->SetFont("defaultFont");
	crossCost->SetTexture("defaultFont");
	sprintf (buffer, "$%d", m_costCross);
	crossCost->SetString(buffer);
	crossCost->SetPosition(Coord(-265.0f,210.0f,0.0f));
	m_overlayObjects.push_back(crossCost);
	

	Debug ("LevelScreen: object initialized.");
	return true;
}


// |----------------------------------------------------------------------------|
// |							    Shutdown									|
// |----------------------------------------------------------------------------|
bool LevelScreen::Shutdown() {
	Debug ("LevelScreen::Shutdown called.");
	
	Screen::Shutdown();
	
	Debug ("LevelScreen: object shutdown.");
	return true;
}


// |----------------------------------------------------------------------------|
// |							     Logic()									|
// |----------------------------------------------------------------------------|
bool LevelScreen::Logic() {
	Debug ("LevelScreen::Logic() called.");

    Screen::Logic();
	
	// Update time
	m_accumulatedTime += 1.0f / 40.0f;
	
	// Move crosshair
	float moveX = 0.0f;
	float moveY = 0.0f;
	moveX += pad[0].axes[PAD_AXIS_LX] * m_aimSpeed;
	moveY += pad[0].axes[PAD_AXIS_LY] * m_aimSpeed;
	
	// Clamp to screen bounds
	float maxX = 300.0f;
	float minX = -300.0f;
	float maxY = 50.0f;
	float minY = -250.0f;
	if (m_crosshair->GetX()+moveX > maxX) moveX = maxX - m_crosshair->GetX();
	if (m_crosshair->GetX()+moveX < minX) moveX = minX - m_crosshair->GetX();
	if (m_crosshair->GetY()+moveY > maxY) moveY = maxY - m_crosshair->GetY();
	if (m_crosshair->GetY()+moveY < minY) moveY = minY - m_crosshair->GetY();
	
	m_crosshair->SetX(m_crosshair->GetX()+moveX);
	m_crosshair->SetY(m_crosshair->GetY()+moveY);
	
	// Move cannon
	Coord rotation = m_cannon->GetOrientation();
	rotation.x += moveY*-0.002f;
	rotation.y += moveX*-0.005f;
	m_cannon->SetOrientation(rotation);
	Coord fireworkTarget = Coord(
			m_crosshair->GetX()*1.4f,
			m_crosshair->GetY()*-1.4f+250.0f,
			-150.0f);
	
	// If button pressed and time greater than cooldown, fire firework
	if( 	( (pad[0].buttons & PAD_TRI) || (pad[0].buttons & PAD_CROSS) || (pad[0].buttons & PAD_SQUARE) || (pad[0].buttons & PAD_CIRCLE) )
		&&   ( m_budget >= m_costCross )
		&&   (m_accumulatedTime > m_cooldown)
		&&	 (m_firework->HasExploded())
	)
	{
		// Fire firework
		m_firework->Fire();
		m_firework->SetTarget(fireworkTarget);
		
		// Start cooldown
		m_accumulatedTime = 0.0f;
		
		// Deduct from budget
		m_budget -= m_costCross;
		char buffer [50];
		sprintf (buffer, "Budget: $%d", m_budget);
		m_budgetDisplay->SetString(buffer);
		
	}

	return true;
}

// |----------------------------------------------------------------------------|
// |							    Render()									|
// |----------------------------------------------------------------------------|
bool LevelScreen::Render() {
	Debug ("LevelScreen::Render() called.");

    Screen::Render();

	return true;
}

// |----------------------------------------------------------------------------|
// |							    OnLoad()									|
// |----------------------------------------------------------------------------|
// Called when the screen is loaded.
bool LevelScreen::OnLoad() {
	Debug("LevelScreen::OnLoad called");
	Screen::OnLoad();
	
	// Set Camera Position
	CPipeline::GetSingleton().PositionCamera(Vector4(0.0f, 00.0f, 400.0f, 1.0f), 23.0f * 3.14f / 180.0f, 0.0f);

	return true;
}

// |----------------------------------------------------------------------------|
// |							    OnExit()									|
// |----------------------------------------------------------------------------|
bool LevelScreen::OnExit() {
	Debug("LevelScreen::OnExit called.");

	return true;
}