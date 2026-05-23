#include <iostream>
#include<cstdlib>
#include<time.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;


class Player {
public:
    string name;

};
class Screen {
private:
    const int resolutionX;
    const int resolutionY;
public:
    Screen() : resolutionX(960), resolutionY(640) {}
    int getResolutionX() const {
        return resolutionX;
    }

    int getResolutionY() const {
        return resolutionY;
    }
};

class Game {
protected:
    Player *P;
    int score;
    Screen* S;
    Font game_over;
    Text game_over_Text;
    Font font;
public:
    virtual void startGame(Event& e, RenderWindow& window) = 0;
    virtual void endGame(Event& e, RenderWindow& window) = 0;
    virtual void render(Event& e, RenderWindow& window) = 0;
    virtual bool goBack() = 0;
};

class Grid {
private:
    Texture gridTexture;
    Sprite playAreaSprite;

public:
    Grid() {}

    void render(RenderWindow& window, Screen* screen) {
        if (!gridTexture.loadFromFile("Images/grid.png")) {
            cerr << "Failed to load grid texture!" << endl;
            return;
        }

        playAreaSprite.setTexture(gridTexture);
        playAreaSprite.setScale(
            static_cast<float>(screen->getResolutionX()) / gridTexture.getSize().x,
            static_cast<float>(screen->getResolutionY()) / gridTexture.getSize().y
        );
        playAreaSprite.setPosition(0, 0);
        window.draw(playAreaSprite);
    }
};



class SnakeSegment {
private:
    float x;
    float y;
    RectangleShape segment;
public:
    SnakeSegment() : x(0), y(0) {}
    void setPosition(float X, float Y) {
        x = X;
        y = Y;
    }

    float getX() const {
        return x;
    }

    float getY() const {
        return y;
    }
};
       
class SnakeGame : public Game {
private:
    Grid G;
    SnakeSegment* SS;
    int snakeSize;
    bool isSnake;
    Texture snakeTexture;
    Sprite snakeSprite;
    char direction;
    bool isFruit;
    Texture foodTexture;
    Sprite foodSprite;
    Texture levelTexture;
    Sprite levelSprite;
    bool collisionHandled;
    static int foodTimer;
    bool isGameover;
    bool isLevel;
    int speed;
    bool back;
    bool isPaused;
    RectangleShape pauseMenuBackground;
    Text resumeText;
    Text restartText;
    Text scoreText;
    int score = 0;
public:
    SnakeGame() : snakeSize(3), isSnake(false), direction('D'), collisionHandled(false), isFruit(false), isGameover(0), isLevel(0), speed(0), back(0), isPaused(0) {
        S = new Screen;
        SS = new SnakeSegment[snakeSize];
        pauseMenuBackground.setPosition(280, 130);
        pauseMenuBackground.setSize(Vector2f(400, 300));
        pauseMenuBackground.setFillColor(sf::Color(43, 51, 28, 255));
    }

