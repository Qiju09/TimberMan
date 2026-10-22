#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>

using namespace sf;

void loadBranchPos();

void branchDraw(int num, RenderWindow &window);

void updateBranches(int seed);

const int NUM_BRANCHES{ 6 };
Sprite branches[NUM_BRANCHES];

enum class side{LEFT,RIGHT,NONE};
side branchPosition[NUM_BRANCHES];

int main() {
	RenderWindow window(VideoMode(1920, 1080), L"Дровосек",Style::Fullscreen);

	Event event;

	Clock clock;

	Time deltaTime;

	RectangleShape timeBar;
	float timeBarStartWidth{ 400 };
	float timeBarStartHeight{ 60 };
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarStartHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2.0f) - timeBarStartWidth / 2, 990);

	RectangleShape blackBar;
	float blackBarWidth{ 430.0f };
	blackBar.setSize(Vector2f(blackBarWidth, 80));
	blackBar.setFillColor(Color::Black);
	blackBar.setPosition((1920 / 2.0f) - blackBarWidth / 2.0f, 980);

	Time gametimeTotal;
	float timeRemaining{ 6.0f };
	float timeBarWidthPeSecond{ timeBarStartWidth / timeRemaining };
	
	bool paused{true};

	Texture textureBackground;
	textureBackground.loadFromFile("graphics\\background.png");
	Sprite spriteBackground;
	spriteBackground.setTexture(textureBackground);
	spriteBackground.setPosition(0, 0);

	Texture textureTree;
	textureTree.loadFromFile("graphics\\tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	Texture textureBee;
	textureBee.loadFromFile("graphics\\bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(2000, 800);

	bool beeActive{ false };
	float beeSpeed{ 0.0f };

	Texture textureCloud;
	textureCloud.loadFromFile("graphics\\cloud.png");
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);
	spriteCloud1.setPosition(-200, 0);
	spriteCloud2.setPosition(2000, 250);
	spriteCloud3.setPosition(-200, 500);

	bool cloudActive1{}, cloudActive2{}, cloudActive3{};
	float cloudSpeed1{ 0.0f }, cloudSpeed2{ 0.0f }, cloudSpeed3{ 0.0f };

	Font font;
	font.loadFromFile("fonts\\monogram-extended.ttf");

	int score{ 0 };

	Text messageText;
	Text scoreText;
	messageText.setFont(font);
	scoreText.setFont(font);
	messageText.setString(L"Нажмите Enter, чтобы начать!");
	scoreText.setString("Score = 0");
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	int maxScore{};

	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, -20);

	Texture textureBranch;
	textureBranch.loadFromFile("graphics\\branch.png");
	for (int i{};i < NUM_BRANCHES;i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		branches[i].setOrigin(220, 20);
	}

	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics\\player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	side playerSide = side::LEFT;

	Texture textureRIP;
	textureRIP.loadFromFile("graphics\\rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	Texture textureAxe;
	textureAxe.loadFromFile("graphics\\axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);	

	const float AXE_POSITION_LEFT{ 700.0f };
	const float AXE_POSITION_RIGHT{ 1075.0f };

	Texture textureLog;
	textureLog.loadFromFile("graphics\\log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	bool logActive{ false };
	float logSpeedX{ 1000.0f };
	float logSpeedY{ -1500.0f };

	bool acceptInput{ false };

	bool dead{ false };

	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound\\chop.wav");
	Sound chop;
	chop.setVolume(20);
	chop.setBuffer(chopBuffer);

	SoundBuffer deadBuffer;
	deadBuffer.loadFromFile("sound\\death.wav");
	Sound deadSound;
	deadSound.setVolume(5);
	deadSound.setBuffer(deadBuffer);

	SoundBuffer out_of_timeBuffer;
	out_of_timeBuffer.loadFromFile("sound\\out_of_time.wav");
	Sound outOfTime;
	outOfTime.setVolume(20);
	outOfTime.setBuffer(out_of_timeBuffer);

	while (window.isOpen()) {
		if (Keyboard::isKeyPressed(Keyboard::Escape)) window.close();

		if (Keyboard::isKeyPressed(Keyboard::Return)&&paused) {
			paused = false;
			dead = false;
			score = 0;
			timeRemaining = 6;

			for (int i{};i < NUM_BRANCHES;i++) {
				branchPosition[i] = side::NONE;
			}
			spriteRIP.setPosition(675, 2000);
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}

		if(!paused){
			while (window.pollEvent(event)) {
				if (event.type == Event::KeyReleased|| event.type == Event::MouseButtonReleased) {
					acceptInput = true;
					spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
				}
			}
		}

		if (acceptInput) {
			if (Keyboard::isKeyPressed(Keyboard::D)||Keyboard::isKeyPressed(Keyboard::Right)||Mouse::isButtonPressed(Mouse::Right)) {
				playerSide = side::RIGHT;

				score++;
				timeRemaining += (2.0f/ score) + .15;
				if (timeRemaining > 6) timeRemaining = 6;

				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 720);

				updateBranches(score);

				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				chop.play();
			}
			if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)||Mouse::isButtonPressed(Mouse::Left)) {
				playerSide = side::LEFT;

				score++;
				timeRemaining += (2.0f / score) + .15;
				if (timeRemaining > 6) timeRemaining = 6;

				spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);

				updateBranches(score);

				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;

				acceptInput = false;

				chop.play();
			}
		}

		deltaTime = clock.restart();
		
		if (!paused) {
			timeRemaining -= deltaTime.asSeconds();

			timeBar.setSize(Vector2f(timeBarWidthPeSecond*timeRemaining, timeBarStartHeight));

			if (timeRemaining <= 0.0f) {
				paused = true;

				messageText.setString(L"Время вышло!");
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				outOfTime.play();
			}

			if (!beeActive) {
				srand((int)time(0));
				beeSpeed = (rand() % 200) + 200;

				srand((int)time(0)*10);
				float heightBee{(float)(rand() % 300 + 500) };
				spriteBee.setPosition(2000, heightBee);
				beeActive = true;
			}
			else {
				spriteBee.setPosition(spriteBee.getPosition().x - beeSpeed * deltaTime.asSeconds(), spriteBee.getPosition().y);
				if (spriteBee.getPosition().x < -100) {
					beeActive = false;
				}
			}

			if (!cloudActive1) {
				srand(int(time(0)*10));
				cloudSpeed1 = rand() % 200+50;

				srand(int(time(0) * 10));
				float heightCloud1{ float(rand() % 150) + 200};
				spriteCloud1.setPosition(-200,heightCloud1);
				cloudActive1 = true;
			}
			else {
				spriteCloud1.setPosition(spriteCloud1.getPosition().x + cloudSpeed1 * deltaTime.asSeconds(), spriteCloud1.getPosition().y);
				if (spriteCloud1.getPosition().x > 1920) {
					cloudActive1 = false;
				}
			}

			if (!cloudActive2) {
				srand(int(time(0) * 20));
				cloudSpeed2 = rand() % 200 + 100;

				srand(int(time(0) * 20));
				float heightCloud2{ float(rand() % 150) + 100 };
				spriteCloud2.setPosition(2000, heightCloud2);
				cloudActive2 = true;
			}
			else {
				spriteCloud2.setPosition(spriteCloud2.getPosition().x - cloudSpeed2 * deltaTime.asSeconds(), spriteCloud2.getPosition().y);
				if (spriteCloud2.getPosition().x <-100) {
					cloudActive2 = false;
				}
			}

			if (!cloudActive3) {
				srand(int(time(0) * 30));
				cloudSpeed3 = rand() % 200 + 100;

				srand(int(time(0) * 30));
				float heightCloud3{ float(rand() % 150) -50};
				spriteCloud3.setPosition(-200, heightCloud3);
				cloudActive3 = true;
			}
			else {
				spriteCloud3.setPosition(spriteCloud3.getPosition().x + cloudSpeed3 * deltaTime.asSeconds(), spriteCloud3.getPosition().y);
				if (spriteCloud3.getPosition().x > 1920) {
					cloudActive3 = false;
				}
			}

			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			loadBranchPos();

			if (logActive) {
				spriteLog.setPosition(spriteLog.getPosition().x + logSpeedX * deltaTime.asSeconds(), spriteLog.getPosition().y + logSpeedY * deltaTime.asSeconds());
				
				if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x>2000) {
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			if (branchPosition[5] == playerSide) {
				paused = true;
				dead = true;

				spritePlayer.setPosition(2000, 600);
				FloatRect RIPRECT = spriteRIP.getLocalBounds();
				spriteRIP.setOrigin(RIPRECT.left + RIPRECT.width / 2.0f,RIPRECT.top);
				spriteRIP.setPosition(1920 / 2, 770);

				messageText.setString(L"Раздавлен!");
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2, 1080 / 2);

				deadSound.play();
			}

			if (maxScore < score) maxScore = score;

			window.clear();
			window.draw(spriteBackground);
			window.draw(spriteCloud3);
			window.draw(spriteCloud2);
			window.draw(spriteCloud1);
			branchDraw(NUM_BRANCHES, window);
			window.draw(spriteLog);
			window.draw(spriteTree);
			window.draw(spritePlayer);
			window.draw(spriteAxe);
			window.draw(spriteLog);
			window.draw(spriteBee);
			window.draw(blackBar);
			window.draw(timeBar);
			window.draw(scoreText);
		}
		else {
			window.draw(spriteBackground);
			window.draw(spriteTree);
			if (dead)window.draw(spriteRIP);
			window.draw(messageText);
			window.draw(scoreText);
		}
		window.display();
	}
}
void updateBranches(int seed){
	for (int i{ NUM_BRANCHES - 1 };i > 0;i--) {
		branchPosition[i] = branchPosition[i - 1];
	}
	srand(time(0) * seed);
	int r = rand() % 5;
	switch (r)
	{
	case 0:
		branchPosition[0] = side::LEFT;
		break;
	case 1:
		branchPosition[0] = side::RIGHT;
		break;
	default:
		branchPosition[0] = side::NONE;
		break;
	}
}

void branchDraw(int num, RenderWindow &window) {

	for (int i{};i < num;i++) {
		window.draw(branches[i]);
	}
}

void loadBranchPos() {
	for (int i{};i < NUM_BRANCHES;i++) {
		float height = i * 170;

		if (branchPosition[i] == side::LEFT) {
			branches[i].setPosition(610, height);
			branches[i].setRotation(180);
		}
		else if (branchPosition[i] == side::RIGHT) {
			branches[i].setPosition(1330, height);
			branches[i].setRotation(0);
		}
		else {
			branches[i].setPosition(3000, height);
		}
	}
}