#include "game.h"
#include "particle_generator.h"


Game::Game(GLuint width, GLuint height)
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
	delete Renderer;
	delete Particles;
	delete Ball;
	delete Player;
	delete Effects;
}

void Game::Init()
{
	// shader init
	ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "sprite");
	ResourceManager::LoadShader("shaders/particle.vs", "shaders/particle.fs", nullptr, "particle");
	ResourceManager::LoadShader("shaders/post_processing.vs", "shaders/post_processing.fs", nullptr, "postprocessing");


	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").Use().SetMatrix4("projection", projection);

	// load textures
	ResourceManager::LoadTexture("textures/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("textures/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("textures/face.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("textures/paddle.png", GL_TRUE, "paddle");
	ResourceManager::LoadTexture("textures/particle.png", GL_TRUE, "particle");

	// render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	Particles = new ParticleGenerator(
		ResourceManager::GetShader("particle"),
		ResourceManager::GetTexture("particle"),
		500
	);
	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);

	// load levels
	GameLevel one; one.Load("levels/one.lvl", this->Width, this->Height * 0.5);
	GameLevel two; two.Load("levels/two.lvl", this->Width, this->Height * 0.5);
	GameLevel three; three.Load("levels/three.lvl", this->Width, this->Height * 0.5);
	GameLevel four; four.Load("levels/four.lvl", this->Width, this->Height * 0.5);
	//GameLevel five; five.Load("levels/five.lvl", this->Width, this->Height * 0.5);
	this->levels.push_back(one);
	this->levels.push_back(two);
	this->levels.push_back(three);
	this->levels.push_back(four);
	//this->levels.push_back(five);
	this->level = 0;

	// player init
	glm::vec2 playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y
	);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	// ball init
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
		ResourceManager::GetTexture("face"));

	//Effects->Chaos = GL_TRUE;
	//Effects->Confuse = GL_TRUE;
}

void Game::Update(GLfloat dt)
{
	// move ball
	Ball->Move(dt, this->Width);

	// check for collisions
	this->DoCollisions();

	// update particles
	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2));

	// reduce shake time
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = GL_FALSE;
	}

	// check loss condition
	if (Ball->Position.y >= this->Height)
	{
		this->ResetLevel();
		this->ResetPlayer();
	}

}

void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;

		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)
					Ball->Position.x -= velocity;
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;
			}	
		}
		if (this->Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;
	}
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		Effects->BeginRender();

			// backgroud
			Renderer->DrawSprite(ResourceManager::GetTexture("background"),
				glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f);

			// bricks
			this->levels[this->level].Draw(*Renderer);
		
			// player
			Player->Draw(*Renderer);

			// particles
			Particles->Draw();

			// ball
			Ball->Draw(*Renderer);

		Effects->EndRender();
		Effects->Render(glfwGetTime());
	}

}

GLboolean Game::CheckCollisioin(GameObject &one, GameObject &two)
{
	// collision x-axis?
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;
	// collision y-axis?
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;

	return collisionX && collisionY;
}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, -1.0f),
		glm::vec2(-1.0f, 0.0f)
	};
	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

Collision Game::CheckCollisioin(BallObject &one, GameObject &two)
{

	// Get center point circle first
	glm::vec2 center(one.Position + one.Radius);
	
	// calculate AABB info
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(
		two.Position.x + aabb_half_extents.x,
		two.Position.y + aabb_half_extents.y
	);

	// get differernce vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

	// get closest point to circle from box
	glm::vec2 closest = aabb_center + clamped;

	// retrieve vector between center circle and closest point
	difference = closest - center;
	if (glm::length(difference) < one.Radius)\
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	else
		return std::make_tuple(GL_FALSE, DOWN, glm::vec2(0, 0));
}

void Game::DoCollisions()
{
	// collision for ball and bricks
	for (GameObject &box : this->levels[this->level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollisioin(*this->Ball, box);
			if (std::get<0>(collision))
			{
				// Destroy block if not solid
				if (!box.IsSolid)
					box.Destroyed = GL_TRUE;
				else
				{
					ShakeTime = 0.05f;
					Effects->Shake = true;
				}
				// Collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);

				if (dir == LEFT || dir == RIGHT) // Horizontal collision
				{
					Ball->Velocity.x = -Ball->Velocity.x;
					// Relocate
					GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
					if (dir == LEFT)
						Ball->Position.x += penetration;
					else
						Ball->Position.x -= penetration;
				}
				else // Vertical collision
				{
					Ball->Velocity.y = -Ball->Velocity.y;
					// Relocate
					GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
					if (dir == UP)
						Ball->Position.y -= penetration;
					else
						Ball->Position.y += penetration;
				}
			}
		}
	}

	// collision for ball and players
	Collision result = CheckCollisioin(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		// Check where it hit
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		GLfloat percentage = distance / (Player->Size.x / 2);
		
		// Move accordingly
		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		//Ball->Velocity.y = -Ball->Velocity.y;
		Ball->Velocity.y = -1 * abs(Ball->Velocity.y);
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
	}
}

void Game::ResetLevel()
{
	if (this->level == 0)
		this->levels[0].Load("levels/one.lvl", this->Width, this->Height * 0.5f);
	else if (this->level == 1)
		this->levels[1].Load("levels/two.lvl", this->Width, this->Height * 0.5f);
	else if (this->level == 2)
		this->levels[2].Load("levels/three.lvl", this->Width, this->Height * 0.5f);
	else if (this->level == 3)
		this->levels[3].Load("levels/four.lvl", this->Width, this->Height * 0.5f);
}

void Game::ResetPlayer()
{
	this->Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}