    void startGame(Event& e, RenderWindow& window) override {
        srand(time(0));
        render(e, window);


        if (!snakeTexture.loadFromFile("Images/snake.png")) {
            cerr << "Failed to load snake texture! " << endl;
        }

        if (!foodTexture.loadFromFile("Images/food.png")) {
            cout << "Failed to load food texture!" << endl;
        }
        foodSprite.setTexture(foodTexture);

        if (!levelTexture.loadFromFile("Images/snakeLevels.png")) {
            cout << "Failed to load level texture!" << endl;
        }
        levelSprite.setPosition(200, 100);
        levelSprite.setTexture(levelTexture);

        font.loadFromFile("Images/arial.ttf");
        resumeText.setFont(font);
        resumeText.setString("Resume");
        resumeText.setCharacterSize(30);
        resumeText.setFillColor(sf::Color(134, 157, 91));
        resumeText.setPosition(430, 220);
        restartText.setFont(font);
        restartText.setString("Restart");
        restartText.setCharacterSize(30);
        restartText.setFillColor(sf::Color(134, 157, 91));
        restartText.setPosition(430, 340);

        scoreText.setFont(font);
        scoreText.setCharacterSize(40);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(10, 10);

        scoreText.setFont(font);              // Set the font
        scoreText.setCharacterSize(40);            // Increase the size of the text (bigger)
        scoreText.setFillColor(Color::White);      // Set the text color to white
        scoreText.setStyle(Text::Bold);            // Set the style to bold
        scoreText.setPosition(10, 10);

        scoreText.setString("Score: " + std::to_string(score));
        Vector2u windowSize = window.getSize();
        scoreText.setPosition(windowSize.x - scoreText.getLocalBounds().width - 10, 10);
        window.draw(scoreText);


        if (isGameover) {
            endGame(e, window);
        }
        else if (!isLevel) {
            FloatRect Level1Bounds(270, 400, 100, 35);
            FloatRect Level2Bounds(410, 400, 120, 35);
            FloatRect Level3Bounds(550, 400, 140, 35);
            window.draw(levelSprite);
            if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (Level1Bounds.contains(mousePos.x, mousePos.y)) {
                    isLevel = true;
                    speed = 10;
                }
                else if (Level2Bounds.contains(mousePos.x, mousePos.y)) {
                    isLevel = true;
                    speed = 5;
                }
                else if (Level3Bounds.contains(mousePos.x, mousePos.y)) {
                    isLevel = true;
                    speed = 3;
                }
            }
        }
        else if (isPaused) {
            window.draw(pauseMenuBackground);
            window.draw(resumeText);
            window.draw(restartText);
            if (e.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (resumeText.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    isPaused = false;
                }

                if (restartText.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    isSnake = false;
                    isFruit = false;
                    isPaused = false;
                    isGameover = false;
                    snakeSize = 3;
                    direction = 'D';

                    foodTimer = 0;
                }
            }
        }
        else {
            if (Keyboard::isKeyPressed(Keyboard::D) && direction != 'A') {
                direction = 'D';
            }
            else if (Keyboard::isKeyPressed(Keyboard::S) && direction != 'W') {
                direction = 'S';
            }
            else if (Keyboard::isKeyPressed(Keyboard::A) && direction != 'D') {
                direction = 'A';
            }
            else if (Keyboard::isKeyPressed(Keyboard::W) && direction != 'S') {
                direction = 'W';
            }
            else if (Keyboard::isKeyPressed(Keyboard::P)) {
                if (isPaused != 1)
                    isPaused = 1;
            }
            if (!isSnake) {
                generateSnake();
            }
            else if (!isGameover) {
                moveSnake();
            }

            if (detectFoodCollision() && !collisionHandled && isFruit) {
                isFruit = false;
                collisionHandled = true;
                incrementSnake();
                score += 10;
            }

            if (detectWallCollision()) {
                isGameover = true;
            }

            if (detectSnakeCollision()) {
                isGameover = true;
            }


            if (!isFruit) {
                if (foodTimer == 0) {
                    generateFood();
                    isFruit = true;
                    collisionHandled = false;
                }
            }
            if (foodTimer >= 100)
                foodTimer = -5;
            foodTimer++;

            // Draw the food
            foodSprite.setScale(45.0f / foodTexture.getSize().x, 40.0f / foodTexture.getSize().y);
            if (isFruit)
                window.draw(foodSprite);

            // Draw each snake segment
            for (int i = 0; i < snakeSize; i++) {
                snakeSprite.setPosition(SS[i].getX(), SS[i].getY());
                snakeSprite.setTexture(snakeTexture);

                if (i == 0) {
                    snakeSprite.setScale(30.0f / snakeTexture.getSize().x, 30.0f / snakeTexture.getSize().y);
                }
                else {
                    snakeSprite.setScale(31.0f / snakeTexture.getSize().x, 31.0f / snakeTexture.getSize().y);
                }

                window.draw(snakeSprite);
            }
        }
    }

    bool goBack() override {
        return back;
    }

    void render(Event& e, RenderWindow& window) override {
        G.render(window, S);
    }

    void generateSnake() {
        float startX = (rand() % (S->getResolutionX() - 200)) + 100;
        float startY = (rand() % (S->getResolutionY() - 200)) + 100;

        for (int i = 0; i < snakeSize; i++) {
            if (i == 0) {
                SS[i].setPosition(startX, startY);
            }
            else {
                SS[i].setPosition(SS[i - 1].getX() - 30.0f, SS[i - 1].getY());
            }
        }
        isSnake = true;
    }

    void generateFood() {
        bool validPosition = false;
        int foodX, foodY;
        float foodWidth = foodSprite.getGlobalBounds().width;
        float foodHeight = foodSprite.getGlobalBounds().height;

        while (!validPosition) {
            foodX = rand() % (S->getResolutionX() - 200) + 100;
            foodY = rand() % (S->getResolutionY() - 200) + 100;

            validPosition = true;

            // Ensure the food doesn't overlap with any snake segment considering sprite size
            for (int i = 0; i < snakeSize; i++) {
                float snakeX = SS[i].getX();
                float snakeY = SS[i].getY();
                float snakeWidth = 31.0f;  // Adjust this value based on your snake segment size
                float snakeHeight = 31.0f; // Adjust this value based on your snake segment size

                // Check if the food's bounding box overlaps with any snake segment
                if (foodX + foodWidth > snakeX && foodX < snakeX + snakeWidth &&
                    foodY + foodHeight > snakeY && foodY < snakeY + snakeHeight) {
                    validPosition = false;
                    break;
                }
            }
        }

        foodSprite.setPosition(foodX, foodY);
    }


    void moveSnake() {
        static int moveCounter = 0;
        moveCounter++;

        if (moveCounter >= speed) {
            float prevX = SS[0].getX();
            float prevY = SS[0].getY();
            float tempX, tempY;

            switch (direction) {
            case 'D': SS[0].setPosition(SS[0].getX() + 30, SS[0].getY()); break;
            case 'A': SS[0].setPosition(SS[0].getX() - 30, SS[0].getY()); break;
            case 'W': SS[0].setPosition(SS[0].getX(), SS[0].getY() - 30); break;
            case 'S': SS[0].setPosition(SS[0].getX(), SS[0].getY() + 30); break;
            }

            for (int i = 1; i < snakeSize; i++) {
                tempX = SS[i].getX();
                tempY = SS[i].getY();
                SS[i].setPosition(prevX, prevY);
                prevX = tempX;
                prevY = tempY;
            }

            moveCounter = 0;
        }
    }

    bool detectFoodCollision() {
        float diffX = SS[0].getX() - foodSprite.getPosition().x;
        float diffY = SS[0].getY() - foodSprite.getPosition().y;
        return (diffX > -30.0f && diffX < 30.0f) && (diffY > -30.0f && diffY < 30.0f);
    }

    void incrementSnake() {
        // Allocate new memory for the snake array, with an extra segment
        SnakeSegment* newSnakeSegments = new SnakeSegment[snakeSize + 1];

        // Copy the current snake segments to the new array
        for (int i = 0; i < snakeSize; i++) {
            newSnakeSegments[i] = SS[i];
        }

        // Set the position of the new segment at the end of the snake
        newSnakeSegments[snakeSize].setPosition(SS[snakeSize - 1].getX() - 26.5f, SS[snakeSize - 1].getY());

        // Free the old memory and assign the new array to SS
        delete[] SS;
        SS = newSnakeSegments;

        // Increase the snake size
        snakeSize++;
    }

    bool detectWallCollision() {
        float headX = SS[0].getX();
        float headY = SS[0].getY();

        // Assuming the game area is defined by the screen resolution, adjust the boundaries as needed
        if (headX < 0 || headX >= S->getResolutionX() || headY < 0 || headY >= S->getResolutionY()) {
            // Snake head is outside the screen area (wall collision)
            return true;
        }
        return false;  // Snake is within bounds
    }

    bool detectSnakeCollision() {
        // Skip the collision check if there's only one segment
        if (snakeSize <= 1) {
            return false; // No collision possible if there's only one segment
        }

        // Head position and size
        float headX = SS[0].getX();
        float headY = SS[0].getY();
        float segmentWidth = 20.0f;  // Size of each snake segment (head or body)
        float segmentHeight = 20.0f; // Size of each snake segment (head or body)

        // Loop through the body segments (starting from index 1)
        for (int i = 4; i < snakeSize; i++) {
            float bodyX = SS[i].getX();
            float bodyY = SS[i].getY();

            // Check if the head overlaps with the sides of the body segment
            if (headX < bodyX + segmentWidth && headX + segmentWidth > bodyX &&
                headY < bodyY + segmentHeight && headY + segmentHeight > bodyY) {
                // Snake head collides with body side
                return true;
            }
        }

        return false;  // No collision with the body
    }




    void endGame(Event& e, RenderWindow& window) override {
        if (!game_over.loadFromFile("Images/arial.ttf"))
        {
            std::cerr << "Failed to load font" << std::endl;
        }
        game_over_Text.setFont(game_over);
        game_over_Text.setCharacterSize(150);
        game_over_Text.setFillColor(sf::Color::Black);
        game_over_Text.setStyle(sf::Text::Bold);
        game_over_Text.setPosition(0, 210);
        game_over_Text.setString("GAME OVER!");
        window.draw(game_over_Text);
        if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) {
            back = 1;
        }
    }
};

int SnakeGame::foodTimer = 0;


class WordManager {
private:
    string secretWord;
    bool guessed[20];
    int wordLength;
    bool hintUsed = false;

public:
    WordManager() : wordLength(0) {}

    void initializeWord(const string& word) {
        secretWord = word;
        wordLength = secretWord.length();
        for (int i = 0; i < wordLength; ++i) {
            guessed[i] = false;
        }
        hintUsed = false; // Reset hint usage
    }

    bool processGuess(char letter) {
        bool found = false;
        for (int i = 0; i < wordLength; ++i) {
            if (secretWord[i] == letter && !guessed[i]) {
                guessed[i] = true;
                found = true;
            }
        }
        return found;
    }

    void displayWord(string& display) const {
        display.clear();
        for (int i = 0; i < wordLength; ++i) {
            display += guessed[i] ? secretWord[i] : '_';
            display += ' ';
        }
    }

    bool isGameWon() const {
        for (int i = 0; i < wordLength; ++i) {
            if (!guessed[i]) return false;
        }
        return true;
    }

    string getSecretWord() const { return secretWord; }

    // Function to give a hint
    char getHint() {
        for (int i = 0; i < wordLength; ++i) {
            if (!guessed[i]) {
                guessed[i] = true; // Reveal the letter
                return secretWord[i]; // Return the hint letter
            }
        }
        return '\0'; // If all letters are guessed, no hint available
    }

    bool isHintUsed() const { return hintUsed; }
    void setHintUsed() { hintUsed = true; }
};

class DisplayManager {
public:
    void drawText(RenderWindow& window, Font& font, const string& text, Vector2f pos, int size, Color color) {
        Text txt;
        txt.setFont(font);
        txt.setString(text);
        txt.setCharacterSize(size);
        txt.setFillColor(color);
        txt.setPosition(pos);
        window.draw(txt);
    }

    void drawHangman(RenderWindow& window, int wrongGuesses) {
        float centerX = 500.0f;  // X-coordinate for the center of the screen
        float centerY = 350.0f;  // Y-coordinate for the center of the screen

        // Draw the stand
        RectangleShape standBase(Vector2f(180.0f, 8.0f));
        standBase.setPosition(centerX - 100.0f, centerY + 210.0f);  // Stand base
        standBase.setFillColor(Color::White);
        window.draw(standBase);

        RectangleShape standPole(Vector2f(8.0f, 260.0f));
        standPole.setPosition(centerX - 20.0f, centerY - 50.0f);  // Stand pole
        standPole.setFillColor(Color::White);
        window.draw(standPole);

        RectangleShape standTop(Vector2f(100.0f, 8.0f));
        standTop.setPosition(centerX - 20.0f, centerY - 58.0f);  // Stand top horizontal bar
        standTop.setFillColor(Color::White);
        window.draw(standTop);

        if (wrongGuesses > 0) {  // Head
            CircleShape head(25.0f);
            head.setPosition(centerX + 50.0f, centerY - 58.0f);  // Adjusted to center the head
            head.setFillColor(Color::White);
            window.draw(head);
        }
        if (wrongGuesses > 1) {  // Body
            RectangleShape body(Vector2f(10, 120));
            body.setPosition(centerX + 67.0f, centerY - 30.0f);  // Adjusted to center the body
            body.setFillColor(Color::White);
            window.draw(body);
        }
        if (wrongGuesses > 2) {  // Left Arm
            RectangleShape leftArm(Vector2f(80, 10));
            leftArm.setPosition(centerX + 10.0f, centerY + 35.0f);  // Adjusted position for left arm
            leftArm.setRotation(-45);
            leftArm.setFillColor(Color::White);
            window.draw(leftArm);
        }
        if (wrongGuesses > 3) {  // Left Arm
            RectangleShape rightArm(Vector2f(80, 10));
            rightArm.setPosition(centerX + 70.0f, centerY - 25.0f);  // Adjusted position for left arm
            rightArm.setRotation(45);
            rightArm.setFillColor(Color::White);
            window.draw(rightArm);
        }
        if (wrongGuesses > 4) {  // Left Leg
            RectangleShape leftLeg(Vector2f(10, 80));
            leftLeg.setPosition(centerX + 68.0f, centerY + 90.0f);  // Adjusted position for left leg
            leftLeg.setRotation(-30);
            leftLeg.setFillColor(Color::White);
            window.draw(leftLeg);

        }
        if (wrongGuesses > 5) {  // Right Leg
            RectangleShape rightLeg(Vector2f(10, 80));
            rightLeg.setPosition(centerX + 68.0f, centerY + 90.0f);  // Adjusted position for right leg
            rightLeg.setRotation(30);
            rightLeg.setFillColor(Color::White);
            window.draw(rightLeg);
        }


    }
};

class HangmanGame : public Game {
private:
    WordManager wordManager;
    DisplayManager displayManager;
    int wrongGuesses;
    int maxWrongGuesses = 6;
    string gameOverMessage;
    bool isgameOver;
    bool hintUsed;
    RectangleShape hintButton;  // Declare hintButton
    int maxHints;  // Max number of hints allowed
    Texture loadingTexture; // For the loading screen image
    Sprite loadingSprite;   // For the loading screen image 
    SoundBuffer dramaticStartBuffer;  // Buffer for the dramatic start sound
    Sound dramaticStartSound;
    bool isLoading;
    bool back;
public:
    HangmanGame() : wrongGuesses(0), isgameOver(false), hintUsed(false), maxHints(0), isLoading(true), back(0) {}

    void startGame(Event& e, RenderWindow& window) {
        if (isLoading) {
            showLoadingScreen(window);
            isLoading = false;
        }

        // Define the two categories
        string fruits[] = {
            "apple", "banana", "grape", "orange", "mango", "pear", "peach", "plum", "lemon", "melon",
            "kiwi", "cherry", "lime", "fig", "date", "guava", "papaya", "apricot", "pineapple", "blueberry",
            "strawberry", "raspberry", "blackberry", "watermelon", "coconut", "pomegranate", "cranberry", "nectarine",
            "tangerine", "cantaloupe", "lychee", "dragonfruit", "jackfruit", "passionfruit", "persimmon", "starfruit",
            "olive", "mulberry", "currant", "quince", "loganberry", "satsuma", "gooseberry", "jabuticaba", "rambutan",
            "durian", "longan", "boysenberry", "honeydew", "acerola", "bloodorange"
        };

        string vegetables[] = {
            "carrot", "potato", "tomato", "cucumber", "spinach", "onion", "garlic", "pepper", "lettuce", "broccoli",
            "cauliflower", "cabbage", "zucchini", "pumpkin", "eggplant", "radish", "turnip", "celery", "parsley", "beet",
            "leek", "kale", "chard", "asparagus", "artichoke", "okra", "peas", "beans", "corn", "sweetpotato",
            "yam", "mushroom", "ginger", "shallot", "parsnip", "fennel", "basil", "cilantro", "dill", "arugula",
            "watercress", "mustardgreens", "bokchoy", "brusselsprouts", "chili", "squash", "scallion", "mint", "oregano", "chive"
        };
        string simpleWords[] = {
            "house", "river", "cloud", "school", "table", "chair", "pencil", "paper", "window", "garden",
            "book", "door", "phone", "flower", "clock", "bridge", "lamp", "shirt", "pants", "plate",
            "glass", "knife", "spoon", "tree", "bird", "dog", "cat", "fish", "train", "bus",
            "car", "road", "sun", "moon", "star", "beach", "park", "lake", "desk", "cup",
            "bed", "box", "bag", "hat", "shoe", "bell", "key", "rug", "wall", "roof"
        };


        // Display category selection menu
        int categoryChoice = 0; // 1 for fruits, 2 for vegetables, 3 for simple words
        Text titleText, fruitsOption, vegetablesOption, simpleWordsOption;
        Font menuFont;
        if (!menuFont.loadFromFile("Images/arial.ttf")) {
            cerr << "Failed to load font!" << endl;
            exit(EXIT_FAILURE);
        }

        titleText.setFont(menuFont);
        titleText.setString("Select a category:\n1) Fruits\n2) Vegetables\n3) General words");
        titleText.setCharacterSize(30);
        titleText.setFillColor(Color::White);
        titleText.setPosition(200, 200);

        while (categoryChoice == 0 && window.isOpen()) {
            while (window.pollEvent(e)) {
                if (e.type == Event::Closed) {
                    window.close();
                }
                if (e.type == sf::Event::KeyPressed) {
                    if (e.key.code == sf::Keyboard::Num1) {
                        categoryChoice = 1; // Fruits
                    }
                    else if (e.key.code == sf::Keyboard::Num2) {
                        categoryChoice = 2; // Vegetables
                    }
                    else if (e.key.code == sf::Keyboard::Num3) {
                        categoryChoice = 3; // Vegetables
                    }
                }
            }

            window.clear();
            window.draw(titleText);
            window.display();
        }

        // Choose the word based on the selected category
        srand(static_cast<unsigned>(time(0)));
        string secretWord;
        if (categoryChoice == 1) {
            secretWord = fruits[rand() % (sizeof(fruits) / sizeof(fruits[0]))];
        }
        else if (categoryChoice == 2) {
            secretWord = vegetables[rand() % (sizeof(vegetables) / sizeof(vegetables[0]))];
        }
        else if (categoryChoice == 3) {
            secretWord = simpleWords[rand() % (sizeof(simpleWords) / sizeof(simpleWords[0]))];
        }

        // Initialize the word for the game
        wordManager.initializeWord(secretWord);

        // Remaining code (unchanged from your original function)
        if (!dramaticStartBuffer.loadFromFile("sound.wav")) {
            cerr << "Failed to load dramatic start sound!" << endl;
            exit(EXIT_FAILURE);
        }
        dramaticStartSound.setBuffer(dramaticStartBuffer);
        dramaticStartSound.play();

        maxHints = (secretWord.length() / 2 < 5) ? secretWord.length() / 2 : 5;

        if (!font.loadFromFile("Images/arial.ttf")) {
            cerr << "Failed to load font!" << endl;
            exit(EXIT_FAILURE);
        }

        // Initialize hintButton properties
        hintButton.setSize(Vector2f(150, 50));
        hintButton.setFillColor(Color::Blue);
        hintButton.setPosition(750, 500);

        while (1) {
            while (window.pollEvent(e)) {
                if (e.type == Event::Closed) {
                    window.close();
                }
                if (e.type == sf::Event::TextEntered) {
                    char input = static_cast<char>(e.text.unicode);

                    if ((input >= 'A' && input <= 'Z') || (input >= 'a' && input <= 'z')) {
                        if (input >= 'A' && input <= 'Z') {
                            input += 32;
                        }

                        if (!wordManager.processGuess(input)) {
                            ++wrongGuesses;
                        }
                    }
                }

                if (e.type == Event::MouseButtonPressed) {
                    Vector2i mousePos = Mouse::getPosition(window);
                    if (hintButton.getGlobalBounds().contains(mousePos.x, mousePos.y) && maxHints > 0) {
                        char hintLetter = wordManager.getHint();
                        if (hintLetter != '\0') {
                            wordManager.setHintUsed();
                            hintUsed = true;
                            maxHints--;
                        }
                    }
                }
            }

            if (wordManager.isGameWon()) {
                if (!dramaticStartBuffer.loadFromFile("happy.wav")) {
                    cerr << "Failed to load dramatic start sound!" << endl;
                    exit(EXIT_FAILURE);
                }
                dramaticStartSound.setBuffer(dramaticStartBuffer);
                dramaticStartSound.play();
                gameOverMessage = "Congratulations! You Won!";
                isgameOver = true;
            }

            if (wrongGuesses >= maxWrongGuesses) {
                if (!dramaticStartBuffer.loadFromFile("sound1.wav")) {
                    cerr << "Failed to load dramatic start sound!" << endl;
                    exit(EXIT_FAILURE);
                }
                dramaticStartSound.setBuffer(dramaticStartBuffer);
                dramaticStartSound.play();
                gameOverMessage = "You Lost! Word: " + wordManager.getSecretWord();
                isgameOver = true;
            }

            render(e, window);
        }
    }



    void showLoadingScreen(RenderWindow& window) {
        if (!loadingTexture.loadFromFile("Images/load1.png")) {
            cerr << "Failed to load loading image!" << endl;
            exit(EXIT_FAILURE);
        }

        loadingSprite.setTexture(loadingTexture);

        Text loadingText;
        loadingText.setFont(font);
        window.clear();
        window.draw(loadingSprite);
        window.draw(loadingText);
        window.display();

        sleep(seconds(2));
    }

    void endGame(Event& e, RenderWindow& window) {
        while (true) {
            window.clear();
            // If the game was won, show "Congratulations"
            if (gameOverMessage == "Congratulations! You Won!") {
                displayManager.drawText(window, font, "CONGRATULATIONS!", { 220.0f, 200.0f }, 60, Color::Green);
            }
            else {
                displayManager.drawText(window, font, "GAME OVER", { 250.0f, 200.0f }, 60, Color::Red);
            }

            displayManager.drawText(window, font, gameOverMessage, { 220.0f, 300.0f }, 50, Color::White);

            RectangleShape restartButton(Vector2f(150, 50));
            restartButton.setFillColor(Color::Green);
            restartButton.setPosition(220, 400);
            window.draw(restartButton);
            displayManager.drawText(window, font, "Restart", { 250.0f, 410.0f }, 30, Color::Black);

            RectangleShape exitButton(Vector2f(150, 50));
            exitButton.setFillColor(Color::Red);
            exitButton.setPosition(420, 400);
            window.draw(exitButton);
            displayManager.drawText(window, font, "Exit", { 470.0f, 410.0f }, 30, Color::Black);

            window.display();

            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::MouseButtonPressed) {
                    Vector2i mousePos = Mouse::getPosition(window);
                    if (restartButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        wrongGuesses = 0;
                        isgameOver = false;
                        hintUsed = false;
                        maxHints = 5;
                        startGame(event, window);
                        return;
                    }

                    if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        back = 1;
                        window.close();
                        return;
                    }
                }
            }
        }
    }
    void render(Event& e, RenderWindow& window) {
        window.clear(Color(100, 100, 255));

        string displayText;
        wordManager.displayWord(displayText);

        displayManager.drawText(window, font, "Word: " + displayText, { 50.0f, 100.0f }, 40, Color::White);
        displayManager.drawText(window, font, "Wrong Guesses: " + to_string(wrongGuesses), { 50.0f, 200.0f }, 40, Color::White);

        // Display hints remaining
        displayManager.drawText(window, font, "Hints Left: " + to_string(maxHints), { 750.0f, 460.0f }, 30, Color::White);

        displayManager.drawHangman(window, wrongGuesses);

        if (isgameOver) {
            endGame(e, window);
        }

        // Draw hint button
        window.draw(hintButton);  // Draw the hint button
        displayManager.drawText(window, font, "Hint", { 790.0f, 505.0f }, 30, Color::White);

        window.display();
    }


    bool goBack() {
        return back;
    }
};



class WordleGame : public Game {
private:
    RenderWindow window;
    Font font;
    const int wordListSize = 10;
    const int gridSize = 5;
    const int tileSize = 70;
    const int keyboardRows = 3;
    const std::string keyboardLayout[3] = { "QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM" };
    std::string wordList[10] = { "apple", "grape", "peach", "berry", "lemon", "mango", "plums", "dates", "pears", "melon" };
    std::string guesses[5] = { "", "", "", "", "" };
    std::string secretWord;
    int currentRow;
    bool gameWon;
    bool gameOver;
    bool isLoading;
    bool back;
    Texture loadingTexture;
    Sprite loadingSprite;

public:
    WordleGame() : isLoading(true), currentRow(0), gameWon(false), gameOver(false), back(0) {}
    void showLoadingScreen(RenderWindow& window) {
        if (!loadingTexture.loadFromFile("Images/3.png")) {
            cerr << "Failed to load loading image!" << endl;
            exit(EXIT_FAILURE);
        }

        loadingSprite.setTexture(loadingTexture);

        Text loadingText;
        loadingText.setFont(font);
        window.clear();
        window.draw(loadingSprite);
        window.draw(loadingText);
        window.display();

        sleep(seconds(2));
    }

    void startGame(Event& e, RenderWindow& window) {
        if (!font.loadFromFile("Images/arial.ttf")) {
            cerr << "Failed to load font!" << endl;
            exit(EXIT_FAILURE);
        }
        if (isLoading) {
            showLoadingScreen(window);
            isLoading = false;
        }


        // Initialize secret word
        srand(static_cast<unsigned>(time(0)));
        secretWord = wordList[rand() % wordListSize];

        // Convert the secret word to lowercase using a normal loop
        for (int i = 0; i < secretWord.length(); i++) {
            secretWord[i] = std::tolower(secretWord[i]);
        }

        std::string currentGuess = "";

        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                // Handle text input
                if (event.type == Event::TextEntered && !gameOver) {
                    char input = static_cast<char>(event.text.unicode);
                    if (input >= 'a' && input <= 'z') {
                        if (currentGuess.length() < gridSize)
                            currentGuess += input;
                    }
                    else if (input >= 'A' && input <= 'Z') {
                        if (currentGuess.length() < gridSize)
                            currentGuess += std::tolower(input); // Convert input to lowercase
                    }
                    else if (input == '\b' && !currentGuess.empty()) { // Backspace
                        currentGuess.pop_back();
                    }
                }

                // Handle Enter key
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter && !gameOver) {
                    if (currentGuess.length() == gridSize) {
                        guesses[currentRow] = currentGuess;

                        // Convert the guess to lowercase using a loop
                        std::string lowerGuess = currentGuess;
                        for (int i = 0; i < lowerGuess.length(); i++) {
                            lowerGuess[i] = std::tolower(lowerGuess[i]);
                        }

                        // Compare the guess with the secret word (case-insensitive)
                        if (lowerGuess == secretWord) {
                            gameWon = true;
                            gameOver = true;
                        }
                        else if (currentRow == gridSize - 1) {
                            gameOver = true;
                        }
                        currentGuess.clear();
                        currentRow++;
                    }
                }

                // Handle Mouse Clicks on the Keyboard
                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2i mousePos = Mouse::getPosition(window);

                    for (size_t row = 0; row < keyboardRows; ++row) {
                        for (size_t col = 0; col < keyboardLayout[row].length(); ++col) {
                            RectangleShape key(Vector2f(tileSize - 10, tileSize - 10));
                            key.setPosition(190 + col * (tileSize - 5), 450 + row * (tileSize));

                            if (key.getGlobalBounds().contains(Vector2f(mousePos))) {
                                char keyChar = keyboardLayout[row][col];
                                if (currentGuess.length() < gridSize) {
                                    currentGuess += std::tolower(keyChar); // Convert to lowercase
                                }
                            }
                        }
                    }
                }
            }
		window.clear();   //////////////////////////////////////////////////////
            draw(currentGuess, window, e);
            window.display();    //////////////////////////////////////////////
        }
    }


    void draw(const std::string& currentGuess, RenderWindow& window, Event& e) {
        window.clear(Color(10, 150, 150));

        // Draw grid
        for (int row = 0; row < gridSize; ++row) {
            for (int col = 0; col < gridSize; ++col) {
                RectangleShape tile(Vector2f(tileSize, tileSize));
                tile.setPosition(290 + col * (tileSize + 10), 25 + row * (tileSize + 10));
                tile.setFillColor(Color::White);
                tile.setOutlineThickness(2);
                tile.setOutlineColor(Color::Black);

                // Handle guessed tiles
                if (row < currentRow) {
                    char guessedChar = guesses[row][col];
                    tile.setFillColor(getTileColor(guessedChar, col));
                    window.draw(tile);
                    // Draw character on top of the tile
                    drawText(window, std::string(1, guessedChar), tile.getPosition());
                }
                // Handle current row
                else if (row == currentRow && col < currentGuess.size()) {
                    char guessedChar = currentGuess[col];
                    // Draw character on top of the tile
                    drawText(window, std::string(1, guessedChar), tile.getPosition());
                }
                else {
                    window.draw(tile);  // Empty tiles for current guess row
                }
            }
        }

        // Draw keyboard
        render(e, window);

        // Draw game over message
        if (gameOver) {

            endGame(e,window);
        }

        window.display();
    }


    Color getTileColor(char guessedChar, int position) {
        if (guessedChar == secretWord[position])
            return Color::Green; // Correct letter in the correct position
        else if (secretWord.find(guessedChar) != std::string::npos)
            return Color::Yellow; // Correct letter in the wrong position
        return Color(150, 150, 150); // Incorrect letter
    }

    void drawText(RenderWindow& window, const std::string& message, Vector2f position, int size = 30, Color color = Color::White) {
        Text text;
        text.setFont(font);
        text.setString(message);
        text.setCharacterSize(size);
        text.setFillColor(color);

        // Adjust position so the text is centered on the key tile
        text.setPosition(position.x + (tileSize / 4), position.y + (tileSize / 4));

        window.draw(text);
    }

    void endGame(Event& e, RenderWindow& window) {
        std::string message = gameWon ? "You Won!" : "You Lost! \nWord: " + secretWord;
        drawText(window, message, { 40, 300 }, 30, Color::Yellow);
        back = 1;
    }
    void render(Event& e, RenderWindow& window) {
        float xOffset = 190;
        float yOffset = 430;
        for (size_t row = 0; row < keyboardRows; ++row) {
            for (size_t col = 0; col < keyboardLayout[row].length(); ++col) {
                RectangleShape key(Vector2f(tileSize - 10, tileSize - 10));
                key.setPosition(xOffset + col * (tileSize - 5), yOffset + row * (tileSize));

                // Draw the key with appropriate color
                key.setFillColor(Color::Cyan);
                key.setOutlineThickness(2);
                key.setOutlineColor(Color::Black);

                char keyChar = keyboardLayout[row][col];

                // Display the character on top of each key tile
                window.draw(key);         ////////////////////////////////////////////////////////////////////////////////
                drawText(window, std::string(1, keyChar), key.getPosition(), 30, Color::Black);///////////////////////////////////////////

            }
        }
    }
    virtual bool goBack() {
        return back;
    }
};


class Menu {
private:
    bool inMenu;
public:
    Menu() : inMenu(1) {}

    bool INMENU() {
        return inMenu;
    }

    bool navigate(char& option) {
        if (option == 'P') {
            inMenu = 0;
            return 1;
        }
        else if (option == 'Q') {
            inMenu = 0;
            return 1;
        }
        else if (option == 'I') {
            inMenu = false;
            return 1;
        }
    }
};

class GameBoy {
private:
    Menu M;
    Screen* S;
    bool play;
    bool instructions;
    bool Quit;
    bool inGame;
    Game* currentGame;
    char option;
    RectangleShape groundRectangle;
    RectangleShape takenameBackground;
    Texture backgroundTexture;
    Sprite backgroundSprite;
    Texture buttonsTexture;
    Sprite buttonsSprite;
    SoundBuffer dramaticStartBuffer;  // Buffer for the dramatic start sound
    Sound dramaticStartSound;
    string gameName;
    Texture instructionsTexture;  // Texture for instructions
    Sprite instructionsSprite;
    Text instructionsText;
    Font font;


public:
    GameBoy() : play(false), Quit(0), instructions(0), inGame(0), M() {
        currentGame = NULL;
        S = new Screen;
        groundRectangle.setSize(Vector2f(S->getResolutionX(), S->getResolutionY()));
        groundRectangle.setPosition(0, 0);
        groundRectangle.setFillColor(Color(254, 149, 146, 255));
        // Set up instructions text
        if (!font.loadFromFile("Images/arial.ttf")) { // Replace "arial.ttf" with your font file
            cerr << "Failed to load font!" << endl;
            exit(EXIT_FAILURE);
        }
        instructionsText.setFont(font);
        instructionsText.setCharacterSize(24);
        instructionsText.setFillColor(Color::White);
        instructionsText.setPosition(50, 50);  // Position where instructions will appear
        instructionsText.setString(
            "Welcome to *GameBoy*: Your Ultimate Gaming Hub!\n\n"
            "Embark on an exciting journey with three thrilling games:\n\n"
            "Hangman: Test your word-guessing skills and save the stickman!\n"
            "Snake: Relive the classic arcade fun and grow your snake to glory.\n"
            "Wordle*: Challenge your brain with a word-solving puzzle frenzy!\n\n"
            "How to Play:\n"
            "1?Select a game from the menu.\n"
            "2?Follow the on-screen instructions.\n"
            "3? Enjoy hours of endless entertainment!\n\n"
            "Pro Tip: Click anywhere to return to the main menu. Let's play!"
        );

    }

    void start() {
        RenderWindow window(VideoMode(S->getResolutionX(), S->getResolutionY()), "GameBoy", Style::Close | Style::Titlebar);
        window.setPosition(Vector2i(500, 200));

        if (!dramaticStartBuffer.loadFromFile("game.wav")) {
            cerr << "Failed to load dramatic start sound!" << endl;
            exit(EXIT_FAILURE);
        }
        dramaticStartSound.setBuffer(dramaticStartBuffer);
        dramaticStartSound.play();

        FloatRect button1Bounds(360, 60, 250, 60);
        FloatRect button2Bounds(360, 165, 250, 60);
        FloatRect button5Bounds(360, 500, 250, 70);
        FloatRect HangmanBounds(10, 120, 270, 320);
        FloatRect SnakeBounds(350, 120, 270, 320);
        FloatRect WordleBounds(680, 120, 270, 320);


        while (window.isOpen()) {
            Event e;
            while (window.pollEvent(e)) {
                if (e.type == Event::Closed) {
                    window.close();
                }
                if (M.INMENU()) {
                    if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) {
                        Vector2i mousePos = Mouse::getPosition(window);
                        if (button1Bounds.contains(mousePos.x, mousePos.y)) {
                            option = 'P';
                            play = M.navigate(option);
                        }
                        if (button2Bounds.contains(mousePos.x, mousePos.y)) {
                            option = 'I';
                            cout << "Hello";
                            instructions = M.navigate(option);
                        }
                        else if (button5Bounds.contains(mousePos.x, mousePos.y)) {
                            option = 'Q';
                            Quit = M.navigate(option);
                        }
                    }
                }
                
                if (play) {
                    if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) {
                        Vector2i mousePos = Mouse::getPosition(window);
                        if (SnakeBounds.contains(mousePos.x, mousePos.y)) {
                            play = false;
                            inGame = true;
                            if (currentGame != NULL) {
                                delete currentGame;
                            }
                            currentGame = new SnakeGame;  // Assuming SnakeGame is the correct class
                            if (!dramaticStartBuffer.loadFromFile("snake.wav")) {
                                cerr << "Failed to load dramatic start sound!" << endl;
                                exit(EXIT_FAILURE);
                            }
                            dramaticStartSound.setBuffer(dramaticStartBuffer);
                            dramaticStartSound.play();
                            gameName = "Snake Game";
                        }
                        else if (HangmanBounds.contains(mousePos.x, mousePos.y)) {
                            play = false;
                            inGame = true;
                            if (currentGame != NULL) {
                                delete currentGame;
                            }
                            currentGame = new HangmanGame;
                            dramaticStartSound.stop();
                        }
                        else if (WordleBounds.contains(mousePos.x, mousePos.y)) {
                            play = false;
                            inGame = true;
                            if (currentGame != NULL) {
                                delete currentGame;
                            }
                            currentGame = new WordleGame;  // Assuming WordleGame is the correct class

                        }
                    }
                }
            }

            window.clear();

            if (M.INMENU()) {
                window.draw(groundRectangle);
                if (!buttonsTexture.loadFromFile("Images/buttons.png")) {
                    cerr << "Failed to load texture!" << endl;
                }
                else {
                    buttonsSprite.setTexture(buttonsTexture);
                    buttonsSprite.setPosition(340, 30);
                    window.draw(buttonsSprite);
                }
            }
            else if (play) {
                if (!backgroundTexture.loadFromFile("Images/background.jpg")) {
                    cerr << "Failed to load texture!" << endl;
                }
                else {
                    backgroundSprite.setTexture(backgroundTexture);
                    Vector2u textureSize = backgroundTexture.getSize();
                    float scaleX = (float)(S->getResolutionX()) / textureSize.x;
                    float scaleY = (float)(S->getResolutionY()) / textureSize.y;
                    backgroundSprite.setScale(scaleX, scaleY);
                    window.draw(backgroundSprite);
                }

            }
            else if (instructions) {
                window.draw(groundRectangle);
                window.draw(instructionsText);  // Display the instructions text
            }
            else if (Quit) {
                window.close();
            }
            else if (inGame) {
                currentGame->startGame(e, window);
                if (currentGame->goBack()) {
                    play = true;
                    inGame = false;
                }

            }
            window.display();
        }
    }

    ~GameBoy() {
        delete currentGame;
        currentGame = nullptr;
        delete S;
    }
};

int main() {
    GameBoy G;
    G.start();
    return 0;
}
